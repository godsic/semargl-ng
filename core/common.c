#include "core/common.h"


size_t get_inv_stamps(double** stamps, size_t size) {
    size_t i;
    double T = (*stamps)[size-1] - (*stamps)[0];
    double df = 1.0 / T;

    log("Frequency resolution: %f MHz\n", df * 1.0e-6);
    log("Fmax: %f MHz\n", df * (double)(size/2));

    for(i=0; i < size/2 + 1; i++) {
        (*stamps)[i] = (double)i * df;
    }

    for(i=size/2 + 1; i < size; i++) {
        (*stamps)[i] = -(double)(size-i) * df;
    }

    return EXIT_SUCCESS;
}


size_t get_time_from_files(const char **filenames, double** xx, dump** fframe, size_t count) {

    size_t i;

    FILE *f;

    dump *frame = *fframe;

    (*xx) = (double *)aligned_alloc(ALIGNETOCACHE, sizeof(double) * count);

    for (i = 0; i < count; i++)
    {
        f = fopen((const char *)filenames[i], "rb");
        if (f == NULL)
            return EXIT_FAILURE;

        if (dloadheader(f, &frame))
            return EXIT_FAILURE;

        (*xx) [i] = frame->time;

        if (fclose(f))
            return EXIT_FAILURE;
        f = NULL;

        log("%s -> %g %s\n", filenames[i], (*xx) [i], frame->timeUnit);

    }
    return EXIT_SUCCESS;
}


/* This code is partly taken from Linux/fs/proc/meminfo.c */

size_t getfreememphys()
{
    size_t MAXCBUF = 1024;
    char *cbuffer = malloc(MAXCBUF * sizeof(char));
    char *key;
    int separator = ':';
    size_t memavailable = 0;
    size_t memunit = 0;

    FILE *f;
    f = fopen("/proc/meminfo", "r");
    if (f == NULL) {
        perror("fopen");
    }

    while (!feof(f)) {
        memset(cbuffer, '\0', MAXCBUF);
        fgets(cbuffer, MAXCBUF, f);
        if (strstr(cbuffer, "MemAvailable") != NULL) {
            key = strchr(cbuffer, separator);
            memavailable = strtoull(key + 1, NULL, 10);
            if (strstr(cbuffer, "kB") != NULL) {
                memunit = 1024;
            }
            break;
        }
    }

    fclose(f);
    free((void*)cbuffer);
    cbuffer = NULL;

    return memavailable * memunit;
}

size_t spawnfilesr2r(size_t count, const char **filenames)
{
    char *filename;
    size_t extpos = 0;

    FILE *f;
    size_t i;

    for (i = 0; i < count; i++)
    {
        filename = (char *)calloc(strlen(filenames[i]) + strlen(UVWEXT) + 1, sizeof(char));

        extpos = strlen(filenames[i]) - strlen(strstr(filenames[i], DUMPEXT));

        strncpy(filename, filenames[i], extpos);
        strcat(filename, UVWEXT);
        strcat(filename, DUMPEXT);

        log("%s\n", filename);

        f = fopen((const char *)filename, "w");
        if (f == NULL)
            return EXIT_FAILURE;

        if (fclose(f))
            return EXIT_FAILURE;

        free((void *)filename);
        f = NULL;
        filename = NULL;
    }
    return EXIT_SUCCESS;

}

