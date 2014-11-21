import sys;

f1 = 0
f2 = 0
f3 = 0

with open(sys.argv[1], "rb") as file1:
	with open(sys.argv[2], "rb") as file2:
		f1 = file1.read()
		f2 = file2.read()

with open(sys.argv[3], "w+b") as file3:
	file3.write(f1);
	#file3.write(chr(0xDE));
	#file3.write(chr(0xAD));
	#file3.write(chr(0xBE));
	#file3.write(chr(0xEF));
	file3.write(f2);