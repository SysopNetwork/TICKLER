# TICKLER

A watchdog marker module for The Major BBS V10.

TICKLER creates a small status file (`TICKLER.RUN`) in the BBS root directory when the BBS loads and removes it when the BBS shuts down cleanly. External monitoring tools can watch for this file to detect crashes: if the file is present but the BBS process is not running, an unclean shutdown has occurred and automated action can be taken.

---

## Features

- Creates `TICKLER.RUN` in the BBS root directory at startup
- Removes `TICKLER.RUN` on clean BBS shutdown
- Refreshes the file timestamp during nightly cleanup so monitoring tools can detect a stale marker
- Runs silently in the background with no user-visible interface
- Status file uses a plain key=value text format readable by scripts and tools on any platform

---

## How It Works

| BBS State | TICKLER.RUN |
|-----------|-------------|
| BBS starting up | File is created |
| BBS running normally | File is present |
| Nightly cleanup runs | File is refreshed with current timestamp |
| BBS shuts down cleanly | File is removed |
| BBS crashes / power loss | File remains (no clean shutdown occurred) |

A monitoring tool that checks whether the BBS process is running can use the presence of `TICKLER.RUN` to distinguish a clean stop from a crash:

- **Process running + file present** → BBS is up, normal
- **Process not running + file absent** → BBS stopped cleanly
- **Process not running + file present** → BBS crashed, action required

---

## TICKLER.RUN File Format

The file uses plain key=value pairs with CRLF line endings:

```
STATUS=RUNNING
VERSION=1.0.0
TIMESTAMP=2026-06-08 14:32:05
```

| Field | Description |
|-------|-------------|
| STATUS | Always `RUNNING` while the BBS is up |
| VERSION | TICKLER module version that created the file |
| TIMESTAMP | Date and time the file was last written (local server time) |

---

## Installation

1. Copy `TICKLER.DLL` and `TICKLER.MDF` from the `DIST\1.0.0\` folder to the BBS root directory.
2. Add `TICKLER` to your module list (e.g., `GALGSBL.CFG`).
3. Restart the BBS.

After startup, `TICKLER.RUN` will appear in the BBS root directory.

---

## Monitoring Integration

Any tool that can read a file and check a process list can use TICKLER. Example approaches:

- **Windows Task Scheduler** — Run a script every few minutes that checks for the BBS process and `TICKLER.RUN`
- **PowerShell watchdog** — Poll process state and file presence, restart service on mismatch
- **Third-party monitoring software** — Configure a file-existence check paired with a process check

---

## Version Information

| Version | Date | Notes |
|---------|------|-------|
| 1.0.0 | 2026-06-08 | Initial release |

---

## Tips / Donations

- **Cash App:** [$laudenbachm](https://cash.app/$laudenbachm)
- **Ko-fi:** [ko-fi.com/laudenbachm](https://ko-fi.com/laudenbachm)

---

## License

TICKLER is released under the [MIT License](LICENSE).

---

## Support

TICKLER is developed and maintained by Mark Laudenbach at Sysop Network.
