/*****************************************************************************
 *   TICKLER.C   V1.0.0                     The Major BBS V10 Tickler Module *
 *                                                                           *
 *   Copyright (C) 2026 Sysop Network.  All Rights Reserved.                *
 *                                                                           *
 *   Description:    Creates a marker file (TICKLER.RUN) in the BBS root    *
 *                   directory when the BBS starts up and removes it when   *
 *                   the BBS shuts down cleanly.                             *
 *                                                                           *
 *                   External monitoring tools can watch for this file to   *
 *                   detect crashes: if TICKLER.RUN is present but the BBS  *
 *                   process is not running, an unclean shutdown occurred   *
 *                   and remedial action (such as a soft restart) can be    *
 *                   taken automatically.                                    *
 *                                                                           *
 *****************************************************************************/

#include <windows.h>
#include <stdio.h>
#include "gcomm.h"
#include "majorbbs.h"

/* Name of the marker file created in the BBS root directory */
#define TICKLER_FILENAME    "TICKLER.RUN"

/* Module version string */
#define TICKLER_VERSION     "1.0.0"

/* Forward declarations */
static VOID build_tickler_path(VOID);
static VOID create_tickler_file(VOID);
static VOID remove_tickler_file(VOID);
VOID midnight_cleanup(VOID);
VOID system_shutdown(VOID);
VOID end_tickler(VOID);

/*
 * Module registration block.
 *
 * TICKLER has no user-facing interface - it runs invisibly in the
 * background. The module manager still requires registration so that
 * the shutdown (finrou) and midnight cleanup (mcurou) callbacks fire.
 */
struct module TICKLER = {
    "",                 /* No menu name  - invisible to BBS users            */
    NULL,               /* No logon routine                                  */
    NULL,               /* No main input routine                             */
    NULL,               /* No status-input routine                           */
    NULL,               /* No injoth routine                                 */
    NULL,               /* No logoff routine                                 */
    NULL,               /* No hangup routine                                 */
    midnight_cleanup,   /* Midnight cleanup: refresh the marker timestamp    */
    NULL,               /* No delete-account routine                         */
    system_shutdown     /* System shutdown: remove the marker file           */
};

/*
 * Full absolute path to TICKLER.RUN, resolved once at startup.
 * Storing an absolute path ensures the file can be removed at shutdown
 * even if the working directory has changed since init.
 */
static CHAR tickler_path[MAX_PATH];

/*
 * build_tickler_path
 *
 * The BBS server runs from its root directory, so at init time the
 * current working directory IS the BBS root.  Capture it as an
 * absolute path now so the file can be reliably removed at shutdown
 * even if the working directory changes later.
 */
static VOID
build_tickler_path(VOID)
{
    CHAR homedir[MAX_PATH];

    if (GetCurrentDirectoryA(sizeof(homedir), homedir) != 0) {
        _snprintf(tickler_path, sizeof(tickler_path) - 1,
                  "%s\\%s", homedir, TICKLER_FILENAME);
        tickler_path[sizeof(tickler_path) - 1] = '\0';
    } else {
        /* Fallback: use bare filename relative to current directory */
        stzcpy(tickler_path, TICKLER_FILENAME, sizeof(tickler_path));
    }
}

/*
 * create_tickler_file
 *
 * Creates or overwrites the marker file and writes a plain-text status
 * block so that monitoring tools can read human-readable information
 * without parsing a binary format.
 *
 * File format (key=value pairs, CRLF line endings):
 *   STATUS=RUNNING
 *   VERSION=1.0.0
 *   TIMESTAMP=YYYY-MM-DD HH:MM:SS
 */
static VOID
create_tickler_file(VOID)
{
    SYSTEMTIME st;
    FILE *fp;

    fp = fopen(tickler_path, "w");
    if (fp == NULL) {
        return;
    }

    GetLocalTime(&st);

    fprintf(fp,
            "STATUS=RUNNING\r\n"
            "VERSION=%s\r\n"
            "TIMESTAMP=%04d-%02d-%02d %02d:%02d:%02d\r\n",
            TICKLER_VERSION,
            st.wYear, st.wMonth,  st.wDay,
            st.wHour, st.wMinute, st.wSecond);

    fclose(fp);
}

/*
 * remove_tickler_file
 *
 * Deletes the marker file.  Its absence tells monitoring tools that
 * the BBS went down cleanly and no restart action is needed.
 */
static VOID
remove_tickler_file(VOID)
{
    DeleteFileA(tickler_path);
}

/*
 * init__tickler
 *
 * Called once by the BBS when the module is loaded at startup.
 * The function name MUST match the DLL name (init__<dllname>).
 */
void EXPORT
init__tickler(VOID)
{
    /* Register the module so the BBS knows about our callbacks */
    stzcpy(TICKLER.descrp, gmdnam("TICKLER.MDF"), MNMSIZ);
    register_module(&TICKLER);

    /* Resolve and cache the absolute path to the marker file */
    build_tickler_path();

    /* Create the marker file - BBS is now up and running */
    create_tickler_file();

    shocst("TICKLER", spr("v%s loaded - marker file: %s",
                          TICKLER_VERSION, tickler_path));
}

/*
 * midnight_cleanup
 *
 * Called nightly by the BBS cleanup scheduler.
 * Refreshes the marker file so its timestamp reflects that the BBS
 * is still alive.  Monitoring tools can use a stale timestamp as an
 * additional signal that something may be wrong.
 */
VOID EXPORT
midnight_cleanup(VOID)
{
    create_tickler_file();
}

/*
 * system_shutdown
 *
 * Called by the BBS during a clean, orderly shutdown.
 * Removing the marker file here signals to any monitoring tool that
 * the BBS stopped intentionally - no restart action should be taken.
 */
VOID EXPORT
system_shutdown(VOID)
{
    remove_tickler_file();
    shocst("TICKLER", "Marker file removed - clean shutdown");
}

/*
 * end_tickler
 *
 * Intentionally empty.  Marks the end of the module's code segment,
 * which helps crash analysis tools (GALEXCEP.OUT) bound the address
 * range belonging to this module.
 */
VOID EXPORT
end_tickler(VOID)
{
}
