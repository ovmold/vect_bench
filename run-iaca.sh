#!/bin/sh

# Intel Sandy Bridge
iaca -arch SNB -o iaca-out.out -graph iaca-out  ./matmul
dot -Tps ./iaca-out1.dot > iaca-out.ps

