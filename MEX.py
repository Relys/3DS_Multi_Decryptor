#Merge Exefs Xorpads

import sys
import os
from struct import unpack

def xor(data, key): #https://stackoverflow.com/questions/5734691/fastest-bitwise-xor-between-two-multibyte-binary-data-variables
	l = len(key)
	buff = ""
	for i in range(0, len(data)):
		buff += chr(ord(data[i]) ^ ord(key[i % l]))
	return buff


if len(sys.argv) < 5:
	print 'Usage: MEX.py encryptedExefs normalCryptoXorpad 7xCryptoXorpad outputXorpad'
	raise SystemExit(0)

for x in xrange(3):
	if not os.path.isfile(sys.argv[x+1]):
		print "Input file '%s' doesn't exist." % sys.argv[x+1]
		raise SystemExit(0)

exefs = sys.argv[1]
normPadName = sys.argv[2]
_7xpadName = sys.argv[3]
outPadName = sys.argv[4]

with open(exefs, 'rb') as fh:
	exefsFiles = fh.read(0xa0)

with open(normPadName, 'rb') as fh:
	normPad = fh.read()

with open(_7xpadName, 'rb') as fh:
	_7xpad = fh.read()

decExefsFiles = xor(exefsFiles, normPad[:0xa0])

i = 0
codeLoc = -1
while(i < 16*8):
	if decExefsFiles[i:i+5] == b'.code':
		codeLoc = i
	i += 16

if codeLoc == -1:
	print '.code not found! Did you specify the input files correctly?'
	raise SystemExit(0)

codeOffs = unpack('<I', decExefsFiles[codeLoc+8:codeLoc+8+4])[0] + 0x200
codeSize = unpack('<I', decExefsFiles[codeLoc+12:codeLoc+12+4])[0]

with open(outPadName, 'wb') as fh:
	fh.write(normPad[:codeOffs])
	fh.write(_7xpad[codeOffs:codeOffs+codeSize])
	fh.write(normPad[codeOffs+codeSize:])

print 'Done!'
