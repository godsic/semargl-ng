#include "core/preprocess.h"


void avg(float **data, double* stamps, size_t comp, size_t comp_size, size_t number_of_frames, size_t frame_bias) {
	size_t i, k, c, bias;
	float *m = *data;
	float avg = 0.0f;
	double stamp;
	float norm = 1.0f / (float)comp_size;

	for (k = 0; k < number_of_frames; k++) {
		bias = comp * comp_size * k;
		stamp = stamps[frame_bias + k];
		printf("%.8e", stamp);
		for (c = 0; c < comp; c++) {
			avg = 0.0f;
			printf("\t");
			for (i = 0; i < comp_size; i++)	{
				avg = avg + m[bias + c * comp_size + i];
			}
			avg = avg * norm;
			printf("%.8e", avg);
		}
		printf("\n");
	}
	return;
}

void mxyz2muvw(float **data, float *m0, size_t comp_size, size_t number_of_frames) {

	size_t i, k, bias;

	float *m = *data;
	float *mx;
	float *my;
	float *mz;

	float *mu;
	float *mv;
	float *mw;

	float *m0x = &m0[0 * comp_size];
	float *m0y = &m0[1 * comp_size];
	float *m0z = &m0[2 * comp_size];

	float dmx, dmy, dmz, dmu, dmv, dmw;
	float ux, uy, uz;
	float vx, vy, vz;
	float wx, wy, wz;
	float nu, nv, nw;

	for (k = 0; k < number_of_frames; k++) {

		bias = 3 * comp_size * k;

		mx = &m[0 * comp_size + bias];
		my = &m[1 * comp_size + bias];
		mz = &m[2 * comp_size + bias];

		mu = mx;
		mv = my;
		mw = mz;

		for (i = 0; i < comp_size; i++)	{

			// calculate dynamic part of the magnetization
			dmx = mx[i] - m0x[i];
			dmy = my[i] - m0y[i];
			dmz = mz[i] - m0z[i];

			// go to native, uvw, coordinate system

			// u coincides with the magnetization direction in ground state
			ux = m0x[i];
			uy = m0y[i];
			uz = m0z[i];

			nu = norm(ux, uy, uz);
			if (nu != 0.0f) {
				ux /= nu;
				uy /= nu;
				uz /= nu;
			}

			// v should be in-plane (won't work for non-planar geometries)
			vx = -uy;
			vy = ux;
			vz = 0.0f;

			nv = norm(vx, vy, vz);
			if (nv != 0.0f) {
				vx /= nv;
				vy /= nv;
				vz /= nv;
			}

			// w = u × v

			wx = (uy * vz - uz * vy);
			wy = (uz * vx - ux * vz);
			wz = (ux * vy - uy * vx);

			nw = norm(wx, wy, wz);
			if (nw != 0.0f) {
				wx /= nw;
				wy /= nw;
				wz /= nw;
			}

			dmu = dot(dmx, dmy, dmz, ux, uy, uz);
			dmv = dot(dmx, dmy, dmz, vx, vy, vz);
			dmw = dot(dmx, dmy, dmz, wx, wy, wz);

			mu[i] = dmu;
			mv[i] = dmv;
			mw[i] = dmw;
		}
	}
	return;
}