size_t spawnfilesr2c(size_t count, const char **filenames)
{
    char *filenamex;
    char *filenamey;
    size_t extpos = 0;

    FILE *fx;
    FILE *fy;
    size_t i;

    for (i = 0; i < count; i++)
    {
        filenamex = (char *)calloc(strlen(filenames[i]) + strlen(XEXT) + 1, sizeof(char));
        filenamey = (char *)calloc(strlen(filenames[i]) + strlen(YEXT) + 1, sizeof(char));

        extpos = strlen(filenames[i]) - strlen(strstr(filenames[i], DUMPEXT));

        strncpy(filenamex, filenames[i], extpos);
        strncpy(filenamey, filenames[i], extpos);

        strcat(filenamex, XEXT);
        strcat(filenamey, YEXT);
        strcat(filenamex, DUMPEXT);
        strcat(filenamey, DUMPEXT);

        log("%s\n", filenamex);
        log("%s\n", filenamey);

        fx = fopen((const char *)filenamex, "w");
        if (fx == NULL)
            return EXIT_FAILURE;
        fy = fopen((const char *)filenamey, "w");
        if (fy == NULL)
            return EXIT_FAILURE;

        if (fclose(fx))
            return EXIT_FAILURE;
        if (fclose(fy))
            return EXIT_FAILURE;

        free((void *)filenamex);
        free((void *)filenamey);
        fx = NULL;
        fy = NULL;
        filenamex = NULL;
        filenamey = NULL;
    }
    return EXIT_SUCCESS;
}

size_t spawnfilesc2c(size_t count, const char **filenamesX, const char **filenamesY)
{
    char *filenamexx;
    char *filenameyy;

    size_t extposX = 0;
    size_t extposY = 0;

    FILE *fx;
    FILE *fy;

    size_t i;

    for (i = 0; i < count; i++)
    {
        filenamexx = (char *)calloc(strlen(filenamesX[i]) + strlen(XEXT) + 1, sizeof(char));
        filenameyy = (char *)calloc(strlen(filenamesY[i]) + strlen(YEXT) + 1, sizeof(char));

        extposX = strlen(filenamesX[i]) - strlen(strstr(filenamesX[i], DUMPEXT));
        extposY = strlen(filenamesY[i]) - strlen(strstr(filenamesY[i], DUMPEXT));

        strncpy(filenamexx, filenamesX[i], extposX);
        strncpy(filenameyy, filenamesY[i], extposY);

        strcat(filenamexx, XEXT);
        strcat(filenameyy, YEXT);
        strcat(filenamexx, DUMPEXT);
        strcat(filenameyy, DUMPEXT);

        log("%s\t%s\n", filenamexx, filenameyy);

        fx = fopen((const char *)filenamexx, "w");
        if (fx == NULL)
            return EXIT_FAILURE;
        fy = fopen((const char *)filenameyy, "w");
        if (fy == NULL)
            return EXIT_FAILURE;

        if (fclose(fx))
            return EXIT_FAILURE;
        if (fclose(fy))
            return EXIT_FAILURE;

        free((void *)filenamexx);
        free((void *)filenameyy);
        fx = NULL;
        fy = NULL;
        filenamexx = NULL;
        filenameyy = NULL;
    }
    return EXIT_SUCCESS;
}


size_t loadspatdatar(size_t count, size_t bias, size_t offset, float **gbuffer, const char **filenames)
{
    FILE *f;
    float *gbf = *gbuffer;
    float *gbf_offset;
    size_t i, k, ii;

    for (i = 0; i < count; i++)
    {
        ii = bias + i;
        k = i * offset;
        gbf_offset = &gbf[k];

        log("%s\n", filenames[ii]);

        f = fopen((const char *)filenames[ii], "rb");
        if (f == NULL)
            return EXIT_FAILURE;

        setvbuf(f, NULL, _IONBF, 0);

        if (dloadchunkfloat(f, 0, offset, &gbf_offset))
            return EXIT_FAILURE;

        if (fclose(f))
            return EXIT_FAILURE;
        f = NULL;
    }
    return EXIT_SUCCESS;
}

