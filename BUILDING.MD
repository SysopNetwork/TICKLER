# Building TICKLER

## Requirements

| Tool | Version | Notes |
|------|---------|-------|
| Microsoft Visual Studio | 2022 (v143 toolset) | Community edition or higher |
| Windows SDK | 10.0 | Included with Visual Studio |
| MBBS V10 Module SDK | Current | See SDK setup below |
| Target platform | Win32 (x86) | Do NOT build for x64 |

---

## SDK Setup

TICKLER requires the MBBS V10 Module SDK for its include files and import libraries. The SDK path is configured through a `Local.props` file that each developer creates locally.

1. Copy `SRC\Local.props.example` to `SRC\Local.props`
2. Open `Local.props` and set `MBBS10SDK` to the full path of your SDK installation. The path must end with a backslash.

Example:

```xml
<MBBS10SDK>D:\MBBS\SDK\</MBBS10SDK>
```

`Local.props` is excluded from version control. Never commit it.

---

## Build Instructions

1. Open `SRC\TICKLER.vcxproj` in Visual Studio 2022
2. Confirm the platform is set to **Win32** (not x64)
3. Select **Debug** or **Release** configuration
4. Build → Build Solution (or press F7)

Build output goes to:

- Debug: `BUILD\Debug\TICKLER.DLL`
- Release: `BUILD\Release\TICKLER.DLL`

---

## Packaging a Release

1. Build the **Release|Win32** configuration
2. Copy `BUILD\Release\TICKLER.DLL` to `DIST\1.0.0\`
3. Verify `DIST\1.0.0\` contains only:
   - `TICKLER.DLL`
   - `TICKLER.MDF`
4. Verify DLL version metadata (right-click → Properties → Details in Explorer)
5. Confirm no debug information was included in the Release build output

---

## Installation on the BBS

1. Copy `TICKLER.DLL` and `TICKLER.MDF` to the BBS root directory
2. Add `TICKLER` to the module list in the BBS server configuration
3. Restart the BBS
4. Confirm `TICKLER.RUN` appears in the BBS root directory after startup

---

## Notes

- The `wgserver_lib.lib` and `galgsbl_lib.lib` import libraries are required at link time and are provided in the MBBS V10 SDK.
- The `/J` compiler flag (treat `char` as `unsigned`) is required for compatibility with the MBBS API, which assumes unsigned character types throughout.
- Do not enable the x64 configurations in the project file — MBBS V10 is a 32-bit server process.
