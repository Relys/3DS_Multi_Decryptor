def hexwrite(src,filename):
	f = open(filename, "wb")
	f.write(src)
	f.close()


def hexdump(src, base=0, length=16, sep='.'):
	FILTER = ''.join([(len(repr(chr(x))) == 3) and chr(x) or sep for x in range(256)])
	lines = []
	for c in xrange(0, len(src), length):
		chars = src[c:c+length]
		hex = ' '.join(["%02x" % ord(x) for x in chars])
		if len(hex) > 24:
			hex = "%s %s" % (hex[:24], hex[24:])
		printable = ''.join(["%s" % ((ord(x) <= 127 and FILTER[ord(x)]) or sep) for x in chars])
		lines.append("%08X: %-*s |%s|\n" % (base + c, length*3, hex, printable))
	print ''.join(lines)
