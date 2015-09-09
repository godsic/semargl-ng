#include "core/common.h"
#include "core/preprocess.h"

static dump *frame;

static alignas(ALIGNETOCACHE) float *buffer;

static FILE *f;

void cleanup()
{
    sync();

    if (buffer != NULL)
        free((void *)buffer);
    if (frame != NULL)
        free((void *)frame);

    buffer = NULL;
    frame = NULL;
}

int main(int argc, char *argv[])
{
    size_t t_count = argc - 1;
    size_t c, bias = 0;
    size_t datasize;
    size_t datacompsize;
    char **filenames = &argv[1];
    size_t batches;
    size_t offset;
    size_t stride;

    if (atexit(cleanup))
        exit(EXIT_FAILURE);

    frame = (dump*)malloc(DUMPHEADERSIZE);
    
    setvbuf(stdout, NULL, _IONBF, 0);

    f = fopen((const char *)filenames[0], "r+b");
    if (f == NULL)
        exit(EXIT_FAILURE);
    if (dloadheader(f, &frame))
        exit(EXIT_FAILURE);
    fclose(f);

    datasize = t_count;

    datacompsize = dsizeofdatacomponent(frame);

    // Calculate batch size to avoid non-uniform loops (lazy)
    stride = dsizeofdata(frame);

    calculatebatch(datasize, stride, &batches, &offset);

    buffer = (float *)aligned_alloc(ALIGNETOCACHE, stride * offset * sizeof(float));

    memset(buffer, 0, stride * offset * sizeof(float));

    for (c = 0; c < batches; c++)
    {
        bias = offset * c;

        if (loadspatdatar(offset, bias, stride, &buffer, (const char **)filenames))
            exit(EXIT_FAILURE);

        avg(&buffer, frame->components, datacompsize, offset, bias);
    }

    return EXIT_SUCCESS;
}
