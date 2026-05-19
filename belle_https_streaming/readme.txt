HTTPS video streaming support patch for Belle Refresh, FP1

doesn't work on some fp2 devices such as 603, 808, because playbackhelper_stub.sis is missing there

dlls are from 808 113.010.1508

playbackhelper.dll:
unpacked
replaced httpt with https
decreased version of dll imports
increased version (petran -version 11.2000 -nocompress)

patch in hex:
2800: 74 -> 73
...

httpfsys.dll:
orig

hxnetwksvc.dll:
unpacked
hardcoded buffer size to fix IPv6 crash
noped CSecureSocket::SetOpt because it sets IP address as SNI
increased version (petran -version 11.2000 -nocompress)

patch in hex:
154B0-154B7: 50 00 94 E5 0F 02 C0 E3 -> 80 00 A0 E3 00 00 A0 E1 (fix crash due to IPv6)
15500-15503: 51 AC FF EB -> 00 00 A0 E1 (NOP SetOpt call)



installation:
playbackhelper.sis (ynstallserver if "component is built-in" error)
dlls to c:\sys\bin


references:
pdk 3.0.4 bin_rndonly_realnetworks.zip clntcore.dll
has debug symbols in winscw build, but slightly older version without securesocket

hxplay old sources: https://fossies.org/linux/misc/old/hxplay-11.0.0.tar.gz
has symbian code from 2004


to enable logs, add
"
Logging\File\Enabled=1
Logging\File\FileName=c:\data\hxlog.txt
"
in C:\Data\R1_Mobile_4_0.cfg



by shinovon, worked on this on 10.04.2026-20.04.2026
