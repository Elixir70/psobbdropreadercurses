#pragma once

#include <stdint.h>
#include "context.h"

#define PMT_POINTER         0xA8DC94
#define PMT_WEP_OFFSET      0x00
#define PMT_ARMOR_OFFSET    0x04
#define PMT_UNIT_OFFSET     0x08
#define PMT_TOOL_OFFSET     0x0C
#define PMT_MAG_OFFSET      0x10

#define PMT_WEAPON_SIZE     44
#define PMT_GUARD_SIZE      32
#define PMT_UNIT_SIZE       20
#define PMT_MAG_SIZE        28
#define PMT_TOOL_SIZE       24

typedef struct pmtWeapon
{
    uint32_t id;
    uint16_t model;
    uint16_t texture;
    uint32_t teamPoints;
    struct
    {
        uint16_t _class;
        uint16_t atpMin;
        uint16_t atpMax;
        uint16_t atpReq;
        uint16_t mstReq;
        uint16_t ataReq;
        uint16_t mst;
        uint8_t  grind;
        int8_t   photonColor;
        uint8_t  special;
        uint8_t  ata;
        uint8_t  statBoost;
        uint8_t  projectile;
        int8_t   photonTrail1X;
        int8_t   photonTrail1Y;
        int8_t   photonTrail2X;
        int8_t   photonTrail2Y;
        int8_t   photonType;
        int8_t   unk1;
        int8_t   unk2;
        int8_t   unk3;
        int8_t   unk4;
        int8_t   unk5;
        int8_t   techBoost;
        uint8_t  attack;
    } w;
} pmtWeapon_t;

typedef struct pmtGuard
{
    uint32_t id;
    uint16_t model;
    uint16_t texture;
    uint32_t teamPoints;
    struct
    {
        uint16_t   dfp;
        uint16_t   evp;
        uint8_t    blockParticle;
        uint8_t    blockEffect;
        uint16_t   _class;
        uint8_t    level;
        int8_t     efr;
        int8_t     eth;
        int8_t     eic;
        int8_t     edk;
        int8_t     elt;
        uint8_t    dfpMax;
        uint8_t    evpMax;
        uint8_t    statBoost;
        uint8_t    techBoost;
        uint8_t    unk1;
        uint8_t    unk2;
    } g;
} pmtGuard_t;

uint32_t
GetPMTAddress(
    ctx_t   *pctx,
    uint8_t  type,
    uint8_t  group,
    uint8_t  index);

void 
ReadWeaponPMTData(
    ctx_t       *pctx, 
    item_t      *item, 
    pmtWeapon_t *pmtw);

void
ReadGuardPMTData(
    ctx_t       *pctx, 
    item_t      *item, 
    pmtGuard_t  *pmtg);
