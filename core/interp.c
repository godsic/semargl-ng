#include "core/interp.h"

void batch_linterp_continuous_axis(float** yy, double* x, size_t size, size_t stride, size_t count)
{
    float *y = *yy;
    float *ty;

    size_t i = 0;
    size_t bias = 0;

    double dt = ((double)x[size - 1] - (double)x[0]) / size;

    for (i = 0; i < count; i++) {
        bias = i * stride;
        ty = &y[bias];
        linterp_continuous_axis(&ty, x, size, dt);
    }

}

void batch_gsl_interp_continuous_axis(float** yy, double* x, size_t size, size_t stride, size_t count)
{
    float *y = *yy;
    float *ty;

    size_t i = 0;
    size_t bias = 0;

    double dt = ((double)x[size - 1] - (double)x[0]) / size;

    gsl_interp_accel *acc = gsl_interp_accel_alloc ();
    gsl_spline *spline = gsl_spline_alloc(gsl_interp_cspline, size);

    for (i = 0; i < count; i++) {
        bias = i * stride;
        ty = &y[bias];
        gsl_interp_continuous_axis(&ty, x, size, dt, &acc, &spline);
    }

    gsl_spline_free (spline);
    gsl_interp_accel_free(acc);

}


void linterp_continuous_axis(float** yy, double* x, size_t size, double dt)
{

    size_t di;
    size_t si = 0;
    double t;
    double t0 = x[0];
    double tt;

    float *y = *yy;
    float *ty = (float *)aligned_alloc(ALIGNETOCACHE, sizeof(float) * size);


    for (di = 0; di < size; di++) {

        t = t0 + dt * (double)di;
        while ((si < size - 1) && !(x[si] <= t && x[si + 1] > t && x[si] != x[si + 1])) {
            si++;
        }

        tt = (t - x[si]) / (x[si + 1] - x[si]);
        if (tt < 0.0 || tt > 1.0) {
            log("There is a bug in LINERP:ratio < 0.0 || ratio > 1.0! at %ld (%f)\n", si, t);
        }
        ty[di] = (float)((1.0 - tt) * (double)y[si] + tt * (double)y[si + 1]);
    }

    memcpy(y, ty, size * sizeof(float));

    free((void *)ty);

    ty = NULL;
}

void gsl_interp_continuous_axis(float** yy, double* x, size_t size, double dx, gsl_interp_accel **acc, gsl_spline **spline)
{

    size_t di;
    double xprime;
    double x0 = x[0];

    float *y = *yy;
    double *y64 = (double *)aligned_alloc(ALIGNETOCACHE, sizeof(double) * size);

    for (di = 0; di < size; di++) {
        y64[di] = (double)y[di];
    }

    gsl_spline_init((*spline), x, y64, size);

    for (di = 0; di < size; di++) {

        xprime = x0 + dx * (double)di;
        y[di] = (float)gsl_spline_eval((*spline), xprime, (*acc));
    }

    free((void *)y64);

    y64 = NULL;
}