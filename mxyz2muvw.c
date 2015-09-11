#include "core/common.h"
#include "core/preprocess.h"

static dump *frame;

static alignas(ALIGNETOCACHE) float *buffer;
static alignas(ALIGNETOCACHE) float *m0;

static alignas(ALIGNETOCACHE) double *stamps;

static char stamp_unit[DUMPFIELDSIZE];

static FILE *f;

void cleanup()
{
    sync();

    if (buffer != NULL)
        free((void *)buffer);
    if (m0 != NULL)
        free((void *)m0);
    if (frame != NULL)
        free((void *)frame);
    if (stamps != NULL)
        free((void *)stamps);

    buffer = NULL;
    m0 = NULL;
    frame = NULL;
    stamps = NULL;
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

    // probe one frame for parameters
    log("DUMPHEADERSIZE: %zd\n", DUMPHEADERSIZE);

    log("Probing:\t%s ...\n", argv[1]);
    f = fopen((const char *)filenames[0], "r+b");
    if (f == NULL)
        exit(EXIT_FAILURE);
    if (dloadheader(f, &frame))
        exit(EXIT_FAILURE);
    fclose(f);

    memset(stamp_unit, '\0', DUMPFIELDSIZE);
    memcpy(stamp_unit, frame->timeUnit, DUMPFIELDSIZE);

    log("components: %zd\n", frame->components);
    log("sizex: %zd\n", frame->meshSize[0]);
    log("sizey: %zd\n", frame->meshSize[1]);
    log("sizez: %zd\n", frame->meshSize[2]);

    log("Getting stamps...\n");
    get_stamps_from_files((const char **)filenames, &stamps, &frame, t_count);

    datasize = t_count;

    datacompsize = dsizeofdatacomponent(frame);

    // Calculate batch size to avoid non-uniform loops (lazy)
    stride = dsizeofdata(frame);

    calculatebatch(datasize, stride, &batches, &offset);

    buffer = (float *)aligned_alloc(ALIGNETOCACHE, stride * offset * sizeof(float));
    m0 = (float *)aligned_alloc(ALIGNETOCACHE, stride * sizeof(float));

    memset(buffer, 0, stride * offset * sizeof(float));
    memset(m0, 0, stride * sizeof(float));


    log("\n\e[1;33mLOAD GROUND STATE\n");
    if (loadspatdatar(1, 0, stride, &m0, (const char **)filenames))
        exit(EXIT_FAILURE);

    log("\n\e[1;33mSPANNING SWAP FILES\n");
    if (spawnfilesr2r(t_count, (const char **)filenames))
        exit(EXIT_FAILURE);

    for (c = 0; c < batches; c++)
    {
        log("%zd (%zd)\n", c, batches - 1);
        bias = offset * c;

        log("\e[0;34mLOAD\n");
        if (loadspatdatar(offset, bias, stride, &buffer, (const char **)filenames))
            exit(EXIT_FAILURE);

        log("\n\e[0;31mPRE-PROCESSING\n");
        mxyz2muvw(&buffer, m0, datacompsize, offset);

        log("\n\e[1;33mSTORE\n");
        if (savespatdatar(offset, bias, stride, &buffer, (const char **)filenames))
            exit(EXIT_FAILURE);
    }

    // finalize the files
    log("\n\e[0;32mFINALIZING\n");

    if (finalizefilesr2r_savestamps(t_count, frame, (const char **)filenames, stamps, stamp_unit))
        exit(EXIT_FAILURE);
    log("\nDONE\n");

    return EXIT_SUCCESS;
}
