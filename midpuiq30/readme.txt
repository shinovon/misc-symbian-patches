jvm uiq 3.0 improvements patch

exe extracted from se p1i, was already epoc format

unpacked
added -comp to force AoT
increased bytecode size limit for jit compilation from 6000 to 65535
removed connection filters for 80 and 443 ports access
replaced load of midp2runtimev2.dll with midp2runtimev3.dll wrapper for custom dynamic system properties

patch in hex:
4963C-49641: 06 A8 58 F0 68 EE -> 00 00 00 00 00 20
7FDF0-7FDF1: 70 17 -> FF FF
8D307: D1 -> E0
8D451-8D45B: 55 73 65 4A 61 72 43 61 63 68 65 -> 63 6F 6D 70 00 00 00 00 00 00 00
8D461-8D473: 43 61 63 68 65 4A 61 72 46 69 6C 65 48 61 6E 64 6C 65 73 -> 55 73 65 56 65 72 69 66 69 65 72 00 00 00 00 00 00 00 00
1EEDD2: 32 -> 33


take midp2runtimev3.dll from midp2runtimev3dll

by shinovon, worked on this on 31.10.2026
