jvm s60v2.8 improvements patch

exe extracted from nokia n72, was already epoc format

unpacked
added -comp to force AoT
removed connection filters for 80 and 443 ports access
replaced load of midp2runtimev2.dll with midp2runtimev3.dll wrapper for custom dynamic system properties

patch in hex:
704EA-7050F: 09 49 30 1C 47 F0 6F F9 00 21 00 28 00 D1 01 21 00 29 0A D0 13 A9 68 46 47 F0 F9 F8 00 28 20 D1 28 68 41 69 13 E0 -> 0E E0 31 1C 28 1C 8F F7 F8 FD 01 1C 05 00 28 D0 20 E0 0A D0 28 00 47 F0 BA F9 00 00 00 00 20 E0 28 68 02 6C ED E7
7053C: D1 -> D3
70548-7054B: 47 F0 DC F8 -> D9 E7 00 00
71ADE-71AE3: 68 46 45 F0 E8 FF -> 00 00 00 00 00 20
80622-80622: 11 D1 -> 00 00
8062A: 12 -> 0D
1910DC-1910E7: 3D 48 65 61 70 4D 69 6E 34 30 30 4B -> 2D 63 6F 6D 70 00 00 00 00 00 00 00
190AA6: 32 -> 33


requires copying midp resources from z:\system\data to c

by shinovon, worked on this on 17.10.2026, 31.10.2026, 04.11.2026-07.11.2026
