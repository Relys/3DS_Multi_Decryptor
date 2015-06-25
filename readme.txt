3DS multitool thing.

Functions:
  NCCH padgen                 -- use ctrkeygen.py to generate the needed ncchinfo.bin
  SD padgen                   -- use SDinfo_gen.py to generate the needed SDinfo.bin
  Title key decrypter         -- check out the scripts in 'ticket-titlekey_stuff'
  NAND FAT16 partition padgen --self-explanatory
  NAND dumper                 --self-explanatory

Controls:
  DPAD Up/Down: change selection
  A: select item
  B: return to menu(after your selected process finishes)


YOU NEED TO SUPPLY YOUR OWN COPY OF slot0x25KeyX.bin.

To build for iQUE (Chinese 3DS) please change the following line in rop.py
_pop_r2_pc = 0x0022952D ### for ique change to 0x00229565

Thanks to enler for finding the correct gadget offset.

credits:

sbJFn5r - coding + initial versions of the python scripts
relys - coding
xerpi - did some refactoring early on, that was then built off of and made ugly again. :(
CaitSith2 - Updates to CDNto3DS
idunoe - Allow user specified moveable.sed
einstein95 - Padding from .rsf

Thanks:

yellows8, plutooo and everyone else that contributes to the 3dbrew wiki
megazig for the crypto functions
some GBAtemper for the FS functions. Don't know who you are, but the person I got them from said they came from someone on GBAtemp.
