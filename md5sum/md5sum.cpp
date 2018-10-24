#define _CRT_SECURE_NO_WARNINGS
#include "md5sum.h"

recursive_mutex md5sum::rec_mutex;
mutex md5sum::_mutex;
condition_variable md5sum::cv;

long long md5sum::file_count = 0;      // 文件个数统计
long long md5sum::thread_operand = 0;  // 线程操作数
bool md5sum::globDone = false;         // 遍历出所有文件已完成标记

/**
* 构造函数
*/
md5sum::md5sum(){}

void md5sum::md5file()
{
	while (1)
	{
		//this_thread::yield(); // 让出小小执行片刻
		this_thread::sleep_for(chrono::milliseconds(5)); // 等待5毫秒
		{
			unique_lock<mutex> lock(_mutex);
			if (thread_operand == file_count && globDone) break;
			cv.wait(lock, [=] {return !fileList.empty(); });

			const char* filename = (fileList.front()).c_str();
			char md5_str[MD5_STR_LEN + 1] = { 0 };
			int ret = Compute_file_md5(filename, md5_str);
			if (0 == ret)
			{
				// 写入文件
				ofstream ofs;//文件流对象
				ofs.open(OUT_MD5_FILE, ios::out | ios::app);
				//判断是否打开成功
				if (!ofs.is_open())
				{
					cerr << "打开文件失败!" << endl;
				}
				ofs << filename << "\t" << md5_str << "\n";
				ofs.close();

				// 输出结果到屏幕
				cout << filename << "\t" << md5_str << "\t..OK!" << endl;
			}
			fileList.pop();
			thread_operand++;
			lock.unlock();
		}
	}
}

/**
 * 目录文件遍历
 */
int md5sum::glob(const char* pattern)
{
	const char *ptrEnd = strrchr(pattern, '/');
	const char *suffix_rule = ptrEnd;              // 尾缀规则 /* 或 /*.*
	const int cur_path_len = ptrEnd + 1 - pattern; // 取到字符串最后一个'/'的位置, 包含 '/'
	char *cur_path = new char[cur_path_len + 1];   // 创建字符存储空间, 包含 \0
	memset(cur_path, 0, cur_path_len + 1);         // 初始化空间0, 包含 \0
	strncpy(cur_path, pattern, cur_path_len);      // 拷贝数据
	//cout << cur_path <<endl;    // 当前遍历目录路径
	//cout << suffix_rule <<endl; // 尾缀规则

	long fh = 0; // 查找文件句柄
	_finddata_t fileInfo;

	if ((fh = _findfirst(pattern, &fileInfo)) == -1L) return -1;

	do
	{
		if ((strcmp(fileInfo.name, ".") != 0) && (strcmp(fileInfo.name, "..") != 0))
		{
			if (fileInfo.attrib == _A_SUBDIR) // 目录继续遍历
			{
				//cout << "DIR:" << string(cur_path) + fileInfo.name + suffix_rule << endl;
				glob((string(cur_path) + fileInfo.name + suffix_rule).c_str()); // 递归遍历子级目录
			}
			else // 文件, 则放入队列
			{
				//cout << string(cur_path) + fileInfo.name << endl;
				file_count++;
				{
					lock_guard<recursive_mutex> guard(rec_mutex);
					string filename = string(cur_path) + fileInfo.name;
					fileList.push(filename);
				}
				cv.notify_all(); // 通知所有工作线程
			}
		}
	} while (_findnext(fh, &fileInfo) == 0);

	// 资源释放
	_findclose(fh);
	delete[] cur_path;

	// 到这里 1号线程已经执行完毕了
	globDone = true;

	return 0;
}

/**
 * 计算文件MD5值
 */
int md5sum::Compute_file_md5(const char *file_path, char *md5_str)
{
	int i;
	FILE *fp;
	int ret;
	unsigned char data[READ_DATA_SIZE];
	unsigned char md5_value[MD5_SIZE];
	MD5_CTX md5;

	fp = fopen(file_path, "rb");
	if (NULL == fp)
	{
		perror("fopen");
		return -1;
	}

	// init md5
	MD5Init(&md5);

	while (1)
	{
		ret = fread(data, 1, READ_DATA_SIZE, fp);
		if (-1 == ret)
		{
			perror("read");
			return -1;
		}

		MD5Update(&md5, data, ret);

		if (0 == ret || ret < READ_DATA_SIZE)
		{
			break;
		}
	}

	fclose(fp);

	MD5Final(&md5, md5_value);

	char tmp[100] = { 0 };
	memset(md5_str, 0, MD5_STR_LEN + 1);

	for (i = 0; i < MD5_SIZE; i++)
	{
		sprintf(tmp, "%02x", md5_value[i]);
		strcat(md5_str, tmp);
	}
	//md5_str[MD5_STR_LEN] = '\0'; // add end

	return 0;
}

