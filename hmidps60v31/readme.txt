jvm s60v3.1 performance patch

exe extracted from nokia e71, was already epoc format

unpacked
added -UseVerifier to disable preverification
added -MixedMode to force AoT and disable JIT
changed z: to c: in bcp arg, to enable adding extra classes to c:\private\102033e6\extensions.jar

patch in hex:
B19D8: 00 -> 01
B1AE5: 7A -> 63
B1B44-B1B59: 3D 53 74 61 63 6B 53 69 7A 65 49 6E 63 72 65 6D 65 6E 74 35 31 32 -> 2D 4C 6F 67 56 4D 4F 75 74 70 75 74 00 00 00 00 00 00 00 00 00 00
B1B7C-B1B83: 4A 61 72 43 61 63 68 65 -> 56 65 72 69 66 69 65 72
B1B89-B1B9B: 43 61 63 68 65 4A 61 72 46 69 6C 65 48 61 6E 64 6C 65 73 -> 4D 69 78 65 64 4D 6F 64 65 00 00 00 00 00 00 00 00 00 00


requires c2z4bin and copying resources of 102033e6 from z to c
not sure about compatibility


asd1.rmp and asd2.rmp are first attempts to patch extensions.jar path

classpath extensions can not replace existing classes

by shinovon, worked on this on 03.08.2024, 29.04.2025
