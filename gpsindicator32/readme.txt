gps indicator enabler for s60v3.2

eposserver.exe:
taken from n86, converted from xip with aromat
changed rproperty uid from 0x101F97B2 to 0x101F75B6 so that sysap.exe could listen to it
without rmp it will show irda indicator

patch in hex:
498: 05 -> 06
3470-3471: B2 97 -> B6 75
3474-3475: B2 97 -> B6 75
3478-3479: B2 97 -> B6 75
347C-347D: B2 97 -> B6 75

gpsindicator.rmp:
replaced irda indicator id with gps (0->53)


installation:
copy eposserver.exe to c:\sys\bin
copy eposserver.rsc from z:\private\101f97b2 to same path in c
add and enable gpsindicator.rmp to patches


tested on e52, n78, 6220c, thanks to ZAIOOD999

by shinovon, worked on this on 12.10.2026
