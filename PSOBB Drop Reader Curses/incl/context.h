#pragma once
#include <Windows.h>
#include <curses.h>
#include <stdint.h>
#include <stdio.h>

// Helper macros
#define tsizeof(x)             (sizeof(x) / sizeof(TCHAR))
#define R8(addr)               ReadMem8   (pctx, (uint32_t)(addr))
#define R16(addr)              ReadMem16  (pctx, (uint32_t)(addr))
#define R32(addr)              ReadMem32  (pctx, (uint32_t)(addr))
#define RWSTR(addr, buffer, n) ReadMemWStr(pctx, (uint32_t)(addr), \
                                           (LPVOID)(buffer), (SIZE_T)(n))
#define printwr(...)   do { printw(__VA_ARGS__); refresh(); } while (0);
#define printwerr(...) do { clear(); printw( __VA_ARGS__ ); refresh(); } while (0);
typedef struct ctx
{
    bool      cmdLinePid;
    DWORD     psobbPid;
    HANDLE    hPSOBB;
    WINDOW   *pwindow;
    uint32_t  lastDisplayCount;
    uint32_t  noItemsCount;
    bool      readMemError; 
    bool      hasColors;
} ctx_t;

uint8_t 
ReadMem8(
    ctx_t    *pctx,
    uint32_t  addr);

uint16_t 
ReadMem16(
    ctx_t    *pctx,
    uint32_t  addr);

uint32_t 
ReadMem32(
    ctx_t    *pctx,
    uint32_t  addr);

TCHAR *
ReadMemWStr(
    ctx_t    *pctx,
    uint32_t  addr,
    LPVOID    lpBuffer,
    SIZE_T    bufSize);

int 
ReadMem(
    ctx_t    *pctx,
    uint32_t  addr,
    LPVOID    lpBuffer,
    SIZE_T    nSize,
    SIZE_T   *poutRead);

int 
InitializeCurses(
    ctx_t *pctx);

int 
CloseCurses(
    ctx_t *pctx);

int
OpenPSOBBHandle(
    ctx_t *pctx);

int
ClosePSOBBHandle(
    ctx_t *pctx);