size_t loaddatar(size_t count, size_t stride, size_t bias, size_t offset, float **gbuffer, float **lbuffer, const char **filenames)
{
    FILE *f;
    float *lbf = *lbuffer;
    float *gbf = *gbuffer;
    size_t i, k, ii;

    for (i = 0; i < count; i++)
    {
        log("%s\n", filenames[i]);
        f = fopen((const char *)filenames[i], "rb");
        if (f == NULL)
            return EXIT_FAILURE;
        setvbuf(f, NULL, _IONBF, 0);
        if (dloadchunkfloat(f, bias, offset, &lbf))
            return EXIT_FAILURE;
        for (k = 0; k < offset; k++)
        {
            ii = k * stride;
            gbf[ii + i] = lbf[k];
        }
        if (fclose(f))
            return EXIT_FAILURE;
        f = NULL;
    }
    return EXIT_SUCCESS;
}

size_t loaddatac(size_t count, size_t stride, size_t bias, size_t size, size_t offset, fftwf_complex **gbuffer, float **lbufferx, float **lbuffery, size_t mode, const char **filenamesX, const char **filenamesY)
{
    FILE *fx;
    FILE *fy;
    float *lbfx = *lbufferx;
    float *lbfy = *lbuffery;
    fftwf_complex *gbf = *gbuffer;
    size_t i, k, ii;
    float x, y;
    double xx, yy;

    for (i = 0; i < count; i++)
    {
        log("%s\t%s\n", filenamesX[i], filenamesY[i]);

        fx = fopen((const char *)filenamesX[i], "rb");
        if (fx == NULL)
            return EXIT_FAILURE;
        fy = fopen((const char *)filenamesY[i], "rb");
        if (fy == NULL)
            return EXIT_FAILURE;

        setvbuf(fx, NULL, _IONBF, 0);
        setvbuf(fy, NULL, _IONBF, 0);

        if (dloadchunkfloat(fx, bias, size, &lbfx))
            return EXIT_FAILURE;
        if (dloadchunkfloat(fy, bias, size, &lbfy))
            return EXIT_FAILURE;

        for (k = 0; k < offset; k++)
        {
            ii = k * stride;
            x = lbfx[k];
            y = lbfy[k];
            switch (mode)
            {
            case MODEAP:
                xx = (double)x;
                yy = (double)y;
                x = (float)(xx * cos(yy));
                y = (float)(xx * sin(yy));
                break;
            case MODEXY:
                break;
            default:
                break;
            }

            gbf[ii + i][0] = x;
            gbf[ii + i][1] = y;
        }

        if (fclose(fx))
            return EXIT_FAILURE;
        if (fclose(fy))
            return EXIT_FAILURE;

        fx = NULL;
        fy = NULL;
    }
    return EXIT_SUCCESS;
}


size_t savespatdatar(size_t count, size_t bias, size_t offset, float **gbuffer, const char **filenames)
{
    size_t i, k, ii;
    size_t extpos = 0;

    FILE *f;
    char *filename;
    float *gbf = *gbuffer;
    float *gbf_offset;

    for (i = 0; i < count; i++)
    {
        ii = bias + i;
        k = i * offset;
        gbf_offset = &gbf[k];

        filename = (char *)calloc(strlen(filenames[ii]) + strlen(UVWEXT) + 1, sizeof(char));

        extpos = strlen(filenames[ii]) - strlen(strstr(filenames[ii], DUMPEXT));

        strncpy(filename, filenames[ii], extpos);

        strcat(filename, UVWEXT);
        strcat(filename, DUMPEXT);

        log("%s\n", filename);

        f = fopen((const char *)filename, "r+");
        if (f == NULL)
            return EXIT_FAILURE;

        setvbuf(f, NULL, _IONBF, 0);

        if (dsavechunkfloat(f, 0, offset, gbf_offset))
            return EXIT_FAILURE;

        if (fclose(f))
            return EXIT_FAILURE;

        free((void *)filename);
        f = NULL;
        filename = NULL;
    }
    return EXIT_SUCCESS;
}

