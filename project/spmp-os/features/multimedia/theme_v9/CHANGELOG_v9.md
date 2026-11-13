# v9 against v8

- Preserved 25 v8 theme asset trees byte-for-byte.
- Added 9 themed clean backgrounds (PNG + RGB565 SPBG with CRC32), JSON styles and separately editable RGBA panels.
- Grew catalog and native freestanding catalog/config API to 34, version 9 names to avoid colliding with previous native catalog symbols.
- Preserved 8 symbol-only icons, localized UI text, audio UI cues, and two menu layouts.
- Added additive SHA256-checked v9 patch independent of v8; host hook optional, conflict-detecting.
- Added full-collection C-rendered 68-screen checks and contact preview.
- Hardware LCD/SD/FAT/RTC/Audio validation still pending.
