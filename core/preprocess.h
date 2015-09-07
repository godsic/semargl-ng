#pragma once

#include "common.h"

void avg(float **data, size_t comp, size_t comp_size, size_t number_of_frames, size_t frame_bias);

void mxyz2muvw(float **data, float *m0, size_t comp_size, size_t number_of_frames);