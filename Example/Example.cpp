#include <iostream>
#include <windows.h>
#include"UnRAR.h"


static void OnExtractRARProgress(
	ProgressData_t progressData)
{

	wprintf(
		L"\r%s %3u%%  %llu / %llu",
		progressData.message,
		progressData.percent,
		progressData.done,
		progressData.total);

	fflush(stdout);
}

int wmain(void)//utf-8 without BOM but with /utf-8 c/c++ cmd line option
{
	setlocale(LC_ALL, "");
	int ret;
	const wchar_t* fileList[] = { L"2679.txt" };
	//ret = ExtractRAR(
	//	L"cl_CL.rar",
	//	L"./",
	//	OnExtractRARProgress);
	ret = ExtractRARSpecified(
		L"cl_CL.rar",
		L"./",
		fileList,
		1,
		OnExtractRARProgress);

	wprintf(L"\n");

	if (ret == 0)
	{
		wprintf(L"解压成功\n");
	}
	else
	{
		wprintf(
			L"解压失败，错误码 = %d\n",
			ret);
	}

	system("pause");
	return ret;
}