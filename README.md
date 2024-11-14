# SIMPLE Assembler & Emulator

The project consists of a two-pass assembler to convert assembly-language programs, written as per the provided SIMPLE instruction-set, into executable machine-code.  
The machine-code can then be executed by an emulator.

## Setup Instructions
Clone this repository:
```
git clone https://github.com/tangerineArc/Simple-Assembler-n-Emulator.git
```
> [!NOTE]
> If your system (particularly Windows and macOS) does not have `make` installed, install `make` first before proceeding.
>
> Also, ensure that `gcc` is installed

Compile `asm.c`. On successful compilation `assemble.exe` will be created in the current directory.
```
make assembler
```
Compile `emu.c`. On successful compilation `emulate.exe` will be created in the current directory.
```
make emulator
```

## Usage Instructions
Assemble an assembly program. On successful assembly, `machine-code.out` and `listing.lst` will be created in the current directory.
```
./assemble.exe file-path
```
Execute the program using emulator.
```
./emulate.exe -option object-file-path
```
To get a list of available emulator options:
```
./emulate.exe -man
```

## Project Filesystem Explanation
Below is a list of all the files and directories that the project contains:
- `asm.c` - assembler source-code
- `emu.c` - emulator source-code
- `/headers` - contains header files
  - `dataStructures.h`
  - `utils.h`
- `/lib` - contains essential functions and data-structure implementations
  - `dataStructures.c`
  - `utils.c`
- `claims.txt` - contains claims regarding the project
- `/bubbleSort` - contains assembly program, console output as log-file, listing, object-file for bubbl-sort program
- `/sampleTests` - contains provided sample programs, console outputs as log-files, listings, object-files
- `/additionalTests` - contains additional assembly programs, console output as log-files, listings, object-files

# Assembly Language
The assembly language is for a machine with four registers:
- Two registers: **A** & **B**, arranged as an internal stack
- A program counter: **PC**
- A stack pointer: **SP**

These registers are 32 bits in size.  
The encoding uses the bottom 8 bits for opcode and the upper 24 bits for operand.  

As with most assembly languages, this is line-based i.e. one statement per line.  
Comments begin with a '**;**' and anything on the line after the '**;**' is ignored.  

Blank lines and lines containing only a comment are permitted and ignored.  
White space (including tabs) are permitted at the beginning of a line and ignored.

Instructions have either no operands or a single operand. The operand is a signed two's complement value.  
An operand is either a label or a number. The number can be decimal, hexadecimal or octal.

Label definitions consist of the label name followed by a '**:**', and an optional statement.  
There is not necessarily a space between the '**:**' and the statement.

A label use is just the label name. A valid label name is an alphanumeric string beginning with a letter.  
For branch instructions label use should calculate the branch displacement.  
For non-branch instructions, the label value should be used directly.

# SIMPLE Instruction Set
The instruction semantics do not show the incrementing of the **PC** to the next instruction.  
This is implicitly performed by each instruction before the actions of the instruction are done. 

| Mnemonic | Opcode | Operand | Formal Specification | Description |
| --- | --- | --- | --- | --- |
| data |  | value | | reserve a memory location, initialized to the value specified |
| ldc | 0 | value | B := A; A := value; | load accumulator with the value specified |
| adc | 1 | value | A := A + value; | add the value specified to the accumulator |
| ldl | 2 | offset | B := A; A := memory[SP + offset]; | load local |
| stl | 3 | offset | memory[SP + offset] := A; A := B; | store local |
| ldnl | 4 | offset | A := memory[A + offset]; | load non-local |
| stnl | 5 | offset | memory[A + offset] := B; | store non-local |
| add | 6 | | A := B + A; | addition |
| sub | 7 | | A := B - A; | subtraction |
| shl | 8 | | A := B << A; | shift left |
| shr | 9 | | A := B >> A; | shift right |
| adj | 10 | value | SP := SP + value; | adjust SP |
| a2sp | 11 | | SP := A; A := B | transfer A to SP |
| sp2a | 12 | | B = A; A := SP; | transfer SP to A |
| call | 13 | offset | B := A; A := PC; PC := PC + offset; | call procedure |
| return | 14 | | PC := A; A := B; | return from procedure |
| brz | 15 | offset | if A == 0 then PC := PC + offset; | if accumulator is zero, branch to specified offset |
| brlz | 16 | offset | if A < 0 then PC := PC + offset; | if accumulator is less than zero, branch to specified offset |
| br | 17 | offset | PC := PC + offset; | branch to specified offset |
| HALT | 18 | | | stop the emulator; not a 'real' instruction; needed to tell emulator when to finish |
| SET | | value | | set the label on current line to the specified value; is a pseudo-instruction |