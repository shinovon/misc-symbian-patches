java lcdui canvas performance fix for nokia e6

increases max fps from 18-20 to 30-36

should've actually worked for all anna/belle refresh devices,
but it didn't for some reason. it applies, but has almost no effect on fps in native resolution on devices other than e6.


anna for jrt 2.2.54
belle_refresh for jrt 2.3.24


side effects:
no more interpolated scaling using jad attributes, it will be nearest neighbor and slow.


lcdui.dll:
unpacked
removed CMIDCanvas::InitPixelSourceL()
changed DrawWindowNgaL call condition to avoid crash caused by missing iAlfCompositionPixelSource

patch in hex (universal for anna and belle refresh):
01 26 00 28 1D D1 A6 62 E8 6D -> 01 26 00 28 1D D1 A6 62 1B E0
20 00 88 47 00 28 02 D1 20 00 FE F7 ?? FC -> 20 00 88 47 00 28 02 D1 20 00 05 E0 00 00


references:
https://github.com/SymbianSource/oss.FCL.sf.app.JRT/blob/eb71dedc1f00a188a9d79953b24d0fd3b0079fc1/javauis/lcdui_akn/lcdui/src/CMIDCanvas.cpp




by shinovon, worked on this on 15.07.2026
