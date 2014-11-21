import struct

knowntids = open('knowntitleids.txt', 'rb').readlines()
knowntids = [x.rstrip() for x in knowntids] #Remove line breaks

i = 0
with open('../nand.fat16.bin', 'rb') as fh:
	while 1:
		data = fh.read(4*1024)

		x = data.find('NCCH')
		if x != -1:
			foundpos = x + (i*4*1024)
			#print 'Found NCCH at: ' + str(foundpos)
			savedpos = fh.tell()
			
			fh.seek(foundpos + 0x18)
			titleid = fh.read(8)
			titleid = ''.join('%02X' % ord(x) for x in titleid[::-1])
			if not titleid in knowntids:
				print titleid
				fh.seek(foundpos + 4)
				size = struct.unpack('<I', fh.read(4))[0] * 512
				fh.seek(foundpos - 0x100)
				with open(titleid + '.ncch', 'wb') as title:
					title.write(fh.read(size))
			
			fh.seek(savedpos)
		i += 1

		if len(data) < (4*1024):
			break