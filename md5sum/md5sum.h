#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <io.h>
#include <queue> // 队列
#include <mutex>
#include <thread>
#include <condition_variable>
#include "md5.h"
using namespace std;

#define CPU_CORE_NUM thread::hardware_concurrency()  // 线程数 - 取 CPU核数

#define OUT_MD5_FILE "./md5file.txt"

#define READ_DATA_SIZE	1024
#define MD5_SIZE		16
#define MD5_STR_LEN		(MD5_SIZE * 2)


class md5sum
{
public:
	md5sum();
	int glob(const char* pattern); // 目录文件遍历
	void md5file();
public:
	static long long file_count;      // 文件个数统计

private:
	int Compute_file_md5(const char *file_path, char *md5_str);  // 计算文件MD5值

private:
	queue<string> fileList;           // 文件队列
	static long long thread_operand;  // 线程操作数
	static bool globDone;             // 遍历出所有文件已完成标记
	static recursive_mutex rec_mutex; // 递归锁
	static mutex _mutex;
	static condition_variable cv;
};

