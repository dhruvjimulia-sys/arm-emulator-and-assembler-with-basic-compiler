#!/bin/bash
cd compile
./compiler tester/test_cases/rock_paper_scissors.bas ../assembler/rps.s
cd ../assembler
rm -f rps
./assemble rps.s rps
rm -f rps.s
mv rps ../emulator
cd ../emulator
./emulate rps -s