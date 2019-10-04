// tpt-prototype.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#define SIMULATIONW 800
#define SIMULATIONH 600

#define WINDOWW 800
#define WINDOWH 600

#define TYPE_NONE 0
#define TYPE_SOLID 1
#define TYPE_POWDER 2
#define TYPE_LIQUID 3
#define TYPE_GAS 4
#define TYPE_PARTICLE 5

#define PART(x, y) (x) + ((y) * SIMULATIONW)

#define PART_POS_QUANT(x) ((int)(x + 0.5f))

#define PIX(x, y) (x) + ((y) * WINDOWW)

// TODO: Reference additional headers your program requires here.
