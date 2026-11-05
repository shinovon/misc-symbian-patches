jvm s60v2.8 improvements patch

exe extracted from nokia n72, was already epoc format

unpacked
added -comp to force AoT
removed connection filters for 80 and 443 ports access

patch in hex:
71ADE-71AE3: 68 46 45 F0 E8 FF -> 00 00 00 00 00 20
80622-80622: 11 D1 -> 00 00
8062A: 12 -> 0D
1910DC-1910E7: 3D 48 65 61 70 4D 69 6E 34 30 30 4B -> 2D 63 6F 6D 70 00 00 00 00 00 00 00


SCRAPPED:
replaced load of midp2runtimev2.dll with midp2runtimev3.dll for custom dynamic system properties
7050E: 13 -> 11
70514-7053C: 11 49 30 1C 47 F0 5A F9 00 21 00 28 00 D1 01 21 00 29 11 D0 13 A9 68 46 47 F0 E4 F8 00 28 0B D1 28 68 01 69 28 1C 8F F7 D1 -> 13 A9 68 46 47 F0 EE F8 00 28 15 D1 28 68 01 6C 06 E0 00 00 00 00 00 00 00 00 00 00 00 00 00 00 0A 1C 31 1C 28 1C 8F F7 D3
190AA6: 32 -> 33


requires copying midp resources from z:\system\data to c

by shinovon, worked on this on 17.10.2026, 31.10.2026, 04.11.2026, 05.11.2026
