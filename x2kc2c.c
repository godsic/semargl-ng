#include "core/common.h"

static dump *frame;

static alignas(ALIGNETOCACHE) float *buffer;
static alignas(ALIGNETOCACHE) float *chunk_buffer;
static alignas(ALIGNETOCACHE) float *chunk_bufferx;
static alignas(ALIGNETOCACHE) float *chunk_buffery;
static alignas(ALIGNETOCACHE) fftwf_complex *buffer_complex;

static fftwf_plan plan;

static FILE *f;

void cleanup()
{
    sync();

    if (buffer != NULL)
        free((void *)buffer);
    if (chunk_buffer != NULL)
        free((void *)chunk_buffer);
    if (chunk_bufferx != NULL)
        free((void *)chunk_bufferx);
    if (chunk_buffery != NULL)
        free((void *)chunk_buffery);
    if (frame != NULL)
        free((void *)frame);
    if (plan != NULL)
        fftwf_destroy_plan(plan);

    fftwf_cleanup();

    buffer = NULL;
    buffer_complex = NULL;
    chunk_buffer = NULL;
    chunk_bufferx = NULL;
    chunk_buffery = NULL;
    frame = NULL;
    plan = NULL;
}

int main(int argc, char *argv[])
{
    size_t t_count;
    size_t c, bias;
    size_t datasize;
    char **filenamesX;
    char **filenamesY;
    size_t batches;
    size_t offset;
    size_t stride;
    size_t stridecmpx;
    int n[3];

    if (atexit(cleanup))
        exit(EXIT_FAILURE);

    t_count = (argc - 1) / 2;

    if ((argc - 1) % 2)
        exit(EXIT_FAILURE);

    filenamesX = &argv[1];
    filenamesY = &argv[1 + t_count];

    setvbuf(stdout, NULL, _IONBF, 0);

    // probe one frame for parameters
    log("DUMPHEADERSIZE: %zd\n", DUMPHEADERSIZE);

    log("Probing:\t%s ...\n", filenamesX[0]);
    f = fopen((const char *)filenamesX[0], "r+b");
    if (f == NULL)
        exit(EXIT_FAILURE);
    if (dloadheader(f, &frame))
        exit(EXIT_FAILURE);
    fclose(f);

    log("components: %zd\n", frame->components);
    log("sizex: %zd\n", frame->meshSize[0]);
    log("sizey: %zd\n", frame->meshSize[1]);
    log("sizez: %zd\n", frame->meshSize[2]);

    n[0] = frame->meshSize[0];
    n[1] = frame->meshSize[1];
    n[2] = frame->meshSize[2];

    stridecmpx = dsizeofdata(frame);
    stride = stridecmpx * DATARANKCMPX;

    // Calculate batch size to avoid non-uniform loops (lazy)
    datasize = t_count;

    calculatebatch(datasize, stride, &batches, &offset);

    buffer = (float *)aligned_alloc(ALIGNETOCACHE, stridecmpx * offset * sizeof(fftwf_complex));
    memset(buffer, 0, stridecmpx * offset * sizeof(fftwf_complex));
    buffer_complex = (fftwf_complex *)buffer;

    chunk_bufferx = (float *)aligned_alloc(ALIGNETOCACHE, sizeof(float) * stridecmpx);
    chunk_buffery = (float *)aligned_alloc(ALIGNETOCACHE, sizeof(float) * stridecmpx);

    plan = fftwf_plan_many_dft(3, (const int *)n, offset * frame->components,
                               buffer_complex, NULL,
                               1, stridecmpx,
                               buffer_complex, NULL,
                               1, stridecmpx,
                               FFTW_FORWARD, FFTW_MEASURE);
    if (plan == NULL)
        exit(EXIT_FAILURE);

    log("\n\e[1;33mSPANNING SWAP FILES\n");
    if (spawnfilesc2c(t_count, (const char **)filenamesX, (const char **)filenamesY))
        exit(EXIT_FAILURE);

    for (c = 0; c < batches; c++)
    {
        log("%zd (%zd)\n", c, batches - 1);
        bias = offset * c;

        log("\e[0;34mLOAD\n");
        if (loaddatac(offset, stridecmpx, 0, stridecmpx, offset, &buffer_complex, &chunk_bufferx, &chunk_buffery, MODEXY, (const char **)&filenamesX[bias], (const char **)&filenamesY[bias]))
            exit(EXIT_FAILURE);

        //     log("\n\e[0;31mFFT\n");
        //     fftwf_execute(plan);

        //     log("\n\e[1;33mSTORE\n");
        //     if (savedatac(stridecmpx, stridecmpx, bias, offset, &buffer_complex, &chunk_bufferx, &chunk_buffery, MODEAP, (const char **)filenames))
        //         exit(EXIT_FAILURE);

        //     // finalize the files
        //     log("\n\e[0;32mFINALIZING\n");
        //     if (finalizefilesr2c(stridecmpx, frame, (const char **)filenames))
        //         exit(EXIT_FAILURE);
        //     log("\nDONE\n");

    }

    // /*

    // stride = 2 * datasize; // the data is compex now
    // maxoffset = floor((double)freememfloat / (double)stride); // the chunk cannot exceed
    // batches = ceil((double)t_count / (double)maxoffset);
    // offset = t_count / batches;

    // */

    return EXIT_SUCCESS;
}
