# ADR-001: Crash Detection Via Marker File

**Date:** 2026-06-08  
**Status:** Accepted

---

## Problem

External monitoring tools need a reliable way to determine whether The Major BBS has crashed (as opposed to shutting down cleanly). The BBS does not expose a native API or status endpoint for external consumers.

---

## Options Considered

### Option A: Process heartbeat via named pipe or socket
A background thread in the module writes periodic heartbeats to a named pipe. A monitoring tool reads the pipe and declares the BBS dead if heartbeats stop.

- Pro: Near real-time detection
- Con: Requires inter-process communication plumbing; adds complexity; monitoring tool must maintain a persistent connection

### Option B: Registry key
The module writes a value to the Windows registry at startup and removes it at shutdown.

- Pro: No filesystem dependency
- Con: Registry access from monitoring scripts requires elevated permissions or PowerShell; less portable and harder to observe manually

### Option C: Marker file in BBS root (selected)
The module creates a plain-text file in the BBS root at startup and deletes it at clean shutdown.

- Pro: Simplest possible implementation; any tool or script can check for a file; human-readable content; no special permissions beyond BBS working directory access; works with file-monitoring utilities, cron jobs, and custom scripts alike
- Con: Requires filesystem access from the monitoring tool; not instantaneous (file system polling interval applies)

---

## Decision

Use a marker file (`TICKLER.RUN`) in the BBS root directory.

The simplicity of the file-based approach outweighs its limitations. The monitoring tool's polling interval (typically seconds) is acceptable for a BBS restart watchdog. The plain-text content provides useful diagnostics without requiring binary parsing.

---

## Reasoning

- The MBBS module SDK provides reliable `finrou` (shutdown) and `init__` (startup) callbacks, making the create/delete lifecycle straightforward.
- No additional infrastructure is required on the monitoring side.
- The approach is self-documenting: an administrator can check for the file manually to assess BBS state.
- Crash detection via persistent marker files is a well-established pattern in server software.
