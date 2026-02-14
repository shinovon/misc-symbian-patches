tool to fix integer division imports for compatibility with older symbian versions.

used in h264 decoder research

replaces drtaeabi imports:
222 (__aeabi_idiv) => 135 (__aeabi_idivmod)
223 (__aeabi_uidiv) => 155 (__aeabi_uidivmod)

written by gemini, 22.09.2025
