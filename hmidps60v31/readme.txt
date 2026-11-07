jvm s60v3.1 improvements patch

exe extracted from nokia e71, was already epoc format

unpacked
added -UseVerifier to disable preverification
added -comp to force AoT
changed z: to c: in bcp arg, to enable adding extra classes to c:\private\102033e6\extensions.jar
increased bytecode size limit for jit compilation from 1500 to 65535
replaced "midiclient{000a0000}[101f7d00].dll" import with "midiclien2{000a0000}[201f7d00].dll" wrapper for volume control support
removed connection filters for 80 and 443 ports access
replaced load of midp2runtimev2.dll with midp2runtimev3.dll wrapper for custom dynamic system properties

patch in hex:
56128-5612F: 06 A8 81 F0 18 EA 05 90 -> 00 00 00 00 00 00 00 20
8FA88-8FA89: DC 05 -> FF FF
B19D8: 00 -> 01
B1AE5: 7A -> 63
B1B44-B1B59: 3D 53 74 61 63 6B 53 69 7A 65 49 6E 63 72 65 6D 65 6E 74 35 31 32 -> 2D 4C 6F 67 56 4D 4F 75 74 70 75 74 00 00 00 00 00 00 00 00 00 00
B1B7C-B1B83: 4A 61 72 43 61 63 68 65 -> 56 65 72 69 66 69 65 72
B1B89-B1B9B: 43 61 63 68 65 4A 61 72 46 69 6C 65 48 61 6E 64 6C 65 73 -> 63 6F 6D 70 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
B1D30: 5A -> 43
236F96: 32 -> 33
2618BD: 74 -> 32
2618C9: 21 -> 32

see midiclien2dll for midi volume control wrapper sources


sometines needs c2z4bin
requires copying resources of 102033e6 from z to c
take midp2runtimev3.dll from midp2runtimev3dll

works on e90, e71, n95, n82, е51
doesn't work on 6120c and maybe on some specific firmware versions too

asd1.rmp and asd2.rmp are first attempts to patch extensions.jar path

classpath extensions can not replace existing classes

by shinovon, worked on this on 03.08.2024, 29.04.2025, 03.02.2026, 27.07.2026, 30.10.2026, 04.11.2026
