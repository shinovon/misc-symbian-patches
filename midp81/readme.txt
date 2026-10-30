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

requires copying midp resources from z:\system\data to c

by shinovon, worked on this on 17.10.2026, 31.10.2026
