# SIMPLE Assembler & Emulator

The project consists of an assembler to convert assembly-language programs, written as per the provided SIMPLE instruction-set, into executable machine-code. The machine-code can then be executed by an emulator.

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
