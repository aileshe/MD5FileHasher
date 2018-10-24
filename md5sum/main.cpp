#define _CRT_SECURE_NO_WARNINGS
#include "md5sum.h"

int main(void)
{
	cout << "| Author: Dejan - QQ: 673008865 \n"
		"| ���������ļ�MD5ֵ����/����Ч���ļ�MD5ֵ���� v0.1" << endl << endl << endl;

	string path;
	cout << "������·��(ƥ�����) :";
	cin >> path;

	md5sum* md = new md5sum();

	thread *thr[4] = { nullptr };

	if (!path.empty())
	{
		for (char& ch : path) if (ch == '\\') ch = '/';
		thr[0] = new thread(&md5sum::glob, md, path.c_str());
	}


	// worker �߳�ͬ������ * 3
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

	cout << "\n�ļ�����:" << md5sum::file_count<<"\t" <<"Ч�����!! "<< endl;

	system("pause");
	return 0;
}




