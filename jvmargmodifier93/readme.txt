Patched Midp2VmArgModifier.dll to remove "Note! Modified VM arguments used!" alert

original dll can be found there:
newer 3524 bytes version in EcmtAgent.sis from Nokia N97 SDK
older 3473 bytes version in midpODDUtility.sis

unpacked
skipped CAknGlobalNote::ShowNoteL usages

patch in hex:
256-279: 9C 3A -> 02 E0

by shinovon, worked on this on 04.02.2026
