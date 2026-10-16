#pragma once
#include<functional>

extern "C" {
	typedef struct _ProgressData_t
	{
		int percent;
		long long done;
		long long total;
		wchar_t message[256];
	}ProgressData_t, * PProgressData_t;

	//typedef void(*ProgressCallback)(int percent, long long done, long long total);
	typedef void (*ProgressCallback)(ProgressData_t progressData);

	int ExtractRAR(
		const wchar_t* rarFile,
		const wchar_t* outDir,
		ProgressCallback progressCallback
	);

	int ExtractRARSpecified(
		const wchar_t* rarFile,
		const wchar_t* outDir,
		const wchar_t* fileArray[],
		int fileCount,
		ProgressCallback progressCallback);
}