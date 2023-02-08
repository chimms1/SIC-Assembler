# SICXE-Assembler
This is a naïve implementation of two-pass SICXE Assembler by [Shreyas Joshi](https://github.com/shreyasjoshi1234) and [Yash Deshpande](https://github.com/chimms1) done as part of System Software course during Undergrad.

### Theory and algorithms are referred from the book 'System Software - An introduction to Systems Programming' by Leland L. Beck

Along with the final version, various iterations of development are also included which can be used as a reference by the interested ones.

### Usage
The program can be used in the following steps:
1. Keep the SICXE input assembly program in input.txt and OPTABLE in OPTAB.txt
2. Run the pass1 code which will generate corresponding output files (intermediate for pass1).

##### Pass1 of few machine independent features of SIC such as Program Blocks, ORG, EQU... are achieved by modifications in the main program.
##### Similarly, SIC assembler code is changed to SICXE assembler

#### Note:- We have used the bruteforce approach while programming the algorithms, which can also be implemented in various efficient ways. The code may also lack documentation in few places and might contain some workarounds to resolve errors which can be handled in a better way
