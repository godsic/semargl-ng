#pragma once

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include "core/dump.h"
#include "core/fft.h"
#include "core/interp.h"

#define ALIGNETOCACHE 128

#define DATARANKCMPX sizeof(fftwf_complex) / sizeof(float)

#define VERBOSE 1

#define XEXT ".X"
#define YEXT ".Y"

#define AEXT ".A"
#define PEXT ".P"

#define MODEXY 0
#define MODEAP 1

#define UVWEXT ".UVW"

#define log(fmt, ...) \
		do { if (VERBOSE) fprintf(stderr, fmt, ##__VA_ARGS__); } while (0)

size_t get_inv_stamps(double** stamps, size_t size);

size_t getfreememphys();

size_t read_header(const char *filename, dump** fframe);

size_t get_stamps_from_files(const char **filenames, double** xx, dump** fframe, size_t count);

size_t spawnfilesr2r(size_t count, const char **filenames);
size_t spawnfilesr2c(size_t count, const char **filenames);
size_t spawnfilesc2c(size_t count, const char **filenamesX, const char **filenamesY);

size_t loadspatdatar(size_t count, size_t bias, size_t offset, float **gbuffer, const char **filenames);
size_t loaddatar(size_t count, size_t stride, size_t bias, size_t offset, float **gbuffer, float **lbuffer, const char **filenames);
size_t loaddatac(size_t count, size_t stride, size_t bias, size_t size, size_t offset, fftwf_complex **gbuffer, float **lbufferx, float **lbuffery, size_t mode, const char **filenamesX, const char **filenamesY);

size_t savespatdatar(size_t count, size_t bias, size_t offset, float **gbuffer, const char **filenames);
size_t savedatar(size_t count, size_t stride, size_t bias, size_t offset, float **gbuffer, float **lbuffer, size_t mode, const char **filenames);
size_t savedatac(size_t count, size_t stride, size_t bias, size_t offset, fftwf_complex **gbuffer, float **lbufferx, float **lbuffery, size_t mode, const char **filenames);

size_t finalizefilesr2r(size_t count, dump *header, const char **filenames);
size_t finalizefilesr2c(size_t count, dump *header, const char **filenames);

size_t finalizefilesr2r_savestamps(size_t count, dump *header, const char **filenames, double* stamps, const char stamp_unit[8]);
size_t finalizefilesr2c_savestamps(size_t count, dump *header, const char **filenames, double* stamps, const char stamp_unit[8]);

void calculatebatch(size_t datasize, size_t stride, size_t *batches, size_t *offset);

void removegroundstate(float **in, size_t count, size_t offset, size_t n);

void applywindow(float **in, size_t size, size_t offset, size_t n);

float dot(float x1, float y1, float z1, float x2, float y2, float z2);

float norm(float x1, float y1, float z1);

double sinc(double x);

double flat_top(double x);