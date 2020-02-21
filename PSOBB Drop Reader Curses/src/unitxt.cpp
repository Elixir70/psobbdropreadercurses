#include "items.h"
#include "pmt.h"
#include "unitxt.h"

// Read unitxt ID from the PMT
uint32_t
ReadUnitxtID(
    ctx_t  *pctx,
    item_t *item)
{
    uint32_t id = -1;
    uint32_t pmtAddress = GetPMTAddress(pctx,
                                        item->data[0],
                                        item->data[1],
                                        item->data[2]);
    if (pmtAddress)
        id = R32(pmtAddress);

    return id;
}

// Read the unitxt entry (wide char string of the name)
void *
ReadUnitxtEntry(
    ctx_t    *pctx,
    uint32_t  group,
    uint32_t  index,
    LPVOID    buffer,
    size_t    bufsize)
{
    LPVOID   ret = NULL;
    uint32_t address = R32(UNITXT_POINTER);
    if (!address)
        goto done;

    address = address + (group * 4);
    address = R32(address);
    if (!address)
        goto done;

    address = address + 4 * index;
    address = R32(address);
    if (!address)
        goto done;

    RWSTR(address, buffer, bufsize);
    ret = buffer;

done:
    return ret;
}

// Read the iten name using the specified ID. Intended for specials.
void
ReadItemNameByID(
    ctx_t    *pctx,
    item_t   *item,
    uint32_t  id,
    LPVOID    buffer,
    size_t    bufsize)
{
    ReadUnitxtEntry(pctx,
                    1,
                    id,
                    buffer, 
                    bufsize);
}

// Read item name by unitxt ID.
void
ReadItemName(
    ctx_t  *pctx,
    item_t *item,
    LPVOID  buffer,
    size_t  bufsize)
{
    ReadUnitxtEntry(pctx,
                    1,
                    item->unitxtID,
                    buffer,
                    bufsize);
}

// Read name of a special.
void
ReadSpecialName(
    ctx_t   *pctx,
    item_t  *item)
{
    ReadItemNameByID(pctx,
                     item,
                     R32(SPECIAL_BASE_ID) + item->d.weapon.specialID,
                     item->d.weapon.special, 
                     sizeof(item->d.weapon.special));
}

// Get the name of a technique.
void
ReadTechniqueName(
    ctx_t   *pctx,
    item_t  *item)
{
    ReadUnitxtEntry(pctx,
                    5,
                    item->data[4],
                    (LPVOID)item->name,
                    sizeof(item->name));
}

void
ReadPhotonBlastName(
    ctx_t   *pctx,
    uint8_t  pb,
    LPVOID   buffer,
    size_t   bufsize)
{
    ReadUnitxtEntry(pctx, 
                    9, 
                    pb, 
                    buffer,
                    bufsize);
}

void
ReadMagColorName(
    ctx_t   *pctx,
    uint8_t  color,
    LPVOID   buffer,
    size_t   bufsize)
{
    const TCHAR *colorName = _T("???");
    static const TCHAR *magColors[] = {
        _T("Red"),
        _T("Blue"),
        _T("Yellow"),
        _T("Green"),
        _T("Purple"),
        _T("Black"),
        _T("White"),
        _T("Cyan"),
        _T("Brown"),
        _T("Orange"),
        _T("Slate Blue"),
        _T("Olive"),
        _T("Turquoise"),
        _T("Fuschia"),
        _T("Grey"),
        _T("Cream"),
        _T("Pink"),
        _T("Dark Green"),
    };

    if (0 <= color &&
        color < (sizeof(magColors) / sizeof(TCHAR *)))
    {
        _tcscpy_s((TCHAR *)buffer, bufsize, magColors[color]);
    }
}

void
ReadESName(
    ctx_t   *pctx,
    item_t  *item,
    char    *buffer)
{
    int     len = 0;

    buffer[len] = '\0';

    for (int i = 0; i < 8; i += 2)
    {
        // name starts at 6th byte.
        int n = (item->data[6 + i] << 8) + item->data[7 + i];
        //n -= 0x8000;
        n ^= 0x8000;

        int temp = n / 0x400 + 0x40;
        if (temp > 0x40 && temp < 0x60)
        {
            buffer[len++] = temp;
            buffer[len] = '\0';
        }

        n = n % 0x400;

        temp = n / 0x20 + 0x40;
        if (temp > 0x40 && temp < 0x60)
        {
            buffer[len++] = temp;
            buffer[len] = '\0';
        }

        n = n % 0x20;
        temp = n + 0x40;
        if (temp > 0x40 && temp < 0x60)
        {
            buffer[len++] = temp;
            buffer[len] = '\0';
        }
    }
}

void
ReadESSpecialName(
    ctx_t      *pctx,
    item_t     *item,
    TCHAR      *buffer)
{
    static const TCHAR *esSpecials[] = {
        _T(""),
        _T("Jellen"),
        _T("Zalure"),
        _T("HP Regeneration"),
        _T("TP Regeneration"),
        _T("Burning"),
        _T("Tempest"),
        _T("Blizzard"),
        _T("Arrest"),
        _T("Chaos"),
        _T("Hell"),
        _T("Spirit"),
        _T("Berserk"),
        _T("Demon's"),
        _T("Gush"),
        _T("Geist"),
        _T("King's")
    };

    int idx = item->d.weapon.specialID;
    if (idx >= 0 && idx < (sizeof(esSpecials) / sizeof(TCHAR *)))
        _tcscpy(buffer, esSpecials[idx]);
    else
        _tcscpy(buffer, _T("Unknown"));
}