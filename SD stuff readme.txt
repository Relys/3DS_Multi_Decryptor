NOTE: THIS ONLY WORKS WITH FILES ENCRYPTED BY YOUR OWN 3DS.
NOTE: THIS CAN DECRYPT CONTENT PURCHASED ON EMUNAND, BUT ONLY IF YOU HAVEN'T SYSTEM FORMATTED YOUR SYSNAND AFTER CREATING THE EMUNAND*.

*It is possible, but outside the scope of these tools.


usage: SDinfo_gen.py folderpath
folderpath: folder on your SD that contains "dbs", "title", etc.

For example, SDinfo_gen.py "X:/Nintendo 3DS/<ID0>/<ID1>/"


Place the generated SDinfo.bin on your 3DS SD card(you can use a different SD card for this part),
and either Launcher.dat or MsetForBoss.dat depending on which ROP loader you use.
Then run it(System Settings -> DS Profile).
This will generate xorpads for the encrypted files that SDinfo_gen.py found on your SD, so make sure you have enough free space.
If you get a blank white screen, turn your 3DS off and back on and try again.

Once you have the xorpads, you can use them with padxorer to decrypt your files(it should be obvious by the filenames which xorpads to use on what files).



SDinfo.bin format:

4 bytes  Number of entries


entry:
    16 bytes  Counter
     4 bytes  File size in Megabytes(rounded up)
   180 bytes  Output filename in UTF16



Counter generation(info from http://3dbrew.org/wiki/Extdata):

  First, take the path of the file you want you want to decrypt.
  Let's use 'X:/Nintendo 3DS/<ID0>/<ID1>/extdata/00000000/0000008f/00000000/00000001' ('00000001' is the file, it has no extension)
  We only need what's after X:/Nintendo 3DS/<ID0>/<ID1>
  So, '/extdata/00000000/0000008f/00000000/00000001'
  Convert that to UTF16, add two null bytes to the end, then take the SHA256.
  The counter is then CTRword[i] = Hashword[i] ^ Hashword[4+i]

  For the example file, the counter is {0x24, 0xE2, 0xF7, 0x91, 0x92, 0x6A, 0xDC, 0x96, 0x49, 0x4E, 0x81, 0xA9, 0x7B, 0x3C, 0x90, 0xCF}


  This works for anything in 'extdata', 'dbs', 'title', and possibly 'backups'(haven't tested this one)