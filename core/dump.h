#pragma once

#include <stdint.h>
#include <stdalign.h>
#include <stdio.h>

#define MAGIC "#dump002"
#define ALIGNTODOUBLE sizeof(double)
#define CRCSIZE 8
#define DUMPEXT ".dump"
#define DUMPFIELDSIZE sizeof(int64_t)

// 8 byte alighment
struct DUMP {
    alignas(ALIGNTODOUBLE) char magic[DUMPFIELDSIZE]; // +0
    alignas(ALIGNTODOUBLE) int64_t components; // +8
    alignas(ALIGNTODOUBLE) int64_t meshSize[3]; // +16
    alignas(ALIGNTODOUBLE) double meshStep[3]; // +40
    alignas(ALIGNTODOUBLE) char meshUnit[DUMPFIELDSIZE]; // +64
    alignas(ALIGNTODOUBLE) double time; // +72
    alignas(ALIGNTODOUBLE) char timeUnit[DUMPFIELDSIZE]; // +80
    alignas(ALIGNTODOUBLE) char dataLabel[DUMPFIELDSIZE]; // +88
    alignas(ALIGNTODOUBLE) char dataUnit[DUMPFIELDSIZE]; // +96
    alignas(ALIGNTODOUBLE) uint64_t precission; // +104
    alignas(ALIGNTODOUBLE) float data[]; // +112 but should be on 128!!!!
    // discard CRC
};

typedef struct DUMP dump;
#define DUMPHEADERSIZE sizeof(dump)

size_t dsettoheader(FILE *f);
size_t dsettodata(FILE *f);
size_t dloadheader(FILE *f, dump **frame);
size_t dsaveheader(FILE *f, dump *frame);
size_t dsizeofdatacomponent(dump *frame);
size_t dsizeofdata(dump *frame);
size_t dsizeofframe(dump *frame);
size_t dsizeofheader(dump *frame);
size_t dfinilize(FILE *f, dump *frame);
size_t dloadsinglefloat(FILE *f, size_t i, float **val);
size_t dloadchunkfloat(FILE *f, size_t i, size_t size, float **buf);
size_t dsavesinglefloat(FILE *f, size_t i, float *val);
size_t dsavechunkfloat(FILE *f, size_t i, size_t size, float *val);

dump *dcloneheader(dump *src);
