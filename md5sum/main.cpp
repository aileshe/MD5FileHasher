#define _CRT_SECURE_NO_WARNINGS
#include "md5sum.h"

int main(void)
{
	cout << "| Author: Dejan - QQ: 673008865 \n"
		"| 批量生成文件MD5值工具/批量效验文件MD5值工具 v0.1" << endl << endl << endl;

	string path;
	cout << "请输入路径(匹配规则) :";
	cin >> path;

	md5sum* md = new md5sum();

	thread *thr[4] = { nullptr };

	if (!path.empty())
	{
		for (char& ch : path) if (ch == '\\') ch = '/';
		thr[0] = new thread(&md5sum::glob, md, path.c_str());
	}


	// worker 线程同步处理 * 3
	for (int i = 1; i < CPU_CORE_NUM; ++i)
	{
		thr[i] = new thread(&md5sum::md5file, md);
	}
	for (int i = 0; i < CPU_CORE_NUM; ++i)
	{
		thr[i]->join();
	}
	for (int i = 0; i < CPU_CORE_NUM; ++i)
	{
		delete thr[i];
		thr[i] = NULL;
	}

	cout << "\n文件个数:" << md5sum::file_count<<"\t" <<"效验完成!! "<< endl;

	system("pause");
	return 0;
}




