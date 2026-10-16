#include"UnRAR.h"
#include <iostream>
#include <Windows.h>
#include"dll.hpp"

struct ExtractState
{
	long long TotalSize;       // 总大小

	long long FinishedSize;    // 已完成文件大小

	long long CurrentSize;     // 当前文件已处理
};

//ExtractState g_ExtractState{};
//std::function<void(int, long long, long long)> g_ProgressCallback = nullptr;
std::function<void(ProgressData_t)> g_ProgressCallback = nullptr;

int CALLBACK CallbackProc(
	UINT msg,
	LPARAM UserData,
	LPARAM P1,
	LPARAM P2
)
{
	switch (msg)
	{

	case UCM_PROCESSDATA:
	{
		// P1 = 数据指针
		// P2 = 数据大小(byte)

		ExtractState* state =
			(ExtractState*)UserData;


		state->CurrentSize += P2;


		long long done =
			state->FinishedSize
			+
			state->CurrentSize;


		int percent =
			(int)(
				done * 100
				/
				state->TotalSize
				);


		//printf(
		//	"\r Extractiing: %d%%",
		//	percent
		//);

		if (g_ProgressCallback)
		{
			ProgressData_t progressData{};
			progressData.percent = percent;
			progressData.done = done;
			progressData.total = state->TotalSize;
			wcscpy_s(progressData.message, 64, L"解压进度");
			g_ProgressCallback(progressData);
			//g_ProgressCallback(percent, done, state->TotalSize);
		}
		else {
			printf("g_ProgressCallback is null \n");
		}

		break;
	}


	case UCM_NEEDPASSWORD:
	{
		// 需要密码

		//strcpy_s(
		//	(char*)P1,
		//	64,
		//	"123456"
		//);

		return 1;
	}


	case UCM_CHANGEVOLUME:
	{
		// 分卷 rar
		break;
	}

	}

	return 1;
}


long long GetTotalSize(const wchar_t* rarFile,
	const wchar_t* outDir)
{
	RAROpenArchiveDataEx archiveData{};

	char rarName[MAX_PATH];

	WideCharToMultiByte(
		CP_ACP,
		0,
		rarFile,
		-1,
		rarName,
		MAX_PATH,
		NULL,
		NULL
	);


	archiveData.ArcName = rarName;
	archiveData.OpenMode = RAR_OM_LIST;


	HANDLE hArc = RAROpenArchiveEx(&archiveData);


	if (hArc == NULL)
	{
		std::cout << "Open Archive failed! \n";
		return -1;
	}


	if (archiveData.OpenResult != 0)
	{
		std::cout << "RAR error:"
			<< archiveData.OpenResult
			<< std::endl;

		RARCloseArchive(hArc);
		return -2;
	}


	long long TotalSize = 0;

	RARHeaderDataEx hd{};

	while (RARReadHeaderEx(hArc, &hd) == 0)
	{
		if (hd.UnpSize > 0)
			TotalSize += hd.UnpSize;

		RARProcessFile(
			hArc,
			RAR_SKIP,
			NULL,
			NULL
		);
	}
	RARCloseArchive(hArc);

	return TotalSize;
}

int ExtractRAR(
	const wchar_t* rarFile,
	const wchar_t* outDir,
	ProgressCallback progressCallback
)
{
	RAROpenArchiveDataEx archiveData{};

	char rarName[MAX_PATH];

	WideCharToMultiByte(
		CP_ACP,
		0,
		rarFile,
		-1,
		rarName,
		MAX_PATH,
		NULL,
		NULL
	);


	archiveData.ArcName = rarName;
	archiveData.OpenMode = RAR_OM_EXTRACT;


	HANDLE hArc = RAROpenArchiveEx(&archiveData);


	if (hArc == NULL)
	{
		std::cout << "Open Archive failed! \n";
		return -1;
	}


	if (archiveData.OpenResult != 0)
	{
		std::cout << "RAR error:"
			<< archiveData.OpenResult
			<< std::endl;

		RARCloseArchive(hArc);
		return -2;
	}


	//RARSetPassword(
	//	hArc,
	//	(char*)"123456"
	//);

	printf("\n RAR Extracting...\n");
	ExtractState g_ExtractState{};
	g_ExtractState.TotalSize = GetTotalSize(rarFile, outDir);
	g_ProgressCallback = progressCallback;

	RARSetCallback(
		hArc,
		CallbackProc,
		(LPARAM)&g_ExtractState
	);

	RARHeaderDataEx header{};


	while (true)
	{

		int ret = RARReadHeaderEx(
			hArc,
			&header
		);


		if (ret != 0)
		{
			break;
		}

		//printf(
		//	"File: %s \n",
		//	header.FileName
		//);

		//std::wcout
		//	<< L"Uncompressing:"
		//	<< header.FileNameW
		//	<< std::endl;



		ret = RARProcessFileW(
			hArc,
			RAR_EXTRACT,
			(wchar_t*)outDir,
			NULL
		);


		if (ret != 0)
		{
			std::cout
				<< "Uncompression failed:"
				<< ret
				<< std::endl;

			RARCloseArchive(hArc);
			return -3;
		}

	}

	//g_ExtractState = {};//清理，防止积累
	RARCloseArchive(hArc);
	return 0;
}
