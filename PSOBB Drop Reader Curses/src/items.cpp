#include <tchar.h>
#include "items.h"
#include "pmt.h"
#include "unitxt.h"

void 
ReadFloorItems(
    ctx_t   *pctx,      
    item_t  *items,
    int32_t  max,
    int32_t *read)
{
    //uint32_t  myIndex   = R32(PLAYER_INDEX);
    uint32_t  itemCount = R32(ITEM_ARRAY_COUNT);
    uint32_t  itemArray = R32(ITEM_ARRAY);
    
    *read = 0;
    for (int32_t i = itemCount - 1; 
         i >= 0 && (*read) < max && !pctx->readMemError; 
         --i)
    {
        uint32_t oneItem = R32(itemArray + 4 * i);
        if (!oneItem)
            continue;

        int8_t owner = (int8_t)R8(oneItem + ITEM_OWNER);
        if (owner != -1)
            continue;

        ReadFloorItem(pctx, oneItem, &items[*read]);
        ++(*read);
    }
}

void 
ReadFloorItem(
    ctx_t    *pctx,
    uint32_t  addr,
    item_t   *item)
{
    memset(item, 0, sizeof(item_t));
    item->addr = (uint32_t)addr;

    item->data[0] = R8(addr + ITEM_CODE + 0);
    item->data[1] = R8(addr + ITEM_CODE + 1);
    item->data[2] = R8(addr + ITEM_CODE + 2);
    item->data[3] = R8(addr + ITEM_MAG_PB);
    for (int i = 4; i < sizeof(item->data); ++i)
        item->data[i] = 0;

    item->hex = (item->data[0] << 16) +
                (item->data[1] << 8)  +
                (item->data[2]);

    item->unitxtID = ReadUnitxtID(pctx, item);
    item->equipped = (R8(addr + ITEM_EQUIPPED) & 1); // not used for floor obviously
    ReadItemName(pctx, item, item->name, tsizeof(item->name));
   
    if (item->data[0] == 0)
    {
        item->data[3]  = R8(addr + ITEM_WEP_GRIND);
        item->data[4]  = R8(addr + ITEM_WEP_SPECIAL);

        item->data[6]  = R8(addr + ITEM_WEP_STATS + 0);
        item->data[7]  = R8(addr + ITEM_WEP_STATS + 1);
        item->data[8]  = R8(addr + ITEM_WEP_STATS + 2);
        item->data[9]  = R8(addr + ITEM_WEP_STATS + 3);
        item->data[10] = R8(addr + ITEM_WEP_STATS + 4);
        item->data[11] = R8(addr + ITEM_WEP_STATS + 5);

        if (item->data[1] == 0x33 || item->data[1] == 0xAB)
            item->d.weapon.kills = R16(addr + ITEM_KILLS);

        // Weapon
        ParseWeapon(pctx, item);
    }
    else if (item->data[0] == 1)
    {
        if (item->data[1] == 1)
        {
            // Frame
            item->data[5] = R8(addr + ITEM_FRM_SLOTS);
            item->data[6] = R8(addr + ITEM_FRM_DFP);
            item->data[7] = R8(addr + ITEM_FRM_EVP);
            
            ParseGuard(pctx, item);
        }
        else if (item->data[1] == 2)
        {
            // Barrier
            item->data[6] = R8(addr + ITEM_BARRIER_DFP);
            item->data[8] = R8(addr + ITEM_BARRIER_EVP);
            
            ParseBarrier(pctx, item);
        }
        else if (item->data[1] == 3)
        {
            // Unit
            item->data[6] = R8(addr + ITEM_UNIT_MOD + 0);
            item->data[8] = R8(addr + ITEM_UNIT_MOD + 1);

            ParseUnit(pctx, item);
        }
    }
    else if (item->data[0] == 2)
    {
        // Mag
        item->data[3] = R8(addr + ITEM_MAG_PB);

        for (int i = 0; i < 8; ++i)
            item->data[4 + i] = R8(addr + ITEM_MAG_STATS + i);

        item->data[12] = R8(addr + ITEM_MAG_SYNC);
        item->data[13] = R8(addr + ITEM_MAG_IQ);
        item->data[14] = R8(addr + ITEM_MAG_PB_HAS);
        item->data[15] = R8(addr + ITEM_MAG_COLOR);

        ParseMag(pctx, item);
    }
    else if (item->data[0] == 3)
    {
        if (item->data[1] == 2)
        {
            // Technique
            item->data[4] = R8(addr + ITEM_TECH_TYPE);
            ParseTechnique(pctx, item);
        }
        else
        {
            // Tool
            item->data[5] = R32(addr + ITEM_TOOL_STACK) ^
                            (addr + ITEM_TOOL_STACK);

            ParseTool(pctx, item);
        }
    }
    else if (item->data[0] == 4)
    {
        // Meseta
        for (int i = 0; i < 4; ++i)
            item->data[12 + i] = R8(addr + ITEM_MESETA_AMOUNT + i);

        ParseMeseta(pctx, item);
    }
}