size_t savedatar(size_t count, size_t stride, size_t bias, size_t offset, float **gbuffer, float **lbuffer, size_t mode, const char **filenames)
{
    size_t i, k, ii;
    size_t extpos = 0;

    FILE *f;
    char *filename;
    float *lbf = *lbuffer;
    float *gbf = *gbuffer;

    for (i = 0; i < count; i++)
    {
        filename = (char *)calloc(strlen(filenames[i]) + strlen(UVWEXT) + 1, sizeof(char));

        extpos = strlen(filenames[i]) - strlen(strstr(filenames[i], DUMPEXT));

        strncpy(filename, filenames[i], extpos);

        strcat(filename, UVWEXT);
        strcat(filename, DUMPEXT);

        log("%s\n", filename);

        f = fopen((const char *)filename, "r+");
        if (f == NULL)
            return EXIT_FAILURE;

        setvbuf(f, NULL, _IONBF, 0);

        for (k = 0; k < offset; k++)
        {
            ii = k * stride;
            lbf[k] = gbf[ii + i];
        }

        if (dsavechunkfloat(f, bias, offset, lbf))
            return EXIT_FAILURE;

        if (fclose(f))
            return EXIT_FAILURE;

        free((void *)filename);
        f = NULL;
        filename = NULL;
    }
    return EXIT_SUCCESS;
}

size_t savedatac(size_t count, size_t stride, size_t bias, size_t offset, fftwf_complex **gbuffer, float **lbufferx, float **lbuffery, size_t mode, const char **filenames)
{
    float x, y;
    float xx, yy;
    size_t i, k, ii;
    size_t extpos = 0;

    FILE *fx;
    FILE *fy;
    char *filenamex;
    char *filenamey;
    float *lbfx = *lbufferx;
    float *lbfy = *lbuffery;
    fftwf_complex *gbf = *gbuffer;

    for (i = 0; i < count; i++)
    {
        filenamex = (char *)calloc(strlen(filenames[i]) + strlen(XEXT) + 1, sizeof(char));
        filenamey = (char *)calloc(strlen(filenames[i]) + strlen(YEXT) + 1, sizeof(char));

        extpos = strlen(filenames[i]) - strlen(strstr(filenames[i], DUMPEXT));

        strncpy(filenamex, filenames[i], extpos);
        strncpy(filenamey, filenames[i], extpos);

        strcat(filenamex, XEXT);
        strcat(filenamey, YEXT);
        strcat(filenamex, DUMPEXT);
        strcat(filenamey, DUMPEXT);

        log("%s\n", filenamex);
        log("%s\n", filenamey);

        fx = fopen((const char *)filenamex, "r+");
        if (fx == NULL)
            return EXIT_FAILURE;
        fy = fopen((const char *)filenamey, "r+");
        if (fy == NULL)
            return EXIT_FAILURE;

        setvbuf(fx, NULL, _IONBF, 0);
        setvbuf(fy, NULL, _IONBF, 0);

        for (k = 0; k < offset; k++)
        {
            ii = k * stride;

            x = gbf[ii + i][0];
            y = gbf[ii + i][1];
            switch (mode)
            {
            case MODEAP:
                xx = x;
                yy = y;
                x = sqrtf(xx * xx + yy * yy);
                y = atan2f(yy, xx);
                break;
            case MODEXY:
                break;
            default:
                break;
            }
            lbfx[k] = x;
            lbfy[k] = y;
        }

        if (dsavechunkfloat(fx, bias, offset, lbfx))
            return EXIT_FAILURE;
        if (dsavechunkfloat(fy, bias, offset, lbfy))
            return EXIT_FAILURE;

        if (fclose(fx))
            return EXIT_FAILURE;
        if (fclose(fy))
            return EXIT_FAILURE;
        free((void *)filenamex);
        free((void *)filenamey);
        fx = NULL;
        fy = NULL;
        filenamex = NULL;
        filenamey = NULL;
    }
    return EXIT_SUCCESS;
}

