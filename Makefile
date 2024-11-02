assemble: asm.c
	gcc -std=c89 -pedantic -W -Wall -c -o utils.o ./lib/utils.c
	gcc -std=c89 -pedantic -W -Wall -c -o dataStructures.o ./lib/dataStructures.c
	gcc -std=c89 -pedantic -W -Wall -c -o asm.o asm.c
	gcc -std=c89 -pedantic -W -Wall -o assemble.exe asm.o dataStructures.o utils.o
	rm utils.o
	rm dataStructures.o
	rm asm.o
clean:
	rm assemble.exe