void
ParseWeapon(
    ctx_t   *pctx,
    item_t  *item)
{
    item->type = WEAPON;
    weapon_t    *weapon = &(item->d.weapon);
    pmtWeapon_t  pmtWeapon;

    memset(&pmtWeapon, 0, sizeof(pmtWeapon_t));
    ReadWeaponPMTData(pctx, item, &pmtWeapon); 
        
    if (item->data[4] > 0xBF)
    {
        weapon->wrapped  = true;
        weapon->untekked = true;
    }
    else if (item->data[4] > 0x7F)
        weapon->untekked = true;
    else if (item->data[4] > 0x3F)
        weapon->wrapped = true;

    weapon->grind = item->data[3];

    weapon->isES = (item->data[1] >= 0x70 && item->data[1] < 0x89) ||
                   (item->data[1] >= 0xA5 && item->data[1] < 0xAA);
    
    if (weapon->isES)
    {
        weapon->specialID = item->data[2];
        ReadESName(pctx, item, weapon->esName);
        ReadESSpecialName(pctx, item, weapon->special);
    }
    else
    {
        weapon->specialID = item->data[4] % 64;

        // Use pmt special
        if (!weapon->specialID)
            weapon->specialID = pmtWeapon.w.special;

        if (weapon->specialID)
            ReadSpecialName(pctx, item);

        for (int i = 0; i < 3; ++i)
        {
            int idx = 6 + 2 * i;
            if (item->data[idx] > 0 && item->data[idx] <= 5)
                weapon->attrs[item->data[idx]] += item->data[idx + 1];
        }
    }

    
}

void
ParseGuard(
    ctx_t   *pctx,
    item_t  *item)
{
    guard_t    *guard = &item->d.guard; 
    pmtGuard_t  pmtGuard;

    memset(&pmtGuard, 0, sizeof(pmtGuard_t));
    ReadGuardPMTData(pctx, item, &pmtGuard);

    item->type    = GUARD;
    guard->slots  = item->data[5];
    guard->dfp    = item->data[6];
    guard->evp    = item->data[8];
    guard->dfpMax = pmtGuard.g.dfpMax;
    guard->evpMax = pmtGuard.g.evpMax;
}

void
ParseBarrier(
    ctx_t   *pctx,
    item_t  *item)
{
    barrier_t *barrier = &item->d.barrier;
    pmtGuard_t pmtGuard;

    memset(&pmtGuard, 0, sizeof(pmtGuard_t));
    ReadGuardPMTData(pctx, item, &pmtGuard);

    item->type      = BARRIER;
    barrier->dfp    = item->data[6];
    barrier->evp    = item->data[8];
    barrier->dfpMax = pmtGuard.g.dfpMax;
    barrier->evpMax = pmtGuard.g.evpMax;
}

void
ParseUnit(
    ctx_t   *pctx,
    item_t  *item)
{
    unit_t *unit = &item->d.unit;
    uint8_t mod = item->data[6];

    item->type = UNIT;

    if (mod > 127) 
        mod = mod - 256;
    
    unit->mod = 0;
    if (mod == 0)
        unit->mod = 0;
    else if (mod == 1)
        unit->mod = 1;
    else if (mod > 1)
        unit->mod = 2;
    else if (mod == -1)
        unit->mod = -1;
    else if (mod < -1)
        unit->mod = -2;
}

void
ParseMag(
    ctx_t   *pctx,
    item_t  *item)
{
    mag_t *mag = &item->d.mag;

    item->type = MAG;
    mag->def   = ((item->data[5] << 8) + (item->data[4])) / 100.0;
    mag->pow   = ((item->data[7] << 8) + (item->data[6])) / 100.0;
    mag->dex   = ((item->data[9] << 8) + (item->data[8])) / 100.0;
    mag->mind  = ((item->data[11] << 8) + (item->data[10])) / 100.0;
    mag->color = item->data[15];
    ReadMagColorName(pctx, mag->color, mag->colorName, 
                     tsizeof(mag->colorName));

    if (item->data[14] & 1)
        mag->pbM = item->data[3] & 0x7;
    else
        mag->pbM = -1;
    if (item->data[14] & 2)
        mag->pbR = (item->data[3] & 0x38) >> 3;
    else 
        mag->pbR = -1;

    if (item->data[14] & 4)
    {
        uint8_t pb = (item->data[3] & 0xC0) >> 6;
        uint8_t pbs[8] = { 0 };

        pbs[mag->pbR >= 0 ? mag->pbR : 0] = 1; 
        pbs[mag->pbM >= 0 ? mag->pbM : 0] = 1;
        for (int i = 0; i < 6; ++i)
        {
            if (!pbs[i])
            {
                if (pb)
                    --pb;
                else
                {
                    mag->pbL = i;
                    break;
                }
            }
        }


    }
    else
        mag->pbL = -1;

    if (mag->pbL > 0)
        ReadPhotonBlastName(pctx, mag->pbL, (LPVOID)mag->pbLName,
                            tsizeof(mag->pbLName));
    if (mag->pbM > 0)
        ReadPhotonBlastName(pctx, mag->pbM, (LPVOID)mag->pbMName,
                            tsizeof(mag->pbMName));
    if (mag->pbR > 0)
        ReadPhotonBlastName(pctx, mag->pbR, (LPVOID)mag->pbRName,
                            tsizeof(mag->pbRName));
}

void
ParseTool(
    ctx_t   *pctx,
    item_t  *item)
{
    item->type = TOOL;
    item->d.tool.stack = item->data[5];
}

void
ParseTechnique(
    ctx_t   *pctx,
    item_t  *item)
{
    item->type = TECH;
    item->hex = (5 << 16) + (item->data[4] << 8) + item->data[3];
    ReadTechniqueName(pctx, item);
    item->d.tech.level = item->data[2] + 1;
}


void
ParseMeseta(
    ctx_t   *pctx,
    item_t  *item)
{
    item->type = MESETA;
    _tcscpy_s(item->name, tsizeof(item->name), MESETA_STR);
    item->d.meseta = (item->data[15] << 24) +
                     (item->data[14] << 16) +
                     (item->data[13] <<  8) + 
                     (item->data[12]);
}

