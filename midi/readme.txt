mobilebae patches

customdls.rmp:
changes z:\resource\ensb.dat to c:\resource\ensb.dls
nops xor decryption subroutine

stereomidi.rmp:
changes channels count from 1 to 2 in output device preferences

mobilebaeheap.rmp:
increases max heap of mobilebae.dll, does not give anything, useless.

mobilebae_u1.zip:
extracted from se satio, has Mobile_0300_base bank built directly into dll and enabled stereo


rmp patches work only if mobilebae.dll is in xip,
check its size in z:\sys\bin, if its size is around ~290 kb, then patch is compatible,
otherwise it would have smaller size, meaning it's epoc and is compressed.


on some devices, samplerate of output device can vary from 24000 to 48000,
it can be changed like done in stereomidi.rmp


when patching mobilebae.dll with c2z, must include BAEMmf*.dll in c


original xor key for ensb.dat is 10 1F 8B 19 63 80 F4 12 27 E5 ED 61 34 A6 D2 05

ensb.py is tool for encrypting and decrypting ensb.dat, written by gemini

by shinovon, worked on this on 08.09.2024, 07.09.2025, 18.09.2025
