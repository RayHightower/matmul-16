/*
  matmul_host.c

  Copyright (C) 2012 Adapteva, Inc.
  Contributed by Yaniv Sapir <yaniv@adapteva.com>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program, see the file COPYING.  If not, see
  <http://www.gnu.org/licenses/>.
*/


// This program is the host part of the matmul() example project.
//
// This program runs on the linux host and invokes the Epiphany matmul()
// implementation. It communicates with the system via the eHost library.
// After establishing a connection using the e-server, operand matrices
// are generated based on the run-time parameter "seed". A reference
// calculation is done on the host and is compared to the Epiphany
// result. A succes/error message is printed on the terminal according
// to the result of the comparison.
//
// Jan-2012, YS.

//#include <unistd.h>
#include <stdio.h>
#include <stddef.h>
#include <sys/time.h>
#include <math.h>
#include <e_host.h>
#include "matlib.h"
#include "matmul.h"
#include "common_buffers.h"

#define _MAX_MEMBER_ 64
#define eMHz 400e6

int   main(int argc, char *argv[]);
void  matrix_init(int seed);
int   matmul_go();
int   matcmp(FILE *fp, volatile float * a, volatile float * b, int NN);


FILE *fo, *fi;
unsigned int DRAM_BASE = 0x81000000;
float Cref[_Smtx * _Smtx];
float Cdiff[_Smtx * _Smtx];

typedef struct timeval timeval_t;


int main(int argc, char *argv[])
{
	const char *servIP = "127.0.0.1";
	const unsigned short eServLoaderPort = 50999;
	float seed;
	unsigned int addr, clocks;
	size_t sz;
	timeval_t timer[4];
	double tdiff[2];
	
	fo = stdout;
	fi = stdin;

#if 0
	// Process input arguments
	if (argc > 1)
		seed = atoi(argv[1]);
	else
		seed = (int) ((double) rand() / RAND_MAX) * 0;
#endif

	fprintf(fo, "\nMatrix: C[%d][%d] = A[%d][%d] * B[%d][%d]\n\n", _Smtx, _Smtx, _Smtx, _Smtx, _Smtx, _Smtx);
	fprintf(fo, "Enter seed for generation of operand matrices: ");
	fscanf(stdin, "%f", &seed);
	seed = fmodf(seed, _MAX_MEMBER_);
	fprintf(fo, "Seed = %f\n", seed); fflush(fo);

	// Connect to e-server for communicating with the Epiphany system
	if (e_open((char *) servIP, eServLoaderPort))
	{
		fprintf(fo, "\nERROR: Can't establish connection to E-SERVER!\n\n");
		exit(1);
	}


	// Generate operand matrices based on a provided seed
	matrix_init(seed);

	// Copy operand matrices to Epiphany system
	addr = DRAM_BASE + offsetof(shared_buf_t, A[0]);
	sz = sizeof(Mailbox.A);
	fprintf(fo, "Writing A[%ldB] to address %08x...\n", sz, addr);
	e_write(addr, (void *) Mailbox.A, sz);
	
	addr = DRAM_BASE + offsetof(shared_buf_t, B[0]);
	sz = sizeof(Mailbox.B);
	fprintf(fo, "Writing B[%ldB] to address %08x...\n", sz, addr);
	e_write(addr, (void *) Mailbox.B, sz);

	// Call the Epiphany matmul() function
	fprintf(fo, "GO! ...   "); fflush(fo);
	gettimeofday(&timer[0], NULL);
	matmul_go();
	gettimeofday(&timer[1], NULL);
	fprintf(fo, "Finished calculating Epiphany result.\n"); fflush(fo);

	// Read result matrix
	addr = DRAM_BASE + offsetof(shared_buf_t, C[0]);
	sz = sizeof(Mailbox.C);
	fprintf(fo, "Reading result from Epiphany ...\n"); fflush(fo);
	e_read(addr, (void *) Mailbox.C, sz);

	addr = DRAM_BASE + offsetof(shared_buf_t, core.clocks);
	sz = sizeof(Mailbox.core.clocks);
	e_read(addr, (void *) &Mailbox.core.clocks, sz);
	clocks = Mailbox.core.clocks;

	// Calculate a reference result
	fprintf(fo, "Calculating result on Host ...   "); fflush(fo);
	gettimeofday(&timer[2], NULL);
	matmul(Mailbox.A, Mailbox.B, Cref, _Smtx);
	gettimeofday(&timer[3], NULL);
	fprintf(fo, "Finished calculating Host result.\n"); fflush(fo);

	// Calculate the difference between the Epiphany result and the reference result
	fprintf(fo, "Verifying result correctness ...   ");	fflush(fo);
	matsub(Mailbox.C, Cref, Cdiff, _Smtx);

//	tdiff[0] = (timer[1].tv_sec - timer[0].tv_sec) * 1000 + ((double) (timer[1].tv_usec - timer[0].tv_usec) / 1000.0);
	tdiff[0] = ((double) clocks) / eMHz * 1000;
	tdiff[1] = (timer[3].tv_sec - timer[2].tv_sec) * 1000 + ((double) (timer[3].tv_usec - timer[2].tv_usec) / 1000.0);


	// If the difference is 0, then the matrices are identical and the
	// calculation was correct
	if (iszero(Cdiff, _Smtx))
	{
		fprintf(fo, "C_epiphany == C_host\n");
	} else {
		fprintf(fo, "\n\nERROR: C_epiphany is different from C_host !!!\n\n");
	}
	fprintf(fo, "Epiphany -  time: %9.1f msec (%u cycles)\n", tdiff[0], clocks);
	fprintf(fo, "Host     -  time: %9.1f msec\n", tdiff[1]);

	fprintf(fo, "\n* * *   EPIPHANY FTW !!!   * * *\n"); fflush(fo);

//	matcmp(fo, Mailbox.C, Cref, _Smtx); fflush(fo);
	
	// Close connection to e-server
	if (e_close())
	{
		fprintf(fo, "\nERROR: Can't close connection to E-SERVER!\n\n");
		exit(1);
	}

	return 0;
}


