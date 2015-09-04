import struct
import os
from binascii import hexlify

with open('decTitleKeys.bin', 'rb') as fh:
	#struct.unpack would cause difference between 32 and 64 systems.
	#nEntries = struct.unpack('<I', fh.read(4))[0]
	nEntries = os.fstat(fh.fileno()).st_size / 32
	fh.seek(16, os.SEEK_SET)
	
	for i in xrange(nEntries):
		fh.seek(8, os.SEEK_CUR)
		titleId = fh.read(8)
		decryptedTitleKey = fh.read(16)
		print '%s: %s' % (hexlify(titleId), hexlify(decryptedTitleKey))
