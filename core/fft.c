#include "core/fft.h"

int init_threaded_fftw()
{
    int err = fftwf_init_threads();
    fftwf_plan_with_nthreads(omp_get_max_threads());
    return err;
}
