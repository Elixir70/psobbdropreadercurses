#pragma once
#define UNITXT_POINTER      0xA9CD50
#define SPECIAL_BASE_ID     0x5E4CBB

uint32_t
ReadUnitxtID(
    ctx_t  *pctx,
    item_t *item);

void *
ReadUnitxtEntry(
    ctx_t    *pctx,
    uint32_t  group,
    uint32_t  index,
    LPVOID    buffer,
    size_t    bufsize);

void
ReadItemNameByID(
    ctx_t    *pctx,
    item_t   *item,
    uint32_t  id,
    LPVOID    buffer,
    size_t    bufsize);

void
ReadItemName(
    ctx_t  *pctx,
    item_t *item,
    LPVOID  buffer,
    size_t  bufsize);

void
ReadSpecialName(
    ctx_t   *pctx,
    item_t  *item);

void
ReadTechniqueName(
    ctx_t   *pctx,
    item_t  *item);

void
ReadPhotonBlastName(
    ctx_t   *pctx,
    uint8_t  pb,
    LPVOID   buffer,
    size_t   bufsize);

void
ReadMagColorName(
    ctx_t   *pctx,
    uint8_t  color,
    LPVOID   buffer,
    size_t   bufsize);

void
ReadESName(
    ctx_t   *pctx,
    item_t  *item,
    char    *buffer);

void
ReadESSpecialName(
    ctx_t      *pctx,
    item_t     *item,
    TCHAR      *buffer);