size_t finalizefilesr2r(size_t count, dump *header, const char **filenames)
{
    FILE *f;
    char *filename;
    size_t i;
    size_t extpos = 0;

    for (i = 0; i < count; i++)
    {
        filename = (char *)calloc(strlen(filenames[i]) + strlen(UVWEXT) + 1, sizeof(char));

        extpos = strlen(filenames[i]) - strlen(strstr(filenames[i], DUMPEXT));

        strncpy(filename, filenames[i], extpos);

        strcat(filename, UVWEXT);
        strcat(filename, DUMPEXT);

        log("%s\n", filename);

        f = fopen((const char *)filename, "r+b");
        if (f == NULL)
            return EXIT_FAILURE;

        setvbuf(f, NULL, _IONBF, 0);

        if (dfinilize(f, header))
            return EXIT_FAILURE;

        if (fclose(f))
            return EXIT_FAILURE;

        free((void *)filename);
        f = NULL;
        filename = NULL;
    }
    return EXIT_SUCCESS;
}


size_t finalizefilesr2c(size_t count, dump *header, const char **filenames)
{
    FILE *fx;
    FILE *fy;
    char *filenamex;
    char *filenamey;
    size_t i;
    size_t extpos = 0;

    for (i = 0; i < count; i++)
    {
        filenamex = (char *)calloc(strlen(filenames[i]) + strlen(XEXT) + 1, sizeof(char));
        filenamey = (char *)calloc(strlen(filenames[i]) + strlen(YEXT) + 1, sizeof(char));

        extpos = strlen(filenames[i]) - strlen(strstr(filenames[i], DUMPEXT));

        strncpy(filenamex, filenames[i], extpos);
        strncpy(filenamey, filenames[i], extpos);

        strcat(filenamex, XEXT);
        strcat(filenamey, YEXT);
        strcat(filenamex, DUMPEXT);
        strcat(filenamey, DUMPEXT);

        log("%s\n", filenamex);
        log("%s\n", filenamey);

        fx = fopen((const char *)filenamex, "r+b");
        if (fx == NULL)
            return EXIT_FAILURE;
        fy = fopen((const char *)filenamey, "r+b");
        if (fy == NULL)
            return EXIT_FAILURE;
        setvbuf(fx, NULL, _IONBF, 0);
        setvbuf(fy, NULL, _IONBF, 0);

        if (dfinilize(fx, header))
            return EXIT_FAILURE;
        if (dfinilize(fy, header))
            return EXIT_FAILURE;

        if (fclose(fx))
            return EXIT_FAILURE;
        if (fclose(fy))
            return EXIT_FAILURE;
        free((void *)filenamex);
        free((void *)filenamey);
        fx = NULL;
        fy = NULL;
        filenamex = NULL;
        filenamey = NULL;
    }
    return EXIT_SUCCESS;
}

size_t finalizefilesr2r_savestamps(size_t count, dump *header, const char **filenames, double* stamps, const char stamp_unit[8])
{
    FILE *f;
    char *filename;
    size_t i;
    size_t extpos = 0;

    memcpy(header->timeUnit, stamp_unit, DUMPFIELDSIZE);

    for (i = 0; i < count; i++)
    {
        header->time = stamps[i];

        filename = (char *)calloc(strlen(filenames[i]) + strlen(UVWEXT) + 1, sizeof(char));

        extpos = strlen(filenames[i]) - strlen(strstr(filenames[i], DUMPEXT));

        strncpy(filename, filenames[i], extpos);

        strcat(filename, UVWEXT);
        strcat(filename, DUMPEXT);

        log("%s\n", filename);

        f = fopen((const char *)filename, "r+b");
        if (f == NULL)
            return EXIT_FAILURE;

        setvbuf(f, NULL, _IONBF, 0);

        if (dfinilize(f, header))
            return EXIT_FAILURE;

        if (fclose(f))
            return EXIT_FAILURE;

        free((void *)filename);
        f = NULL;
        filename = NULL;
    }
    return EXIT_SUCCESS;
}

