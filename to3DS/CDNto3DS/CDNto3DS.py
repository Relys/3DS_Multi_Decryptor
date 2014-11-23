#This script is old and shitty

import os
import sys
import urllib2
from struct import unpack, pack
from subprocess import call
from binascii import hexlify


##########From https://stackoverflow.com/questions/5783517/downloading-progress-bar-urllib2-python
def chunk_report(bytes_so_far, chunk_size, total_size):
	percent = float(bytes_so_far) / total_size
	percent = round(percent*100, 2)
	sys.stdout.write("Downloaded %d of %d bytes (%0.2f%%)\r" % (bytes_so_far, total_size, percent))

	if bytes_so_far >= total_size:
		sys.stdout.write('\n')

def chunk_read(response, outfname, chunk_size=2*1024*1024, report_hook=None):
	fh = open(outfname,'wb')
	total_size = response.info().getheader('Content-Length').strip()
	total_size = int(total_size)
	bytes_so_far = 0
	data = []

	while 1:
		if report_hook:
			report_hook(bytes_so_far, chunk_size, total_size)

		chunk = response.read(chunk_size)
		bytes_so_far += len(chunk)

		if not chunk:
			 break

		fh.write(chunk)

	fh.close()
##########



if len(sys.argv) < 4:
	print 'Usage: CDNto3DS.py TitleID TitleKey cardType'
	print "cardType: valid options are 'card1' and 'card2'"
	raise SystemExit(0)


titleid = sys.argv[1]
titlekey = sys.argv[2]
cardType = sys.argv[3].lower()
if cardType not in ['', 'card1', 'card2']:
	print 'Invalid cardType.'
	raise SystemExit(0)
cardType = ['', 'card1', 'card2'].index(cardType)

if len(titleid) != 16 or len(titlekey) != 32:
	print 'Invalid arguments'
	raise SystemExit(0)

eShopContent = ['00040000', '00040002']
if titleid[:8] not in eShopContent:
	print 'This only works with eShop content.'
	raise SystemExit(0)


baseurl = 'http://nus.cdn.c.shop.nintendowifi.net/ccs/download/' + titleid

print 'Downloading TMD...'

try:
	tmd = urllib2.urlopen(baseurl + '/tmd')
except urllib2.URLError, e:
	print 'ERROR: Bad title ID?'
	raise SystemExit(0)

tmd = tmd.read()

open('tmd.out','wb').write(tmd)
print 'Done\n'


if tmd[:4] != '\x00\x01\x00\x04':
	print 'Unexpected signature type.'
	raise SystemExit(0)

contentCount = unpack('>H', tmd[0x206:0x208])[0]

if contentCount > 8:
	print 'Content count too high.'
	raise SystemExit(0)

print 'Content count: ' + str(contentCount) + '\n'

indextypes = [' (Main Content)', ' (Manual)', '(Download Play container)', '', '', '', '', '']

mRomCmd = 'makerom -f cci -rsf rom.rsf -o ' + titleid + '.3ds -nomodtid'

fSize = 16*1024

for i in xrange(contentCount):
	cOffs = 0xB04+(0x30*i)
	cID = unpack('>I', tmd[cOffs:cOffs+4])[0]
	cIDX = unpack('>H', tmd[cOffs+4:cOffs+6])[0]
	print 'Content ID:    ' + str(cID).zfill(8)
	print 'Content Index: ' + str(cIDX).zfill(8) + indextypes[cIDX]

	outfname = titleid + '.' + str(cID).zfill(8)
	response = urllib2.urlopen(baseurl + '/' + str(cID).zfill(8))
	chunk_read(response, outfname, report_hook=chunk_report)

	call(["aescbc", outfname, outfname + '_dec', titlekey, str(cIDX).zfill(4) + '0000000000000000000000000000'])

	with open(outfname + '_dec','rb') as fh:
		fh.seek(0x100)
		if fh.read(4) != 'NCCH':
			print 'Decryption failed. Wrong title key?'
			raise SystemExit(0)
		fh.seek(0, os.SEEK_END)
		fSize += fh.tell()

	print '\n'
	mRomCmd = mRomCmd + ' -content ' + outfname + '_dec' + ':' + str(cIDX)


romrsf = ''

romrsf1 = 'CardInfo:\n  MediaSize               : '
romrsf2 = '\n  MediaType               : '
romrsf3 = '\n  CardDevice              : '
romrsf4 = '\n\nOption:\n  MediaFootPadding: true'
romrsf5 = '\n\nSystemControlInfo:\n  SaveDataSize: 512KB\n'

mediaSizes = ['128MB', '256MB', '512MB', '1GB', '2GB', '4GB', '8GB']
romSizes = [128*1024*1024, 256*1024*1024, 512*1024*1024, 1*1024*1024*1024, 2*1024*1024*1024, 4*1024*1024*1024, 8*1024*1024*1024]
sizeIDX = min(range(len(romSizes)), key=lambda i: abs(romSizes[i]-fSize))
if romSizes[sizeIDX] < fSize:
	sizeIDX += 1

if cardType == 1: #Card1
	romrsf = romrsf1 + mediaSizes[sizeIDX] + romrsf2 + 'Card1' + romrsf3 + 'NorFlash' + romrsf4 + romrsf5
else: #Card2
	romrsf = romrsf1 + mediaSizes[sizeIDX] + romrsf2 + 'Card2' + romrsf3 + 'None' + romrsf4

with open('rom.rsf', 'wb') as fh:
	fh.write(romrsf)

print 'Building ' + titleid + '.3ds...\n'

os.system(mRomCmd)

if not os.path.isfile(titleid + '.3ds'):
	print "Something went wrong."
	raise SystemExit(0)

print 'Done!'
