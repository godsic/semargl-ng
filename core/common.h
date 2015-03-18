#pragma once

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include "core/dump.h"
#include "core/fft.h"

#define ALIGNETOCACHE 128

#define DATARANKCMPX sizeof(fftwf_complex) / sizeof(float)

#define XEXT ".X"
#define YEXT ".Y"

#define AEXT ".A"
#define PEXT ".P"

#define MODEXY 0
#define MODEAP 1

size_t getfreememphys();

size_t spawnfilesr2c(size_t count, const char **filenames);
size_t spawnfilesc2c(size_t count, const char **filenamesX, const char **filenamesY);

size_t loaddatar(size_t count, size_t stride, size_t bias, size_t offset, float **gbuffer, float **lbuffer, const char **filenames);
size_t loaddatac(size_t count, size_t stride, size_t bias, size_t size, size_t offset, fftwf_complex **gbuffer, float **lbufferx, float **lbuffery, size_t mode, const char **filenamesX, const char **filenamesY);

size_t savedatac(size_t count, size_t stride, size_t bias, size_t offset, fftwf_complex **gbuffer, float **lbufferx, float **lbuffery, size_t mode, const char **filenames);

size_t finalizefilesr2c(size_t count, dump *header, const char **filenames);

void calculatebatch(size_t datasize, size_t stride, size_t *batches, size_t *offset);

void removegroundstate(float **in, size_t count, size_t offset, size_t n);

// regular Hanning windows
void applywindow(float **in, size_t size, size_t offset, size_t n);