#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include "context.h"
#include "display.h"

int 
ParseArgs(
    ctx_t *pctx, 
    int    argc, 
    char  *argv[]);

int 
main(
    int   argc, 
    char *argv[])
{
    int       rc; 
    ctx_t     ctx = { 0 };
    int       count = 0;

    if ((rc = ParseArgs(&ctx, argc, argv)))
    {
        fprintf(stderr, "Error %i parsing arguments. Exiting...", rc);
        goto done;
    }

    if ((rc = InitializeCurses(&ctx)))
    {
        fprintf(stderr, "Error %i initializing screen for curses. Exiting...", rc);
        goto done;
    }
        
    while (1)
    {
        if (count % 10 == 0)
        {
            clear();
            printwr("Looking for psobb.exe process.");
        }

        if ((rc = OpenPSOBBHandle(&ctx)))
        {
            if (ctx.cmdLinePid)
            {
                // Couldn't open specified PID
                printwr("Unexpected error %i opening handle to psobb.exe.\n", rc);
                break;
            }

            count++;
            Sleep(1000);
            printwr(".");
            continue;
        }

        ctx.readMemError = false;
        count = 0;
        DoDisplay(&ctx);
        if (ctx.readMemError)
            ClosePSOBBHandle(&ctx);
        if (ctx.cmdLinePid)
            // Ran once, nothing else to do.
            break;

        ctx.psobbPid = 0;
    }

done:
    CloseCurses(&ctx);

    return 0;
}

// Get the PID from list of arguments. Eventually should allow specifying options like 
// Soly's Item Reader.
int 
ParseArgs(
    ctx_t *pctx, 
    int    argc, 
    char  *argv[])
{
    int rc = 0;

    // Only optional pid, nothing else.
    if (argc > 2)
    {
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "%s [<PSOBB_Process_ID>]\n", argv[0]);
        rc = -1;
        goto done;
    }

    if (argc == 2)
    {
        //pctx->psobbPid = atoi(argv[1]);
        pctx->cmdLinePid = atoi(argv[1]);
        if (pctx->cmdLinePid < 0)
        {
            fprintf(stderr, "Invalid Process ID %i specified for PSOBB process.\n", pctx->cmdLinePid);
            rc = -1;
            goto done;
        }
    }

done:
    return rc;
}


