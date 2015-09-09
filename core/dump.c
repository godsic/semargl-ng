#include "core/dump.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t dloadheader(FILE *f, dump **frame)
{
    size_t success;
    success = dsettoheader(f);
    if (success != 0)
        return 1;
    success = fread((void*)(*frame), 1, DUMPHEADERSIZE, f);
    if (success != DUMPHEADERSIZE)
        return 1;

    if (memcmp((*frame)->magic, MAGIC, 8) != 0)
    {
        return 1;
    }
    return 0;
}

size_t dsaveheader(FILE *f, dump *frame)
{
    size_t success;
    success = dsettoheader(f);
    if (success != 0)
        return 1;
    success = fwrite((void *)frame, 1, DUMPHEADERSIZE, f);
    if (success != DUMPHEADERSIZE)
        return 1;
    return 0;
}

size_t dsettoheader(FILE *f)
{
    return fseek(f, 0, SEEK_SET);
}

size_t dsettodata(FILE *f)
{
    return fseek(f, DUMPHEADERSIZE, SEEK_SET);
}

size_t dsizeofdatacomponent(dump *frame)
{
    return frame->meshSize[0] * frame->meshSize[1] * frame->meshSize[2];
}

size_t dsizeofdata(dump *frame)
{
    return frame->components * dsizeofdatacomponent(frame);
}

size_t dsizeofframe(dump *frame)
{
    return DUMPHEADERSIZE + dsizeofdata(frame) * sizeof(float); // excl CRC
}

size_t dfinilize(FILE *f, dump *frame)
{
    if (dsaveheader(f, frame))
        return 1;
    void *buff = (void *)calloc(CRCSIZE, 1);
    size_t success = fseek(f, dsizeofframe(frame), SEEK_SET);
    if (success != 0)
        return 1;
    success = fwrite(buff, 1, CRCSIZE, f);
    if (buff != NULL)
        free(buff);
    buff = NULL;
    if (success != CRCSIZE)
        return 1;
    return 0;
}

dump *dcloneheader(dump *src)
{
    dump *out = (dump *)malloc(DUMPHEADERSIZE);
    memcpy(out, src, DUMPHEADERSIZE);
    return out;
}

size_t dloadsinglefloat(FILE *f, size_t i, float **val)
{
    size_t success = dsettodata(f);
    if (success != 0)
        return 1;
    success = fseek(f, sizeof(float) * i, SEEK_CUR);
    if (success != 0)
        return 1;
    success = fread(*val, 1, sizeof(float), f);
    if (success != sizeof(float))
        return 1;
    return 0;
}

size_t dloadchunkfloat(FILE *f, size_t i, size_t size, float **buf)
{
    size_t success = dsettodata(f);
    if (success != 0)
        return 1;
    success = fseek(f, sizeof(float) * i, SEEK_CUR);
    if (success != 0)
        return 1;
    success = fread(*buf, 1, sizeof(float) * size, f);
    if (success != sizeof(float) * size)
        return 1;
    return 0;
}

size_t dsavesinglefloat(FILE *f, size_t i, float *val)
{
    size_t success = dsettodata(f);
    if (success != 0)
        return 1;
    success = fseek(f, sizeof(float) * i, SEEK_CUR);
    if (success != 0)
        return 1;
    success = fwrite((void *)val, 1, sizeof(float), f);
    if (success != sizeof(float))
        return 1;
    return 0;
}

size_t dsavechunkfloat(FILE *f, size_t i, size_t size, float *buf)
{
    size_t success = dsettodata(f);
    if (success != 0)
        return 1;
    success = fseek(f, sizeof(float) * i, SEEK_CUR);
    if (success != 0)
        return 1;
    success = fwrite((void *)buf, 1, sizeof(float) * size, f);
    if (success != sizeof(float) * size)
        return 1;
    return 0;
}
