assemble: asm.c
	gcc -std=c89 -pedantic -W -Wall asm.c -o assemble.exe
clean:
	rm assemble.exe