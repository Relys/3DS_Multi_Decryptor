# Example RAM dumper.
# Copyright (C) 2013 naehrwert
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, version 2.0.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License 2.0 for more details.
# 
# A copy of the GPL 2.0 should have been included with the program.
# If not, see http://www.gnu.org/licenses/

from p3ds.util import *
from p3ds.ROP import *

r = ROP(0x002B0000)

# Clear 0x279004 and 0x279008

#r.pop_r4(0x279004)
#r.pop_r1(0x279008)
#r.i32(0x101AAC)
#r.i32(0x000004)

#r.pop_r1(0)
#r.store_r1(0x279004)
#r.store_r1(0x279008)

r.store_i32(0, 0x279004)
r.store_i32(0, 0x279008)

# file_open(0x270000, "YS:/DUMP.BIN", 6)
r.call(0x1B82AC, [0x279000, Ref("fname"), 6], 5)
# file_write(0x270000, 0x279020, 0x100000, 0x300000)
r.call(0x1B3B54, [0x279000, 0x279020, 0x100000, 0x300000], 9)

# Data.
r.label("fname")
r.data("\x59\x00\x53\x00\x3A\x00\x2F\x00\x44\x00\x55\x00\x4D\x00\x50\x00\x2E\x00\x42\x00\x49\x00\x4E\x00\x00\x00")

rop = r.gen()
hexdump(rop, base=0x2B0000)

#f = open("Launcher.dat", "wb")
#f.write(rop)
#f.close()
