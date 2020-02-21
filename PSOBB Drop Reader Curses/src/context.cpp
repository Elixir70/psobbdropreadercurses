#include <Windows.h>
#include <stdint.h>
#include <Psapi.h>
#include <Shlwapi.h>
#include <tchar.h>
#include "context.h"

int 
ReadMem(
    ctx_t   *pctx,
    uint32_t addr,
    LPVOID   lpBuffer,
    SIZE_T   nSize,
    SIZE_T  *poutRead)
{
    int rc = 0;
    
    if (pctx->readMemError)
    {
        // Already hit an error earlier. This is here just to force the error up.
        rc = -1;
        goto done;
    }

    if (!ReadProcessMemory(pctx->hPSOBB,
                           (LPCVOID)addr,
                           lpBuffer,
                           nSize,
                           poutRead))
    {
        rc = -1;
        
        // TODO: Diagnostics? 
        pctx->readMemError = true;
    }
          
done:
    return rc;
}

uint8_t 
ReadMem8(
    ctx_t   *pctx,
    uint32_t addr)
{
    uint8_t buffer = 0xDE;
    SIZE_T  dontCare = 0;

    ReadMem(pctx, addr, (LPVOID)&buffer, 1, &dontCare);
    return buffer;
}

uint16_t 
ReadMem16(
    ctx_t   *pctx,
    uint32_t addr)
{
    uint16_t buffer = 0xDEAD;
    SIZE_T  dontCare = 0;

    ReadMem(pctx, addr, (LPVOID)&buffer, 2, &dontCare);
    return buffer;
}

uint32_t 
ReadMem32(
    ctx_t   *pctx,           
    uint32_t addr)
{
    uint32_t buffer = 0xDEADBEEF;
    SIZE_T   dontCare = 0;

    ReadMem(pctx, addr, (LPVOID)&buffer, 4, &dontCare);
    return buffer;
}

TCHAR *
ReadMemWStr(
    ctx_t   *pctx,
    uint32_t addr,
    LPVOID   lpBuffer,
    SIZE_T   bufSize)
{
    SIZE_T   dontCare = 0;

    ReadMem(pctx, addr, lpBuffer, bufSize, &dontCare);
    return (TCHAR *)lpBuffer;
}

int 
InitializeCurses(
    ctx_t *pctx)
{
    int rc = 0;

    if (!(pctx->pwindow = initscr()))
        rc = -1;
    
    pctx->lastDisplayCount = 1; 
    pctx->noItemsCount = 0;
    pctx->hasColors = has_colors();

    if (pctx->hasColors)
    {
        start_color();
        init_pair(1, COLOR_RED, COLOR_BLACK);
        init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    }

    return rc;
}

int 
CloseCurses(
    ctx_t *pctx)
{
    int rc = 0;

    if (pctx->pwindow)
        rc = endwin();

    return rc;
}

int
OpenPSOBBHandle(
    ctx_t *pctx)
{
    int rc = 0;

    if (!pctx->cmdLinePid)
    {
        // Find a psobb.exe process
        DWORD pids[1024] = { 0 };
        DWORD cbNeeded;
        DWORD cProcesses;

        if (!EnumProcesses(pids, sizeof(pids), &cbNeeded))
        {
            printw("Error %i trying to enumerate processes to find running PSOBB.\n", GetLastError());
            refresh();
            rc = -1;
            goto done;
        }

        cProcesses = cbNeeded / sizeof(DWORD);
        for (DWORD i = 0; i < cProcesses && !pctx->psobbPid; ++i)
        {
            if (pids[i])
            {
                HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                              FALSE, pids[i]);
                if (hProcess)
                {
                    TCHAR processPath[MAX_PATH + 1] = _T("");
                    if (!GetModuleFileNameEx(hProcess, 0, processPath, MAX_PATH))
                    {
                        printw("Unexpected error %i when querying for the process file name of process ID %i.\n",
                               GetLastError(), pids[i]);
                        refresh();
                        rc = -1;
                    }
                    else
                    {
                        TCHAR processName[MAX_PATH + 1] = _T("");
                        _tcscpy_s(processName, MAX_PATH, processPath);
                        PathStripPath(processName);
                        if (!_tcscmp(processName, _T("psobb.exe")))
                            pctx->psobbPid = pids[i];
                    }

                    CloseHandle(hProcess);
                }

                if (rc)
                    goto done;
            }
        }
    }
    else
    {
        // Use pid from command line
        pctx->psobbPid = pctx->cmdLinePid;
    }

    if (pctx->psobbPid)
    {
        pctx->hPSOBB = OpenProcess(PROCESS_VM_READ, false, pctx->psobbPid);
        if (!pctx->hPSOBB)
        {
            printw("Error %i opening process handle for PSOBB Process ID %i.\n",
                   GetLastError(),
                   pctx->psobbPid);
            refresh();
            rc = -1;
            goto done;
        }
    }
    else
    {
        rc = -1;
        goto done;
    }

done:
    return rc;
}

int
ClosePSOBBHandle(
    ctx_t *pctx)
{
    int rc = 0;

    if (pctx->hPSOBB)
        CloseHandle(pctx->hPSOBB);
    pctx->hPSOBB = 0;

    return rc;
}
