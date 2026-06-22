# TICKLER - Technical Documentation

## Overview

TICKLER is a background-only MBBS V10 module. It has no user-facing menu, no message file, and no Btrieve data file. Its sole function is to manage a plain-text marker file (`TICKLER.RUN`) in the BBS root directory as a runtime presence indicator.

---

## Architecture

### Module Registration

TICKLER registers with the BBS module manager using the standard `register_module()` call during `init__tickler()`. Although it has no user interface, registration is required to receive the `finrou` (shutdown) and `mcurou` (midnight cleanup) callbacks from the BBS kernel.

The module interface block sets all user-interaction callbacks to `NULL`:

```
lonrou  = NULL   (no logon supplement)
sttrou  = NULL   (no main input handler)
stsrou  = NULL   (no status-input handler)
injrou  = NULL   (no injoth handler)
lofrou  = NULL   (no logoff handler)
huprou  = NULL   (no hangup handler)
mcurou  = midnight_cleanup
dlarou  = NULL   (no delete-account handler)
finrou  = system_shutdown
```

---

## Startup Sequence

1. BBS loads `TICKLER.DLL` and calls `init__tickler()`
2. `register_module()` is called to register callbacks
3. `build_tickler_path()` calls `GetCurrentDirectoryA()` to capture the BBS root as an absolute path. The full path to `TICKLER.RUN` is stored in a static buffer for use at shutdown.
4. `create_tickler_file()` writes `TICKLER.RUN` to disk
5. A status message is logged via `shocst()`

---

## Shutdown Sequence

When the BBS shuts down cleanly, the kernel calls the `finrou` callback, which is wired to `system_shutdown()`:

1. `remove_tickler_file()` calls `DeleteFileA()` on the stored absolute path
2. A status message is logged via `shocst()`

If the BBS crashes or loses power, `system_shutdown()` never executes and `TICKLER.RUN` remains on disk — this is the intended crash-detection mechanism.

---

## Midnight Cleanup

The `mcurou` callback fires nightly when the BBS runs its maintenance cycle. `midnight_cleanup()` calls `create_tickler_file()`, which overwrites `TICKLER.RUN` with a fresh timestamp. This allows monitoring tools to use a stale `TIMESTAMP` field as an additional health signal.

---

## File Path Resolution

The BBS working directory can change during normal operation. The BBS server starts from its root directory, so at module init time the current working directory is the BBS root. To ensure reliable file removal at shutdown, TICKLER captures that root as an absolute path once at startup:

```c
GetCurrentDirectoryA(sizeof(buf), buf);       // capture absolute path (BBS root)
_snprintf(tickler_path, ..., "%s\\%s", ...);  // build full file path
```

The absolute path is stored in the module-private `tickler_path[]` buffer and used for all subsequent file operations.

---

## TICKLER.RUN File Format

Plain text, key=value pairs, CRLF line endings, no section headers.

```
STATUS=RUNNING
VERSION=1.0.0
TIMESTAMP=2026-06-08 14:32:05
```

| Key | Value | Notes |
|-----|-------|-------|
| STATUS | `RUNNING` | Always this value while BBS is up |
| VERSION | e.g. `1.0.0` | TICKLER module version |
| TIMESTAMP | `YYYY-MM-DD HH:MM:SS` | Local server time, updated at startup and nightly cleanup |

---

## Dependencies

| Dependency | Purpose |
|------------|---------|
| `gcomm.h` | Core Galacticomm types, `stzcpy()`, `spr()` |
| `majorbbs.h` | `register_module()`, `shocst()`, `gmdnam()`, `MNMSIZ`, module struct |
| `windows.h` | `GetCurrentDirectoryA()`, `DeleteFileA()`, `GetLocalTime()`, `MAX_PATH` |
| `stdio.h` | `fopen()`, `fprintf()`, `fclose()`, `_snprintf()` |

---

## DLL Exports

| Export | Ordinal | Description |
|--------|---------|-------------|
| `init__tickler` | 1 | Module initialization entry point |
| `midnight_cleanup` | — | Nightly cleanup callback |
| `system_shutdown` | — | Clean shutdown callback |
| `end_tickler` | — | End-of-module marker for crash analysis |

---

## Distribution Files

| File | Required | Purpose |
|------|----------|---------|
| `TICKLER.DLL` | Yes | Module binary |
| `TICKLER.MDF` | Yes | Module definition (name, DLL list) |

No message files (`.MCV`) or data files (`.DAT`) are required.

---

## Known Limitations

- `TICKLER.RUN` uses local server time; the timestamp is not UTC.
- If the BBS root directory path exceeds `MAX_PATH` (260 characters), the fallback path (bare filename) is used. This is extremely unlikely in practice.
- If the BBS process is killed via Task Manager without the kernel dispatching `finrou`, the file will remain on disk as expected (crash-detection is working as designed).
