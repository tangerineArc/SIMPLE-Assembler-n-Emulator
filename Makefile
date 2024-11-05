assembler: asm.c
	@gcc -std=c89 -pedantic -W -Wall -Wpointer-arith -Wwrite-strings -Wstrict-prototypes -c -o utils.o ./lib/utils.c
	@gcc -std=c89 -pedantic -W -Wall -Wpointer-arith -Wwrite-strings -Wstrict-prototypes -c -o dataStructures.o ./lib/dataStructures.c
	@gcc -std=c89 -pedantic -W -Wall -Wpointer-arith -Wwrite-strings -Wstrict-prototypes -c -o asm.o asm.c
	@gcc -std=c89 -pedantic -W -Wall -Wpointer-arith -Wwrite-strings -Wstrict-prototypes -o assemble.exe asm.o dataStructures.o utils.o
	@rm utils.o dataStructures.o asm.o
	@echo ">>> by the grace of Swagatam 'assemble.exe' has been created in the current directory"
emulator: emu.c
	@gcc -std=c89 -pedantic -W -Wall -Wpointer-arith -Wwrite-strings -Wstrict-prototypes -c -o utils.o ./lib/utils.c
	@gcc -std=c89 -pedantic -W -Wall -Wpointer-arith -Wwrite-strings -Wstrict-prototypes -c -o dataStructures.o ./lib/dataStructures.c
	@gcc -std=c89 -pedantic -W -Wall -Wpointer-arith -Wwrite-strings -Wstrict-prototypes -c -o emu.o emu.c
	@gcc -std=c89 -pedantic -W -Wall -Wpointer-arith -Wwrite-strings -Wstrict-prototypes -o emulate.exe emu.o dataStructures.o utils.o
	@rm utils.o dataStructures.o emu.o
	@echo ">>> by the grace of Swagatam 'emulate.exe' has been created in the current directory"
clean-asm:
	@rm assemble.exe
	@echo ">>> annihilated 'assemble.exe'"
clean-emu:
	@rm emulate.exe
	@echo ">>> annihilated 'emulate.exe'"