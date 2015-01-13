#include "core/common.h"

size_t getfreememphys()
{
    return (size_t)sysconf(_SC_PAGESIZE) * (size_t)sysconf(_SC_AVPHYS_PAGES);
}

size_t spawnfilesr2c(size_t count, const char **filenames)
{
    char *filenamex;
    char *filenamey;
    FILE *fx;
    FILE *fy;
    size_t i;

    for (i = 0; i < count; i++)
    {
        filenamex = (char *)calloc(strlen(filenames[i]) + strlen(XEXT) + 1, sizeof(char));
        filenamey = (char *)calloc(strlen(filenames[i]) + strlen(YEXT) + 1, sizeof(char));
        strcpy(filenamex, filenames[i]);
        strcpy(filenamey, filenames[i]);
        strcat(filenamex, XEXT);
        strcat(filenamey, YEXT);

        printf("%s\n", filenamex);
        printf("%s\n", filenamey);

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
    FILE *fx;
    FILE *fy;
    size_t i;

    for (i = 0; i < count; i++)
    {
        filenamexx = (char *)calloc(strlen(filenamesX[i]) + strlen(XEXT) + 1, sizeof(char));
        filenameyy = (char *)calloc(strlen(filenamesY[i]) + strlen(YEXT) + 1, sizeof(char));
        strcpy(filenamexx, filenamesX[i]);
        strcpy(filenameyy, filenamesY[i]);
        strcat(filenamexx, XEXT);
        strcat(filenameyy, YEXT);

        printf("%s\t%s\n", filenamexx, filenameyy);

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


size_t loaddatar(size_t count, size_t stride, size_t bias, size_t offset, float **gbuffer, float **lbuffer, const char **filenames)
{
    FILE *f;
    float *lbf = *lbuffer;
    float *gbf = *gbuffer;
    size_t i, k, ii;

    for (i = 0; i < count; i++)
    {
        printf("%s\n", filenames[i]);
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
        printf("%s\t%s\n", filenamesX[i], filenamesY[i]);

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

size_t savedatac(size_t count, size_t stride, size_t bias, size_t offset, fftwf_complex **gbuffer, float **lbufferx, float **lbuffery, size_t mode, const char **filenames)
{
    float x, y;
    double xx, yy;
    size_t i, k, ii;
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
        strcpy(filenamex, filenames[i]);
        strcpy(filenamey, filenames[i]);
        strcat(filenamex, XEXT);
        strcat(filenamey, YEXT);

        printf("%s\n", filenamex);
        printf("%s\n", filenamey);

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
                xx = (double)x;
                yy = (double)y;
                x = (float)(sqrt(xx * xx + yy * yy));
                y = (float)(atan2(yy, xx));
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

size_t finalizefilesr2c(size_t count, dump *header, const char **filenames)
{
    FILE *fx;
    FILE *fy;
    char *filenamex;
    char *filenamey;
    size_t i;

    for (i = 0; i < count; i++)
    {
        filenamex = (char *)calloc(strlen(filenames[i]) + strlen(XEXT) + 1, sizeof(char));
        filenamey = (char *)calloc(strlen(filenames[i]) + strlen(YEXT) + 1, sizeof(char));
        strcpy(filenamex, filenames[i]);
        strcpy(filenamey, filenames[i]);
        strcat(filenamex, XEXT);
        strcat(filenamey, YEXT);

        printf("%s\n", filenamex);
        printf("%s\n", filenamey);

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

    freememfloat = floor((double)getfreememphys() / (double)sizeof(float));

    (*offset) = floor((double)freememfloat / (double)stride); // the chunk cannot exceed this value
    while (datasize % (*offset) != 0)
    {
        (*offset)--;
    } 
    *batches = datasize / (*offset);

    printf("Availiable memory: %zd FLOATS\n", freememfloat);
    printf("datasize: %zd\tstride: %zd\t offset: %zd\t batches: %zd\n", datasize, stride, *offset, *batches);

    if ((*batches) * (*offset) != datasize)
        exit(EXIT_FAILURE);
}