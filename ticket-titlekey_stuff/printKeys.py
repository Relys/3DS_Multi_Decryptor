import struct
import os
from binascii import hexlify

with open('decTitleKeys.bin', 'rb') as fh:
	nEntries = struct.unpack('<I', fh.read(4))[0]
	fh.seek(12, os.SEEK_CUR)
	
	for i in xrange(nEntries):
		fh.seek(8, os.SEEK_CUR)
		titleId = fh.read(8)
		decryptedTitleKey = fh.read(16)
		print '%s: %s' % (hexlify(titleId), hexlify(decryptedTitleKey))
