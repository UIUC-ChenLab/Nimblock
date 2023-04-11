#ifndef __COMPRESSION_H__
#define __COMPRESSION_H__

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define IMAGE_MAX_M 1920
#define IMAGE_MAX_N 1080
#define IMAGE_MAX IMAGE_MAX_M*IMAGE_MAX_N

typedef float DTYPE;
typedef float QTYPE;

#define DZERO 0.00f
#define QZERO 0

int padImage(volatile DTYPE *src, volatile DTYPE *dst,  int srcm,  int srcn,  int dstm,  int dstn);
int extractBlockD(volatile DTYPE *src, volatile DTYPE *dst,  int srcm,  int srcn);

int bundle1(volatile DTYPE *src, volatile DTYPE *dst, int srcm,  int srcn,  int dstm,  int dstn);

int matmul_fast(DTYPE srcA[8][8],DTYPE srcB[8][8], DTYPE dst[8][8]);
int dct(volatile DTYPE *src, volatile DTYPE *dst, int srcm, int srcn);
int idct(volatile DTYPE *src, volatile DTYPE *dst, int srcm, int srcn);
int quantize(volatile DTYPE *src, volatile QTYPE *dst, int srcm, int srcn);
int dequantize(volatile QTYPE *src, volatile DTYPE *dst, int srcm, int srcn);
int reconstructBlockD(volatile DTYPE *src, volatile DTYPE *dst,  int srcm,  int srcn);

#endif
