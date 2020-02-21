#include "items.h"
#include "pmt.h"

// Get in memory address of an item in the PMT.
uint32_t
GetPMTAddress(
    ctx_t   *pctx,
    uint8_t  type,
    uint8_t  group,
    uint8_t  index)
{
    uint32_t ret = 0;
    uint32_t pmtPointer = R32(PMT_POINTER);

    if (!pmtPointer)
        goto done;

    if (type == 0)
    {
        // Weapon
        uint32_t wepAddr = R32(pmtPointer + PMT_WEP_OFFSET);
        if (!wepAddr)
            goto done;

        uint32_t groupAddr = wepAddr + 8 * group;
        uint32_t count = R32(groupAddr);
        uint32_t itemAddr = R32(groupAddr + 4);
        if (index < count && itemAddr)
            ret = itemAddr + PMT_WEAPON_SIZE * index;
    }
    else if (type == 1)
    {
        if (group == 1 || group == 2)
        {
            // Frame/Barrier
            uint32_t guardAddr = R32(pmtPointer + PMT_ARMOR_OFFSET);
            if (!guardAddr)
                goto done;

            uint32_t groupAddr = guardAddr + 8 * (group - 1);
            uint32_t count = R32(groupAddr);
            uint32_t itemAddr = R32(groupAddr + 4);
            if (index < count && itemAddr)
                ret = itemAddr + PMT_GUARD_SIZE * index;
        }
        else if (group == 3)
        {
            // Unit
            uint32_t unitAddr = R32(pmtPointer + PMT_UNIT_OFFSET);
            uint32_t count = R32(unitAddr);
            uint32_t itemAddr = R32(unitAddr + 4);
            if (index < count && itemAddr)
                ret = itemAddr + PMT_UNIT_SIZE * index;
        }
    }
    else if (type == 2)
    {
        // Mag
        uint32_t magAddr = R32(pmtPointer + PMT_MAG_OFFSET);
        uint32_t count = R32(magAddr);
        uint32_t itemAddr = R32(magAddr + 4);
        if (group < count && itemAddr)
            ret = itemAddr + PMT_MAG_SIZE * group;
    }
    else if (type == 3)
    {
        // Tool
        uint32_t toolAddr = R32(pmtPointer + PMT_TOOL_OFFSET);
        if (!toolAddr)
            goto done;
        uint32_t groupAddr = toolAddr + 8 * group;
        uint32_t count = R32(groupAddr);
        uint32_t itemAddr = R32(groupAddr + 4);
        if (index < count && itemAddr)
            ret = itemAddr + PMT_TOOL_SIZE * index;
    }
done:
    return ret;
}

void
ReadWeaponPMTData(
    ctx_t       *pctx,
    item_t      *item,
    pmtWeapon_t *pmtw)
{
    uint32_t     addr = GetPMTAddress(pctx, item->data[0], item->data[1], item->data[2]);

    if (!addr)
        return;

    pmtw->id              = R32(addr + 0);
    pmtw->model           = R16(addr + 4);
    pmtw->texture         = R16(addr + 6);
    pmtw->teamPoints      = R32(addr + 8);
    pmtw->w._class        = R16(addr + 12);
    pmtw->w.atpMin        = R16(addr + 14);
    pmtw->w.atpMax        = R16(addr + 16);
    pmtw->w.atpReq        = R16(addr + 18);
    pmtw->w.mstReq        = R16(addr + 20);
    pmtw->w.ataReq        = R16(addr + 22);
    pmtw->w.mst           = R16(addr + 24);
    pmtw->w.grind         = R8(addr + 26);
    pmtw->w.photonColor   = R8(addr + 27);
    pmtw->w.special       = R8(addr + 28);
    pmtw->w.ata           = R8(addr + 29);
    pmtw->w.statBoost     = R8(addr + 30);
    pmtw->w.projectile    = R8(addr + 31);
    pmtw->w.photonTrail1X = R8(addr + 32);
    pmtw->w.photonTrail1Y = R8(addr + 33);
    pmtw->w.photonTrail2X = R8(addr + 34);
    pmtw->w.photonTrail2Y = R8(addr + 35);
    pmtw->w.photonType    = R8(addr + 36);
    pmtw->w.unk1          = R8(addr + 37);
    pmtw->w.unk2          = R8(addr + 38);
    pmtw->w.unk3          = R8(addr + 39);
    pmtw->w.unk4          = R8(addr + 40);
    pmtw->w.unk5          = R8(addr + 41);
    pmtw->w.techBoost     = R8(addr + 42);
    pmtw->w.attack        = R8(addr + 43);
}


void
ReadGuardPMTData(
    ctx_t       *pctx,
    item_t      *item,
    pmtGuard_t  *pmtg)
{
    uint32_t     addr = GetPMTAddress(pctx, item->data[0], item->data[1], item->data[2]);

    if (!addr)
        return;

    pmtg->id              = R32(addr + 0);
    pmtg->model           = R16(addr + 4);
    pmtg->texture         = R16(addr + 6);
    pmtg->teamPoints      = R16(addr + 8);
    pmtg->g.dfp           = R16(addr + 12);
    pmtg->g.evp           = R16(addr + 14);
    pmtg->g.blockParticle = R8(addr + 16);
    pmtg->g.blockEffect   = R8(addr + 17);
    pmtg->g._class        = R16(addr + 18);
    pmtg->g.level         = R8(addr + 20);
    pmtg->g.efr           = R8(addr + 21);
    pmtg->g.eth           = R8(addr + 22);
    pmtg->g.eic           = R8(addr + 23);
    pmtg->g.edk           = R8(addr + 24);
    pmtg->g.elt           = R8(addr + 25);
    pmtg->g.dfpMax        = R8(addr + 26);
    pmtg->g.evpMax        = R8(addr + 27);
    pmtg->g.statBoost     = R8(addr + 28);
    pmtg->g.techBoost     = R8(addr + 29);
    pmtg->g.unk1          = R8(addr + 30);
    pmtg->g.unk2          = R8(addr + 31);
}