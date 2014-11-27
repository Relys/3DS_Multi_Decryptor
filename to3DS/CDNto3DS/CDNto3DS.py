#This script is old and shitty

import os
import errno
import sys
import urllib2
from struct import unpack, pack
from subprocess import call
from binascii import hexlify


def mkdir_p(path):
    try:
        os.makedirs(path)
    except OSError as exc: # Python >2.5
        if exc.errno == errno.EEXIST and os.path.isdir(path):
            pass
        else: raise

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

def SystemUsage():
	print 'Usage: CDNto3DS.py TitleID TitleKey [-redown -redec -no3ds -nocia]'
	print '-redown : redownload content'
	print '-redec  : re-attempt content decryption'
	print '-no3ds  : don\'t build 3DS file'
	print '-nocia  : don\'t build CIA file'
	raise SystemExit(0)

if len(sys.argv) < 3:
	SystemUsage()

titleid = sys.argv[1]
titlekey = sys.argv[2]
forceDownload = 0
forceDecrypt = 0
make3ds = 1
makecia = 1

for i in xrange(len(sys.argv)) :
	if sys.argv[i] == '-redown': forceDownload = 1
	elif sys.argv[i] == '-redec': forceDecrypt = 1
	elif sys.argv[i] == '-no3ds': makecia = 0
	elif sys.argv[i] == '-nocia': make3ds = 0
	#else : SystemUsage()
	
if len(titleid) != 16 or len(titlekey) != 32:
	print 'Invalid arguments'
	raise SystemExit(0)

baseurl = 'http://nus.cdn.c.shop.nintendowifi.net/ccs/download/' + titleid

print 'Downloading TMD...'

try:
	tmd = urllib2.urlopen(baseurl + '/tmd')
except urllib2.URLError, e:
	print 'ERROR: Bad title ID?'
	raise SystemExit(0)

tmd = tmd.read()

mkdir_p(titleid)
open(titleid + '/tmd','wb').write(tmd)
print 'Done\n'

if tmd[:4] != '\x00\x01\x00\x04':
	print 'Unexpected signature type.'
	raise SystemExit(0)

# If Not Application Title Exit
if titleid[:8] != '00040000':
	make3ds = 0

mCiaCmd = 'makerom -f cia -rsf rom.rsf -o ' + titleid + '.cia'
mRomCmd = 'makerom -f cci -rsf rom.rsf -nomodtid -o ' + titleid + '.3ds'

# Set Proper CommonKey ID
if unpack('>H', tmd[0x18e:0x190])[0] & 0x10 == 0x10 :
	mCiaCmd = mCiaCmd + ' -ckeyid 1'
else :
	mCiaCmd = mCiaCmd + ' -ckeyid 0'
	
# Set Proper Version
version = unpack('>H', tmd[0x1dc:0x1de])[0]
mCiaCmd = mCiaCmd + ' -major ' + str((version & 0xfc00) >> 10) + ' -minor ' + str((version & 0x3f0) >> 4) + ' -micro ' + str(version & 0xF)

# Set Save Size
saveSize = (unpack('<I', tmd[0x19a:0x19e])[0])/1024
mCiaCmd = mCiaCmd + ' -DSaveSize=' + str(saveSize)
mRomCmd = mRomCmd + ' -DSaveSize=' + str(saveSize)

# If DLC Set DLC flag
if titleid[:8] == '0004008c':
	mCiaCmd = mCiaCmd + ' -dlc'
	
contentCount = unpack('>H', tmd[0x206:0x208])[0]

print 'Content count: ' + str(contentCount) + '\n'
if contentCount >= 8 :
	make3ds = 0

	
# Download Contents
fSize = 16*1024
for i in xrange(contentCount):
	cOffs = 0xB04+(0x30*i)
	cID = format(unpack('>I', tmd[cOffs:cOffs+4])[0], '08x')
	cIDX = format(unpack('>H', tmd[cOffs+4:cOffs+6])[0], '04x')
	if unpack('>H', tmd[cOffs+4:cOffs+6])[0] >= 8 :
		make3ds = 0

	cOffs = 0xB04+(0x30*i)
	cID = format(unpack('>I', tmd[cOffs:cOffs+4])[0], '08x')
	cIDX = format(unpack('>H', tmd[cOffs+4:cOffs+6])[0], '04x')
	if unpack('>H', tmd[cOffs+4:cOffs+6])[0] >= 8 :
		make3ds = 0
	
	print 'Content ID:    ' + cID
	print 'Content Index: ' + cIDX

	outfname = titleid + '/' + cID
	if os.path.exists(outfname) == 0 or forceDownload == 1:
		response = urllib2.urlopen(baseurl + '/' + cID)
		chunk_read(response, outfname, report_hook=chunk_report)

	if os.path.exists(outfname + '.dec') == 0 or forceDecrypt == 1:
		call(["aescbc", outfname, outfname + '.dec', titlekey, cIDX + '0000000000000000000000000000'])

	with open(outfname + '.dec','rb') as fh:
		fh.seek(0x100)
		if fh.read(4) != 'NCCH':
			print 'Decryption failed. Wrong title key?'
			raise SystemExit(0)
		fh.seek(0, os.SEEK_END)
		fSize += fh.tell()
		
	print '\n'
	mCiaCmd = mCiaCmd + ' -i ' + outfname + '.dec' + ':0x' + cIDX
	mRomCmd = mRomCmd + ' -i ' + outfname + '.dec' + ':0x' + cIDX + ':0x' + cID

# Create RSF File
romrsf = 'Option:\n  MediaFootPadding: true\n  EnableCrypt: false\nSystemControlInfo:\n  SaveDataSize: $(SaveSize)K'
with open('rom.rsf', 'wb') as fh:
	fh.write(romrsf)
	
if makecia == 1:
	print '\nBuilding ' + titleid + '.cia...'
	#print mCiaCmd
	os.system(mCiaCmd)

if make3ds == 1:
	print '\nBuilding ' + titleid + '.3ds...'
	#print mRomCmd
	os.system(mRomCmd)
	
os.remove('rom.rsf')

if not os.path.isfile(titleid + '.cia') and makecia == 1:
	print "Something went wrong."
	raise SystemExit(0)

if not os.path.isfile(titleid + '.3ds') and make3ds == 1:
	print "Something went wrong."
	raise SystemExit(0)
	
print 'Done!'
