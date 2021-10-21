#include "pch.h"
//#include "stdafx.h"
#include "EvryThng.h"
#include <stdlib.h>

#define BUF_SIZE 256
static VOID type(LPTSTR, LPTSTR);

using namespace System;

int _tmain(DWORD argc, LPTSTR argv[])
/* ������� ��������� ������� ��� ������� ����� �� ��������� ������.
����������� ��������������� ��������� ���� � ������� ��������. */
{
    HANDLE hTempFile;
    BOOL prov;
    TCHAR outFile[MAX_PATH + 100];
    SECURITY_ATTRIBUTES StdOutSA =
        /* ����� ������� ��� ������������ �����������. */
    { sizeof
    (SECURITY_ATTRIBUTES), NULL, TRUE };
    TCHAR CommandLine[MAX_PATH + 100];
    STARTUPINFO StartUpSearch, StartUp;
    PROCESS_INFORMATION ProcessInfo;
    DWORD iProc, ExCode;
    HANDLE* hProc;
    /* ��������� �� ������ ������������ ���������. */
    typedef struct
    {
        TCHAR TempFile[MAX_PATH];
    }
    PROCFILE;
    PROCFILE* ProcFile;
    /* ��������� �� ������ ���� ��������� ������. */
    /* ���������� ������� ��� ������� ��������� �������� ������ � ���
    ��������, ������� ����� �������� ����������. */
    GetStartupInfo(&StartUpSearch);
    GetStartupInfo(&StartUp);
    /* ��������������� ����� ��� ������� �������� ������ ���������,
    ���������� ���������� �������� � ��� ���������� �����. */
    ProcFile = (PROCFILE*)malloc((argc - 2) * sizeof(PROCFILE));
    hProc = (HANDLE*)malloc((argc - 2) * sizeof(HANDLE));
    /* ������� ��������� ������� "find" ��� ������� ����� �� ���������
    ������*/
    for (iProc = 0; iProc < argc - 2; iProc++)
    {
        /* ������� ��������� ������ ���� find argv [1] argv [iProc + 2] */
        _stprintf(CommandLine, _T("%s\"%s\" %s"),
            _T("find "), argv[1], argv[iProc + 2]);
        _tprintf(_T("%s\n"), CommandLine);
        GetTempFileName(_T("."), _T("gtm"), 0, ProcFile[iProc].TempFile);
            /* ��������� ����������� ����� ��� �������� ������. */
            hTempFile = CreateFile(ProcFile[iProc].TempFile, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, &StdOutSA,CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        /* ���� ���������� ����������� */

        StartUpSearch.dwFlags = STARTF_USESTDHANDLES;
        StartUpSearch.hStdOutput = hTempFile;
        StartUpSearch.hStdError = hTempFile;
        StartUpSearch.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
        /* ������� ������� ��� ���������� ��������� ������. */
        CreateProcess(NULL, CommandLine, NULL, NULL, TRUE, 0, NULL, NULL, &StartUpSearch, &ProcessInfo);
        /* ��������� �������� ����������� */
        CloseHandle(hTempFile);
        CloseHandle(ProcessInfo.hThread);
        /* ��������� ���������� ��������. */
        hProc[iProc] = ProcessInfo.hProcess;
    }
    /* ��� �������� �����������, �������� �� ����������. */
    for (iProc = 0; iProc < argc - 2; iProc += MAXIMUM_WAIT_OBJECTS)
        WaitForMultipleObjects(min(MAXIMUM_WAIT_OBJECTS, argc - 2 - iProc), &hProc[iProc], TRUE, INFINITE);
    /* �������������� ����� ���������� �� ����������� ����� �
    ������� "type". */
    for (iProc = 0; iProc < argc - 2; iProc++)
    {
        printf("Proc= %d\n", iProc);
        prov = GetExitCodeProcess(hProc[iProc], &ExCode);
        if (ExCode != 0) DeleteFile(ProcFile[iProc].TempFile);
        if (GetExitCodeProcess(hProc[iProc], &ExCode) && ExCode == 0)
        {
            /* ������� ������ - �������� ����������. */
            if (argc > 3) _tprintf(_T("%s : \n"), argv[iProc + 2]);
            fflush(stdout);
            /* ��������� ��� ������������� ������������ ������
            ����������� ���������� */
            _stprintf(outFile, _T("%s"), ProcFile[iProc].TempFile);
            type(argv[iProc + 2], (LPTSTR)outFile);
            _stprintf(CommandLine, _T("%s%s"),
                _T("type "), ProcFile[iProc].TempFile);
            _tprintf(_T("%s\n"), CommandLine);
            CreateProcess(NULL, CommandLine, NULL, NULL,
                TRUE, 0, NULL, NULL, &StartUp, &ProcessInfo);
            WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
            CloseHandle(ProcessInfo.hProcess);
            CloseHandle(ProcessInfo.hThread);
        }
        CloseHandle(hProc[iProc]);
        /*DeleteFile (ProcFile [iProc].TempFile); */
    }
    free(ProcFile);
    free(hProc);
    return 0;
}
static VOID type(LPTSTR hInFile, LPTSTR hOutFile)
{
    CopyFile(hInFile, hOutFile, FALSE);
    return;
}
