#pragma once
#include <stdint.h>
#include <tchar.h>
#include "context.h"

#define MESETA_STR _T("Meseta")

#define PLAYER_INDEX        0xA9C4F4
#define PLAYER_ARRAY        0xA94254

#define ITEM_ARRAY          0xA8D81C
#define ITEM_ARRAY_COUNT    0xA8D820


#define ITEM_ID             0xD8
#define ITEM_OWNER          0xE4
#define ITEM_CODE           0xF2
#define ITEM_EQUIPPED       0x190
#define ITEM_KILLS          0xE8
#define ITEM_WRAPPED        0xDC
#define ITEM_WEP_GRIND      0x1F5
#define ITEM_WEP_SPECIAL    0x1F6
#define ITEM_WEP_STATS      0x1C8
#define ITEM_FRM_SLOTS      0x1B8
#define ITEM_FRM_DFP        0x1B9
#define ITEM_FRM_EVP        0x1BA
#define ITEM_BARRIER_DFP    0x1E4
#define ITEM_BARRIER_EVP    0x1E5
#define ITEM_UNIT_MOD       0x1DC
#define ITEM_MAG_STATS      0x1C0
#define ITEM_MAG_PB_HAS     0x1C8
#define ITEM_MAG_PB         0x1C9
#define ITEM_MAG_COLOR      0x1CA
#define ITEM_MAG_SYNC       0x1BE
#define ITEM_MAG_IQ         0x1BC
#define ITEM_MAG_TIMER      0x1B4
#define ITEM_TOOL_STACK     0x104
#define ITEM_TECH_TYPE      0x108
#define ITEM_MESETA_AMOUNT  0x100

typedef struct weapon
{
    uint8_t  grind;
    uint8_t  specialID;
    uint8_t  esSpecialID; 
    TCHAR    special[64];
    char     esName[10]; // note it's char 
    bool     isES;
    bool     untekked;
    bool     wrapped;
    uint16_t kills;
    uint16_t attrs[6]; // 0 unused, 1..5 used by native..hit
} weapon_t;

typedef struct guard
{
    uint8_t slots;
    uint8_t dfp;
    uint8_t dfpMax;
    uint8_t evp;
    uint8_t evpMax;
} guard_t;

typedef struct barrier
{
    uint8_t dfp;
    uint8_t dfpMax;
    uint8_t evp;
    uint8_t evpMax;
} barrier_t;

typedef struct unit
{
    uint8_t  mod;
    uint16_t kills;
} unit_t;

typedef struct tool
{
    uint8_t stack;
} tool_t;

typedef struct mag
{
    uint8_t color;
    TCHAR   colorName[32];
    double  def;
    double  pow;
    double  dex;
    double  mind;
    uint8_t iq;
    uint8_t synchro;
    uint8_t pbL;
    uint8_t pbM;
    uint8_t pbR;
    TCHAR   pbLName[32];
    TCHAR   pbMName[32];
    TCHAR   pbRName[32];
} mag_t;

typedef struct tech
{
    uint8_t level;
} tech_t;

#define WEAPON  1
#define GUARD   2
#define BARRIER 3
#define UNIT    4
#define TOOL    5
#define MAG     6
#define TECH    7
#define MESETA  8

typedef struct item
{
    uint8_t  type;
    uint32_t addr;
    uint8_t  data[16];
    uint32_t hex;
    uint8_t  equipped;
    uint32_t unitxtID;
    TCHAR    name[512];
    union
    {
        weapon_t  weapon;
        guard_t   guard;
        barrier_t barrier;
        unit_t    unit;
        tool_t    tool;
        mag_t     mag;
        tech_t    tech;
        uint32_t  meseta;
    } d;
} item_t;

void 
ReadFloorItems(
    ctx_t   *pctx, 
    item_t  *items,
    int32_t  max,
    int32_t *read);

void 
ReadFloorItem(
    ctx_t    *pctx, 
    uint32_t  addr, 
    item_t   *item);

void
ParseWeapon(
    ctx_t   *pctx,
    item_t  *item);

void
ParseGuard(
    ctx_t   *pctx,
    item_t  *item);

void
ParseBarrier(
    ctx_t   *pctx,
    item_t  *item);

void
ParseUnit(
    ctx_t   *pctx,
    item_t  *item);

void
ParseMag(
    ctx_t   *pctx,
    item_t  *item);

void
ParseTool(
    ctx_t   *pctx,
    item_t  *item);

void
ParseTechnique(
    ctx_t   *pctx,
    item_t  *item);

void
ParseMeseta(
    ctx_t   *pctx,
    item_t  *item);

void
ReadFloorItem(
    ctx_t    *pctx,
    uint32_t *addr,
    item_t   *item);