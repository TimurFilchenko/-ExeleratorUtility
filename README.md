# Exelerator Utility

Exelerator Utility is a Windows x64 command-line program written in C.

It is designed to find duplicate files and help the user safely remove unnecessary copies.

## How It Works

Exelerator scans a selected directory and its subdirectories.

The program:

1. Finds files.
2. Compares their sizes.
3. Calculates SHA-256 hashes for files with the same size.
4. Groups files with identical content.
5. Shows the duplicate groups.
6. Lets the user select files to remove.
7. Asks for confirmation before deleting.

## Running

No installation is required.

Run the program from Windows Terminal:

```text
Exelerator.exe

You can also start it directly from File Explorer.

Building

The project uses C, Make, and LLVM-MinGW.

Build the project with:

make

The executable will be created at:

build/Exelerator.exe

To remove build files:

make clean

Project Structure

Exelerator/
├── Makefile
├── README.md
├── include/
├── src/
└── tests/

Main modules:

core       Application logic
scanner    Directory scanning
hash       File hashing
duplicate  Duplicate detection
file       File operations
ui         Console interface
tests      Project tests

Safety

Exelerator does not automatically delete duplicate files.

The user must select the files to remove and confirm the deletion.

The program should always keep at least one copy from each duplicate group.

Requirements

To run the compiled program:

Windows x64

To build the project:

C compiler
LLVM-MinGW
Make

Version

0.1.0
