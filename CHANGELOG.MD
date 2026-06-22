# Changelog

All notable changes to TICKLER will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [1.0.0] - 2026-06-08

### Added

- Initial release
- Creates `TICKLER.RUN` in the BBS root directory at startup
- Removes `TICKLER.RUN` on clean BBS shutdown
- Refreshes `TICKLER.RUN` timestamp during nightly midnight cleanup
- Plain-text key=value file format (STATUS, VERSION, TIMESTAMP)
- Windows version resource metadata in DLL (product name, version, copyright)
- Absolute path resolution at startup to ensure reliable file removal at shutdown
- Audit log entries via `shocst()` on load and shutdown
