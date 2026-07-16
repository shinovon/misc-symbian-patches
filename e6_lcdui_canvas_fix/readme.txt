java lcdui canvas performance fix for nokia e6

increases max fps from 18-20 to 30-36

should've actually worked for all anna/belle refresh devices,
but it didn't for some reason. it applies, but has almost no effect on fps in native resolution on devices other than e6.


anna for jrt 2.2.54, orig dll from e6 025.007
belle_refresh for jrt 2.3.24, orig dll from e7 111.040.1511


side effects:
hardware scaling will reset to slow software scaling after minimizing app


lcdui.dll:
unpacked
added extra condition to create pixel source only if scaling is enabled in CMIDCanvas::InitPixelSourceL()
changed DrawWindowNgaL call condition to avoid crash if iAlfCompositionPixelSource is not set

patch in hex:

anna:
01 26 00 28 1D D1 A6 62 E8 6D FD F7 4A E8 0B 28 18 D0 0C 28 18 D0 0D 28 13 D1 03 20 E0 63 -> 00 28 1E D1 98 20 28 58 00 28 1A D0 01 26 A6 62 E8 6D FD F7 46 E8 0A 38 E0 63 C0 46 C0 46
20 00 88 47 00 28 02 D1 20 00 FE F7 5C FC -> 20 00 88 47 00 28 02 D1 20 00 05 E0 00 00

belle refresh:
01 26 00 28 1D D1 A6 62 E8 6D FE F7 16 ED 0B 28 18 D0 0C 28 18 D0 0D 28 13 D1 03 20 E0 63 -> 00 28 1E D1 98 20 28 58 00 28 1A D0 01 26 A6 62 E8 6D FE F7 12 ED 0A 38 E0 63 C0 46 C0 46
20 00 88 47 00 28 02 D1 20 00 FE F7 7F FC -> 20 00 88 47 00 28 02 D1 20 00 05 E0 00 00


references:
https://github.com/SymbianSource/oss.FCL.sf.app.JRT/blob/eb71dedc1f00a188a9d79953b24d0fd3b0079fc1/javauis/lcdui_akn/lcdui/src/CMIDCanvas.cpp




by shinovon, worked on this on 15.07.2026, 16.07.2026