// Call (invoke) the matmul() function
int matmul_go()
{
	unsigned int addr;
	size_t sz;
	
	// Signal cores to start crunching
	Mailbox.core.go[0] = _MAX_MEMBER_;
	addr = DRAM_BASE + offsetof(shared_buf_t, core.go[0]);
	sz = sizeof(int);
	e_write(addr, (void *) (&Mailbox.core.go[0]), sz);


	// Wait until cores finished calculation
	addr = DRAM_BASE + offsetof(shared_buf_t, core.ready);
	sz = sizeof(int);
	Mailbox.core.ready = 0;
	while (Mailbox.core.ready == 0)
		e_read(addr, (void *) (&Mailbox.core.ready), sz);

	return 0;
}


// Initialize operand matrices
void matrix_init(int seed)
{
	int i, j, p;

	p = 0;
	for (i=0; i<_Smtx; i++)
		for (j=0; j<_Smtx; j++)
			Mailbox.A[p++] = (i + j + seed) % _MAX_MEMBER_;

	p = 0;
	for (i=0; i<_Smtx; i++)
		for (j=0; j<_Smtx; j++)
			Mailbox.B[p++] = ((i + j) * 2 + seed) % _MAX_MEMBER_;

	p = 0;
	for (i=0; i<_Smtx; i++)
		for (j=0; j<_Smtx; j++)
			Mailbox.C[p++] = 0x8dead;
		
	return;
}


// Compare two matrices a and b  NNxNN and print different elements
int matcmp(FILE *fp, volatile float * a, volatile float * b, int NN)
{
	int i, j, z;

	z = 0;
	for (i=0; i<NN; i++)
		for (j=0; j<NN; j++)
			if (fabs(a[i*NN+j] - b[i*NN+j]) > EPS)
			{
				fprintf(fp, "%10d , %10d , %f , %f\n", i, j, a[i*NN+j], b[i*NN+j]);
				z = z | 1;
			}

	return (!z);
}
