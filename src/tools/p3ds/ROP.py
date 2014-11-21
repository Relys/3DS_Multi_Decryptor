# 3DS ROP library (for DS user settings exploit).
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

import struct

# Gadgets.
# Register loads.
_pop_pc = 0x001002F9
_pop_r0_pc = 0x00143D8C
_pop_r1_pc = 0x001C4FC4 #0x001549E1
_pop_r2_pc = 0x0022952D
_pop_r3_pc = 0x0010538C
_pop_r4_pc = 0x001001ED #0x001B3AA0
_pop_r1_r5_r6_pc = 0x001F1075
_pop_r4_to_r12_pc = 0x0018D5DC
_pop_r4_lr_bx_r2 = 0x001D9360
# Loads and stores.
_ldr_r0_r0_pop_r4_pc = 0x0012FBBC
_str_r1_r0_pop_r4_pc = 0x0010CCBC
# Stack pivoting.
_add_sp_r3_ldr_pc_sp_4 = 0x00143D60

# Functions.
#_memcpy = 0x001BFA64

class Ref:
	def __init__(self, _name):
		self.name = _name

class Data:
	def __init__(self, _data):
		m = len(_data) % 4
		self.data = (_data + "\x00" * (4 - m) if m else _data)

class ROP:
	def __init__(self, _base):
		self.base = _base
		self.addr = _base
		self.stack = []
		self.labels = {}

	def _append(self, v):
		self.stack.append(v)
		self.addr += 4

	def label(self, name):
		self.labels[name] = self.addr

	def ref(self, name):
		self._append(Ref(name))

	def data(self, data):
		d = Data(data)
		self.stack.append(d)
		self.addr += len(d.data)

	def i32(self, v):
		self._append(v)

	def pop_pc(self):
		self._append(_pop_pc)

	def pop_r0(self, r0):
		self._append(_pop_r0_pc)
		self._append(r0)

	def pop_r1(self, r1):
		self._append(_pop_r1_pc)
		self._append(r1)

	def pop_r2(self, r2):
		self._append(_pop_r2_pc)
		self._append(r2)

	def pop_r3(self, r3):
		self._append(_pop_r3_pc)
		self._append(r3)

	def pop_r4(self, r4):
		self._append(_pop_r4_pc)
		self._append(r4)

	def pop_r1_r5_r6(self, r1, r5, r6):
		self._append(_pop_r1_r5_r6_pc)
		self._append(r1)
		self._append(r5)
		self._append(r6)

	def pop_rX(self, **kwargs):
		regs = ['r4', 'r5', 'r6', 'r7', 'r8', 'r9', 'r10', 'r11', 'r12']
		values = [
			0x44444444, 0x55555555, 0x66666666, 
			0x77777777, 0x88888888, 0x99999999, 
			0xAAAAAAAA, 0xBBBBBBBB, 0xCCCCCCCC
		]
		for k, v in kwargs.items():
			if k not in regs:
				print "Wat? ({0})".format(k)
				return
			else:
				values[int(k[1:]) - 4] = v
		self._append(_pop_r4_to_r12_pc)
		for v in values:
			self._append(v)

	def pop_lr(self, addy):
		self.pop_r2(_pop_pc)
		self._append(_pop_r4_lr_bx_r2)
		self._append(0x44444444)
		self._append(addy)

	def load_r0(self, addy):
		self.pop_r0(addy)
		self._append(_ldr_r0_r0_pop_r4_pc)
		self._append(0x44444444)

	def store_r1(self, addy):
		self.pop_r0(addy)
		self._append(_str_r1_r0_pop_r4_pc)
		self._append(0x44444444)

	def store_i32(self, value, addy):
		self.pop_r1(value)
		self.store_r1(addy)

	def call(self, fun, args, cleancnt):
		pops = [_pop_r0_pc, _pop_r1_pc, _pop_r2_pc, _pop_r3_pc]
		if len(args) > 4:
			print "Nahhhh, not now, maybe later ({0})".format(args)
			return
		for i in xrange(len(args)):
			self._append(pops[i])
			self._append(args[i])
		self._append(fun)
		for i in xrange(cleancnt):
			self._append(0xDEADBEEF)

	def call_lr(self, fun, args):
		pops = [_pop_r0_pc, _pop_r1_pc, _pop_r2_pc, _pop_r3_pc]
		if len(args) > 4:
			print "Nahhhh, not now, maybe later ({0})".format(args)
			return
		self.pop_lr(_pop_pc)
		for i in xrange(len(args)):
			self._append(pops[i])
			self._append(args[i])
		self._append(fun)

	def mov_r4_r0(self):
		# 0x001B4F0C: adds r4, r0, r5; subs r4, r4, r7; movs r0, r4; blx r6
		self.pop_rX(r5 = 0, r6 = _pop_pc, r7 = 0)
		self._append(0x1B4F0D)

#	def memcpy(self, dst, src, size):
#		self.call(_memcpy, [dst, src, size], 7)

	def pivot(self, size): #TODO: test this	
		self.pop_r3(size)
		self._append(_add_sp_r3_ldr_pc_sp_4)

	def gen(self):
		res = ""
		for s in self.stack:
			if isinstance(s, Ref):
				res += struct.pack("<I", self.labels[s.name])
			elif isinstance(s, Data):
				res += s.data
			else:
				res += struct.pack("<I", s)
		return res
