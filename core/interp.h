#pragma once

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "core/common.h"
#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>

void batch_linterp_continuous_axis(float** yy, double* x, size_t size, size_t stride, size_t count);

void linterp_continuous_axis(float** yy, double* x, size_t size, double dt);

void gsl_interp_continuous_axis(float** yy, double* x, size_t size, double dx, gsl_interp_accel **acc, gsl_spline **spline);

void batch_gsl_interp_continuous_axis(float** yy, double* x, size_t size, size_t stride, size_t count);
