backport of h264decoder_11 from belle refresh to s60v3.2

adds h264 main profile support to realplayer
is proof of concept, not a real solution, shows awful performance.

xip extracted from rom dump of nokia e7 111.040.1511
converted to epoc with aromat 0.19
replaced imports of 222,223=>135,155 (__aeabi_idiv,__aeabi_uidiv=>__aeabi_idivmod,__aeabi_uidivmod) from drtaeabi{000a0000}.dll
changed plugin uid in implementation table from 0x102073EF to 0x202073EF
changed dll UID3 from 0x102073EE to 0x202073EE (petran -sid 0x202073ee -uid3 0x202073ee -nocompress)
compiled plugin rsc with correct uid

install h264decoder2.sis
copy props starting with MDFCodecRule from z:\resource\R1_Mobile_4_0_Platform.cfg to c:\data\R1_Mobile_4_0.cfg
keep correct order, where X - number

instead of:
MDFCodecRuleX={ H264 Baseline 176x144 0x102073ef 0x10273417 0x102073ef 0x10273417 }
MDFCodecRuleX={ H264 Main     176x144 0x00000000 0x00000000 0x00000000 0x00000000 }
MDFCodecRuleX={ H264 Extended 176x144 0x00000000 0x00000000 0x00000000 0x00000000 }

write:
MDFCodecRuleX={ H264 Baseline 176x144 0x202073EF 0x10273417 0x202073EF 0x10273417 176x144 0x3 0x0 }
MDFCodecRuleX={ H264 Main     176x144 0x202073EF 0x10273417 0x202073EF 0x10273417 176x144 0x3 0x0 }
MDFCodecRuleX={ H264 High     176x144 0x202073EF 0x10273417 0x202073EF 0x10273417 176x144 0x3 0x0 }
MDFCodecRuleX={ H264 Extended 176x144 0x00000000 0x00000000 0x00000000 0x00000000 176x144 0x3 0x0 }

add:
MaxVideoWidth=640
MaxVideoHeight=640


optional:
SynchronizeDecoding_H264=1
DataFormat_DecoderPostProcessor_mp4=EYuvRawData, EYuvBt709Range0, EYuv420Chroma1, EYuvDataPlanar
DataFormat_DecoderPostProcessor_h264=EYuvRawData, EYuvBt709Range0, EYuv420Chroma1, EYuvDataPlanar
EnablePARAllResolution=1
DisableVideoClock=1

tested on e72, shows very different results on each video file.

requires further porting of video post processor plugin and maybe payload format plugin too.
suspecting that old armpostprochwdevice.dll does not support keyframes.

by shinovon, worked on this on 09.09.2024, 24.09.2025
