1080p H264 decoding support patch for Belle FP1, FP2

port of h264 decoding driver from 808 because it has increased resolution limits

"old" is rmp version of this patch, proof of concept that didn't work well


installation:
requires c2z4bin patch
copy sys to c:\
add to C:\Data\R1_Mobile_4_0.cfg:
MaxVideoWidth=1920
MaxVideoHeight=1088


tested on 701 fp2, 700 fp1

by shinovon, worked on this on 10.07.2026-11.07.2026
