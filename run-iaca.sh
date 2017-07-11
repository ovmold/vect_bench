#!/bin/sh

# Intel Sandy Bridge
#iaca -arch SNB -o iaca-out.out -graph iaca-out  ./matmul
# Intel Haswell
#iaca -arch HSW -o ./iaca/iaca-out.out -graph ./iaca/iaca-out  ./matmul

# Intel Broadwell
iaca -arch BDW -o ./iaca/iaca-out.out -graph ./iaca/iaca-out  ./matmul
cd iaca
dot -Tps ./iaca-out1.dot > ./iaca-out.ps