size_t finalizefilesr2c_savestamps(size_t count, dump *header, const char **filenames, double* stamps, const char stamp_unit[8])
{
    FILE *fx;
    FILE *fy;
    char *filenamex;
    char *filenamey;
    size_t i;
    size_t extpos = 0;

    memcpy(header->timeUnit, stamp_unit, DUMPFIELDSIZE);

    for (i = 0; i < count; i++)
    {

        header->time = stamps[i];

        filenamex = (char *)calloc(strlen(filenames[i]) + strlen(XEXT) + 1, sizeof(char));
        filenamey = (char *)calloc(strlen(filenames[i]) + strlen(YEXT) + 1, sizeof(char));

        extpos = strlen(filenames[i]) - strlen(strstr(filenames[i], DUMPEXT));

        strncpy(filenamex, filenames[i], extpos);
        strncpy(filenamey, filenames[i], extpos);

        strcat(filenamex, XEXT);
        strcat(filenamey, YEXT);
        strcat(filenamex, DUMPEXT);
        strcat(filenamey, DUMPEXT);

        log("%s\n", filenamex);
        log("%s\n", filenamey);

        fx = fopen((const char *)filenamex, "r+b");
        if (fx == NULL)
            return EXIT_FAILURE;
        fy = fopen((const char *)filenamey, "r+b");
        if (fy == NULL)
            return EXIT_FAILURE;
        setvbuf(fx, NULL, _IONBF, 0);
        setvbuf(fy, NULL, _IONBF, 0);

        if (dfinilize(fx, header))
            return EXIT_FAILURE;
        if (dfinilize(fy, header))
            return EXIT_FAILURE;

        if (fclose(fx))
            return EXIT_FAILURE;
        if (fclose(fy))
            return EXIT_FAILURE;
        free((void *)filenamex);
        free((void *)filenamey);
        fx = NULL;
        fy = NULL;
        filenamex = NULL;
        filenamey = NULL;
    }
    return EXIT_SUCCESS;
}

void calculatebatch(size_t datasize, size_t stride, size_t *batches, size_t *offset)
{
    size_t freememfloat;

    freememfloat = floor(0.6 * (double)getfreememphys() / (double)sizeof(float));

    (*offset) = floor((double)freememfloat / (double)stride); // the chunk cannot exceed this value
    while (datasize % (*offset) != 0)
    {
        (*offset)--;
    }
    *batches = datasize / (*offset);

    log("Availiable memory: %zd FLOATS\n", freememfloat);
    log("datasize: %zd\tstride: %zd\t offset: %zd\t batches: %zd\n", datasize, stride, *offset, *batches);

    if ((*batches) * (*offset) != datasize)
        exit(EXIT_FAILURE);
}

void removegroundstate(float **in, size_t size, size_t offset, size_t n)
{
    float *out = *in;
    float y0;
    int i, k, bias;
    for (k = 0; k < n; k++)
    {
        bias = k * offset;
        y0 = out[bias];
        for (i = 0; i < size; i++)
        {
            out[bias + i] = out[bias + i] - y0;
        }
    }
}

// Lanczos window
void applywindow(float **in, size_t size, size_t offset, size_t n)
{
    size_t i, k, bias;
    double arg;
    double v;
    log("size: %ld\toffset: %ld\tn: %ld\n", size, offset, n);
    for (k = 0; k < n; k++)
    {
        bias = k * offset;
        for (i = 0; i < size; i++)
        {
            arg = 2.0 * (double)i / (double)(size - 1);
            v = sinc(arg - 1.0);
            (*in)[bias + i] = (float)((double)(*in)[bias + i] * v);
        }
    }
}

inline float dot(float x1, float y1, float z1, float x2, float y2, float z2) {
    return x1 * x2 + y1 * y2 + z1 * z2;
}

inline float norm(float x1, float y1, float z1) {
    return sqrtf(dot(x1, y1, z1, x1, y1, z1));
}

inline double sinc(double x) {
    double v;
    double arg = M_PI * x;
    v = (x == 0.0) ? 1.0 : sin(arg) / arg;
    return v;
}