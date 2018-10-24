#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <io.h>
#include <queue> // ����
#include <mutex>
#include <thread>
#include <condition_variable>
#include "md5.h"
using namespace std;

#define CPU_CORE_NUM thread::hardware_concurrency()  // �߳��� - ȡ CPU����

#define OUT_MD5_FILE "./md5file.txt"

#define READ_DATA_SIZE	1024
#define MD5_SIZE		16
#define MD5_STR_LEN		(MD5_SIZE * 2)


class md5sum
{
public:
	md5sum();
	int glob(const char* pattern); // Ŀ¼�ļ�����
	void md5file();
public:
	static long long file_count;      // �ļ�����ͳ��

private:
	int Compute_file_md5(const char *file_path, char *md5_str);  // �����ļ�MD5ֵ

private:
	queue<string> fileList;           // �ļ�����
	static long long thread_operand;  // �̲߳�����
	static bool globDone;             // �����������ļ�����ɱ��
	static recursive_mutex rec_mutex; // �ݹ���
	static mutex _mutex;
	static condition_variable cv;
};

