#include "core/common.h"

static dump *frame;

static alignas(ALIGNETOCACHE) float *buffer;
static alignas(ALIGNETOCACHE) float *chunk_buffer;
static alignas(ALIGNETOCACHE) float *chunk_bufferx;
static alignas(ALIGNETOCACHE) float *chunk_buffery;
static alignas(ALIGNETOCACHE) fftwf_complex *buffer_complex;

static alignas(ALIGNETOCACHE) double *stamps;
static char stamp_unit[DUMPFIELDSIZE];
static char stamp_inv_unit[DUMPFIELDSIZE] = "Hz\0";

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
    if (stamps != NULL)
        free((void *)stamps);
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
    stamps = NULL;
    frame = NULL;
    plan = NULL;
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
    size_t stridecmpx;

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

    datasize = dsizeofdata(frame);

    // Calculate batch size to avoid non-uniform loops (lazy)
    stride = t_count + 2;
    stridecmpx = t_count / 2 + 1;

    calculatebatch(datasize, stride, &batches, &offset);

    buffer = (float *)aligned_alloc(ALIGNETOCACHE, stride * offset * sizeof(float)); // 2 goes for padding required by fftw r2c
    memset(buffer, 0, stride * offset * sizeof(float));
    buffer_complex = (fftwf_complex *)&buffer[0];
    chunk_buffer = (float *)aligned_alloc(ALIGNETOCACHE, sizeof(float) * offset);
    chunk_bufferx = (float *)aligned_alloc(ALIGNETOCACHE, sizeof(float) * offset);
    chunk_buffery = (float *)aligned_alloc(ALIGNETOCACHE, sizeof(float) * offset);

    log("in: %p\n", buffer);
    log("out: %p\n", buffer_complex);

    plan = fftwf_plan_many_dft_r2c(1, (const int *)&t_count, offset,
                                   buffer, NULL,
                                   1, stride,
                                   buffer_complex, NULL,
                                   1, stridecmpx,
                                   FFTW_ESTIMATE);
    if (plan == NULL)
        exit(EXIT_FAILURE);

    log("\n\e[1;33mSPANNING SWAP FILES\n");
    if (spawnfilesr2c(stridecmpx, (const char **)filenames))
        exit(EXIT_FAILURE);

    for (c = 0; c < batches; c++) {
        log("%zd (%zd)\n", c, batches - 1);
        bias = offset * c;

        log("\e[0;34mLOAD\n");
        if (loaddatar(t_count, stride, bias, offset, &buffer, &chunk_buffer, (const char **)filenames))
            exit(EXIT_FAILURE);

        log("\n\e[0;35mMAKING GRID UNIFORM\n");
        batch_gsl_interp_continuous_axis(&buffer, stamps, t_count, stride, offset);

        log("\n\e[0;35mAPPLYING WINDOW\n");
        applywindow(&buffer, t_count, stride, offset);

        log("\n\e[0;31mFFT\n");
        fftwf_execute(plan);

        log("\n\e[1;33mSTORE\n");
        if (savedatac(stridecmpx, stridecmpx, bias, offset, &buffer_complex, &chunk_bufferx, &chunk_buffery, MODEAP, (const char **)filenames))
            exit(EXIT_FAILURE);
    }

    get_inv_stamps(&stamps, t_count);

    // finalize the files
    log("\n\e[0;32mFINALIZING\n");
    if (finalizefilesr2c_savestamps(stridecmpx, frame, (const char **)filenames, stamps, stamp_inv_unit))
        exit(EXIT_FAILURE);
    log("\nDONE\n");

    return EXIT_SUCCESS;
}
