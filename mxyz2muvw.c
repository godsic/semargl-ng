#include "core/common.h"

static dump *frame;

static alignas(ALIGNETOCACHE) float *buffer;
static alignas(ALIGNETOCACHE) float *chunk_buffer;

static FILE *f;

void cleanup()
{
    sync();

    if (buffer != NULL)
        free((void *)buffer);
    if (chunk_buffer != NULL)
        free((void *)chunk_buffer);
    if (frame != NULL)
        free((void *)frame);

    buffer = NULL;
    chunk_buffer = NULL;
    frame = NULL;
}

int main(int argc, char *argv[])
{
    size_t t_count = argc - 1;
    size_t c, bias = 0;
    size_t datasize;
    char **filenames = &argv[1];
    size_t batches;
    size_t offset;
    size_t stride;

    if (atexit(cleanup))
        exit(EXIT_FAILURE);

    setvbuf(stdout, NULL, _IONBF, 0);

    // probe one frame for parameters
    printf("DUMPHEADERSIZE: %zd\n", DUMPHEADERSIZE);

    printf("Probing:\t%s ...\n", argv[1]);
    f = fopen((const char *)filenames[0], "r+b");
    if (f == NULL)
        exit(EXIT_FAILURE);
    if (dloadheader(f, &frame))
        exit(EXIT_FAILURE);
    fclose(f);

    printf("components: %zd\n", frame->components);
    printf("sizex: %zd\n", frame->meshSize[0]);
    printf("sizey: %zd\n", frame->meshSize[1]);
    printf("sizez: %zd\n", frame->meshSize[2]);

    datasize = t_count;

    // Calculate batch size to avoid non-uniform loops (lazy)
    stride = dsizeofdata(frame);

    calculatebatch(datasize, stride, &batches, &offset);

    buffer = (float *)aligned_alloc(ALIGNETOCACHE, stride * offset * sizeof(float));
    memset(buffer, 0, stride * offset * sizeof(float));
    chunk_buffer = (float *)aligned_alloc(ALIGNETOCACHE, sizeof(float) * stride);


    printf("\n\e[1;33mSPANNING SWAP FILES\n");
    if (spawnfilesr2r(t_count, (const char **)filenames))
        exit(EXIT_FAILURE);

    for (c = 0; c < batches; c++)
    {
        printf("%zd (%zd)\n", c, batches - 1);
        bias = offset * c;

        printf("\e[0;34mLOAD\n");
        if (loadspatdatar(offset, bias, stride, &buffer, (const char **)filenames))
            exit(EXIT_FAILURE);

        printf("\n\e[1;33mSTORE\n");
        if (savespatdatar(offset, bias, stride, &buffer, (const char **)filenames))
            exit(EXIT_FAILURE);
    }

    // finalize the files
    printf("\n\e[0;32mFINALIZING\n");
    if (finalizefilesr2r(t_count, frame, (const char **)filenames))
        exit(EXIT_FAILURE);
    printf("\nDONE\n");

    return EXIT_SUCCESS;
}
