# CrcCheckForDalivkCache
use crc32 checksum to protect dalvik-cache

in post L  android versions , google add a boot flag  to protect files under dalvik-cache.
but, this operation can't detect bits flip in those odex files and boot-fail after boot-complete.
so, i add a new way to detect bit-flip of dex files.

# Usage:
put crc.c into  system/core/toolbox/
add crc.c in   system/core/toolbox/Android.mk
modify art/runtime/gc/spaces/image_spaces.cc ,replace checking .boot flag to check crc32-checksums of the dexfiles while a abnormal shutdown was detected.
modify frameworks/base/xxx/PowerManagerService.java add shutdown flag.
# Cost:
125 files under dalvik-cache :
crc check cost 500ms
crc create cost 600ms




