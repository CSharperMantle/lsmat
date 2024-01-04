# lsmat

Assignment 2.3 for HDU 2023 fall course *Preliminary Program Designing and Algorithms*.

Simple library for creating and manipulating sparse matrices with linked-list storage.

## Introduction

Interactively create sparse matrices and observe elementary operations on them.

```text
lsmat_cli > help
new <ID> <DIM0> <DIM1>
fillrand <ID>
set <ID> <I0> <I1> <VAL>
eval <DEST>=<EXPR>
disp <ID> <PREC>
quit
help
license
OK
lsmat_cli > new A 3 3
OK
lsmat_cli > fillrand A
OK
lsmat_cli > new 2I 3 3
OK
lsmat_cli > set 2I 0 0 2.0
OK
lsmat_cli > set 2I 1 1 2.0
OK
lsmat_cli > set 2I 2 2 2.0
OK
lsmat_cli > eval B=A*2I
OK
lsmat_cli > disp A 3
0.416 0.751 0.028 
0.268 0.729 0.312 
0.280 0.298 0.324 
OK
lsmat_cli > disp B 3
0.833 1.502 0.056 
0.537 1.459 0.624 
0.560 0.596 0.649 
OK
lsmat_cli > 
```

## License

```text
BSD 3-Clause License

Copyright (c) 2023-2024, Rong "Mantle" Bao <baorong2005@126.com>

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
```
