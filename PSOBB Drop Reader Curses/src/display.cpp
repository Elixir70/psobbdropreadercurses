#include <curses.h>

#include "display.h"
#include "items.h"

#define NUM_ITEMS 512

void 
DisplayFloorItems(
    ctx_t   *pctx,
    item_t  *items,
    int32_t  count);

void
DelayForChanges(
    ctx_t    *pctx,
    uint32_t  count);

void 
DoDisplay(
    ctx_t *pctx)
{
    item_t items[NUM_ITEMS];

    while (!pctx->readMemError)
    {
        int32_t itemsRead = 0;
        ReadFloorItems(pctx, items, NUM_ITEMS, &itemsRead);
        DisplayFloorItems(pctx, items, itemsRead);
        DelayForChanges(pctx, itemsRead);
    }

    if (pctx->readMemError)
    {
        clear();
        //printw("An error was encountered reading the process memory.\n");
        //printw("This program will now stop running.\n");
        refresh();
    }
}

#define LINE_CHARS 100
void
DisplayFloorItem(
    ctx_t   *pctx,
    item_t  *item)
{
    char       mbcs[LINE_CHARS+1];
    size_t     charsConverted = 0;
    weapon_t  *weapon;
    guard_t   *guard;
    barrier_t *barrier;
    unit_t    *unit;
    tool_t    *tool;
    mag_t     *mag;
    tech_t    *tech;
    uint32_t  *meseta;

    if (item->type == MESETA)
        goto done;

    if (WEAPON == item->type && item->d.weapon.isES)
        printw("S-RANK ");

    printw("%ls ", item->name);
    switch (item->type)
    {
        case WEAPON:
            weapon = &item->d.weapon;
            if (weapon->isES)
                printw("%s ", weapon->esName);

            if (weapon->grind > 0)
                printw("+%i ", weapon->grind);

            printw("[%ls] ", weapon->special);

            if (!weapon->isES)
            { 
                printw("[");
                for (int i = 1; i <= 5; ++i)
                {
                    uint16_t attr = weapon->attrs[i];
                    int      colorOn = 0;
                    if (attr >= 70)
                        colorOn = 1;
                    else if (attr > 0)
                        colorOn = 2;
                    if (colorOn)
                        attron(COLOR_PAIR(colorOn));
                    printw("%i", attr);
                    if (colorOn)
                        attroff(COLOR_PAIR(colorOn));
                    if (i < 4)
                        printw("/");
                    else if (i == 4)
                        printw("|");
                    
                }
                printw("] ");
            }

            break;

        case GUARD:
        {
            int colorOn = 0;
            guard = &item->d.guard;
            printw("[");

            if (guard->dfp == guard->dfpMax)
                colorOn = 1;
            if (colorOn)
                attron(COLOR_PAIR(colorOn));
            printw("%i", guard->dfp);
            if (colorOn)
                attroff(COLOR_PAIR(colorOn));
            printw("/%i | ", guard->dfpMax);

            colorOn = 0;
            if (guard->evp == guard->evpMax)
                colorOn = 1;
            if (colorOn)
                attron(COLOR_PAIR(colorOn));
            printw("%i", guard->evp);
            if (colorOn)
                attroff(COLOR_PAIR(colorOn));
            printw("/%i] ", guard->evpMax);
            break;
        }
        case BARRIER:
        {
            int colorOn = 0;
            barrier = &item->d.barrier;
            printw("[");

            if (barrier->dfp == barrier->dfpMax)
                colorOn = 1;
            if (colorOn)
                attron(COLOR_PAIR(colorOn));
            printw("%i", barrier->dfp);
            if (colorOn)
                attroff(COLOR_PAIR(colorOn));
            printw("/%i | ", barrier->dfpMax);

            colorOn = 0;
            if (barrier->evp == barrier->evpMax)
                colorOn = 1;
            if (colorOn)
                attron(COLOR_PAIR(colorOn));
            printw("%i", barrier->evp);
            if (colorOn)
                attroff(COLOR_PAIR(colorOn));
            printw("/%i] ", barrier->evpMax);
            break;
        }
        case UNIT:
            unit = &item->d.unit;
            switch (unit->mod)
            {
                case -2:
                    printw("-");
                case -1:
                    printw("- ");
                    break;

                case 2:
                    printw("+");
                case 1:
                    printw("+ ");
                    break;

                default:
                    break;
            }

            if (unit->kills)
                printw("[%iK] ", unit->kills);
            break;

        case TOOL:
            tool = &item->d.tool;
            if (tool->stack)
                printw("x%i ", tool->stack);
            break;

        case MAG:
            mag = &item->d.mag;
            printw("[%ls] [%.02f/%.02f/%.02f/%.02f] [%ls|%ls|%ls] ",
                   mag->colorName, 
                   mag->def, mag->pow, mag->dex, mag->mind,
                   mag->pbLName, mag->pbMName, mag->pbRName);
            break;

        case TECH:
            tech = &item->d.tech;
            if (tech->level)
                printw("Lv%i ", tech->level);
            break;

        case MESETA:
            meseta = &item->d.meseta;
            if (*meseta)
                printw("x%i ", *meseta);
            break;
    }

    printw("\n");

done:
    return;
}

void 
DisplayFloorItems(
    ctx_t   *pctx,
    item_t  *items, 
    int32_t  count)
{
    if (pctx->readMemError)
        return;
    
    if (pctx->noItemsCount >= 5)
    {
        // Force a clear() if count is still 0.
        pctx->lastDisplayCount = -1;
        pctx->noItemsCount = 0;
    }

    // Clear screen only if we have items to display or if
    // swapping between items to no items.
    if (count ||
        (0 == count && 0 != pctx->lastDisplayCount))
    {
        clear();
    }

    for (int32_t i = 0; i < count; ++i)
    {
        item_t  *item = &items[i];

        DisplayFloorItem(pctx, item);
    }

    if (0 == count)
    {
        if (0 != pctx->lastDisplayCount)
            printw("No items on floor.");

        ++pctx->noItemsCount;
    }

    pctx->lastDisplayCount = count;

    refresh();
}

void
DelayForChanges(
    ctx_t    *pctx,
    uint32_t  count)
{
    Sleep(500); 
    if (0 == count)
        printw(".");
    refresh();
}

