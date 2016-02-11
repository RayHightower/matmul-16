/*
  matmul_unit.c

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


// This program is the accelerator part of the matmul() example project.
//
// This program runs on the Epiphany system and answers the host with the
// calculation result of the operand matrices.
//
// Aug-2012, YS.


#include "matlib.h"
#include "matmul.h"
#include "common_buffers.h"

#define _gptr(coreID, ptr) ( (void *) ( (((unsigned) coreID) << 20) | ((unsigned) ((void *) ptr)) ) )

#define _MAX_MEMBER_ 64

// Here's an example of explicit placement of static objects in memory. The three matrices
// are placed in the respective SRAM banks. However, if necessary, the linker may relocate
// the objects wherever within the bank. The core structure "me" is specifically located
// at an explicit address - 0x7000. To do that, a custom linker file (LDF) was defined,
// based on a standard LDF, in which a special data section was added with the required
// address to assign to the "me" object.
volatile float  AA[2][_Score][_Score] SECTION(".data_bank1");  // local A submatrix
volatile float  BB[2][_Score][_Score] SECTION(".data_bank2");  // local B submatrix
volatile float  CC   [_Score][_Score] SECTION(".data_bank3");  // local C submatrix
volatile e_tcb_t _tcb                 SECTION("section_core") ALIGN(8);
volatile core_t me                    SECTION("section_core"); // core data structure
volatile shared_buf_ptr_t Mailbox     SECTION("section_core"); // Mailbox pointers;

void bigmatmul();
void init();

int  dma0cpyi(void *src, void *dst);
int  dma0cpyo(void *src, void *dst);
int  dma0cpyc(void *src, void *dst);

unsigned int ctimer_set(e_ctimer_id_t timer, e_ctimer_config_t config, unsigned int val);

int matmul_unit()
{
	unsigned time_s, time_e;

	// Initialize data structures - mainly target pointers
	init();
	
	while (1)
	{
		// Core #0 initializes operand matrices
		if (me.corenum == 0)
		{
			// Wait for matmul() call from host. When a rising
			// edge is detected in the core's respective mailbox,
			// the loop is terminated and a call to the actual
			// matmul() function is initiated.
			while (Mailbox.pCore->go[me.corenum] == 0) {};

			ctimer_set(E_CTIMER_0, E_CTIMER_CLK, E_CTIMER_MAX);
			__asm__ volatile ("movfs %0, ctimer0" : "=r" (time_s));

			Mailbox.pCore->ready = 0;

			me.go_sync = 64;
		} else {
			// Wait for matmul() call from host. When a rising
			// edge is detected in the core's respective mailbox,
			// the loop is terminated and a call to the actual
			// matmul() function is initiated.
			while (me.go_sync == 0) {};
		}
		// Signal to next core.
		*me.tgt_go_sync = me.corenum + 1;

		// Calculate. During this time, the host polls the
		// core's respective mailbox, waiting for a falling
		// edge indicating the end of the calculation.
		bigmatmul();

		// Signal End-Of-Calculation to the host.
		Mailbox.pCore->go[me.corenum] = 0;

		// Core #0 waits until cores finished calculation.
		if (me.corenum == 0)
		{
			// Signal End-Of-Calculation to previous core and wait for next core's signal.
			me.go_sync = 0;
			while (*me.tgt_go_sync > 0) {};

			__asm__ volatile ("movfs %0, ctimer0" : "=r" (time_e));
			me.count = time_s - time_e;

			Mailbox.pCore->clocks = me.count;
			Mailbox.pCore->ready  = 1;
		} else {
			// If next core ended calculation, signal own End-Of-Calculation to previous core.
			while (*me.tgt_go_sync > 0) {};
			me.go_sync = 0;
		}
	}

	return 0;
}


void init()
{
	// Initialize the mailbox shared buffer pointers
	Mailbox.pBase = (void *) SHARED_DRAM;
	Mailbox.pA    = Mailbox.pBase + offsetof(shared_buf_t, A[0]);
	Mailbox.pB    = Mailbox.pBase + offsetof(shared_buf_t, B[0]);
	Mailbox.pC    = Mailbox.pBase + offsetof(shared_buf_t, C[0]);
	Mailbox.pCore = Mailbox.pBase + offsetof(shared_buf_t, core);

	// Initialize per-core parameters - core data structure
	
	// Initialize pointers to the operand matrices ping-pong arrays
	me.bank[_BankA][_PING] = (void *) &(AA[0][0][0]);
	me.bank[_BankA][_PONG] = (void *) &(AA[1][0][0]);
	me.bank[_BankB][_PING] = (void *) &(BB[0][0][0]);
	me.bank[_BankB][_PONG] = (void *) &(BB[1][0][0]);
	me.bank[_BankC][_PING] = (void *) &(CC   [0][0]);

	// Initialize the pointer addresses of the arrays in the horizontal and vertical target
	// cores, where the submatrices data will be swapped, and the inter-core sync signals.
	me.tgt_bk[_BankA][_PING] = _gptr(me.coreIDh, me.bank[_BankA][_PONG]);
	me.tgt_bk[_BankA][_PONG] = _gptr(me.coreIDh, me.bank[_BankA][_PING]);
	me.tgt_synch             = _gptr(me.coreIDh, (&(me.synch)));
	
	me.tgt_bk[_BankB][_PING] = _gptr(me.coreIDv, me.bank[_BankB][_PONG]);
	me.tgt_bk[_BankB][_PONG] = _gptr(me.coreIDv, me.bank[_BankB][_PING]);
	me.tgt_syncv             = _gptr(me.coreIDv, (&(me.syncv)));

	me.tgt_go_sync  = _gptr(me.coreIDn, (&(me.go_sync)));
	me.tgt_dma_sync = _gptr(me.coreIDn, (&(me.dma_sync)));

	me.pingpong = _PING;

	// Clear the inter-core sync signals
	me.synch = 0;
	me.syncv = 0;
	me.go_sync = 0;
	me.dma_sync = (me.corenum == 0) ? 1 : 0;
	// Init the host-accelerator sync signals
	Mailbox.pCore->go[me.corenum] = 0;
	if (me.corenum == 0)
		Mailbox.pCore->ready = 1;
	
	me.count = 0;

	return;
}


void bigmatmul()
{
	int   im, jm, km, imp, jmp, kmp; // index of chip array (chip) (0..#arrays-in-matrix)
	int   kc, icp, jcp;              // index of core in a chip (0..Nside)

	// Chip (mesh) loop through operand matrix:
	// Smtx is size of operand matrices (Smtx x Smtx)
	// Schip is size of a chip matrix (Schip x Schip)
	for (im=0, imp=0; im<_Nchips; im++, imp+=_Schip)
		for (jm=0, jmp=0; jm<_Nchips; jm++, jmp+=_Schip)
		{
			// First clear the local result submatrix. The product result will be
			// integrated into this submatrix.
			matclr(me.bank[_BankC][_PING], _Score);

			for (km=0, kmp=0; km<_Nchips; km++, kmp+=_Schip)
			{
				// Core loop through chip:
				// for every chip (mesh) iteration on the operand matrix
				// calculate the matmul of the chip-sized submatrices
				// in granularity of cores
				while (me.dma_sync == 0) {};
				me.dma_sync = 0;
				icp = me.row * _Score;
				jcp = ((me.col + me.row) % _Nside) * _Score;
				dma0cpyi(&(Mailbox.pA[(imp+icp)*_Smtx + (kmp+jcp)]), me.bank[_BankA][me.pingpong]);


				jcp = me.col * _Score;
				icp = ((me.row + me.col) % _Nside) * _Score;
				dma0cpyi(&(Mailbox.pB[(kmp+icp)*_Smtx + (jmp+jcp)]), me.bank[_BankB][me.pingpong]);
				*me.tgt_dma_sync = 1;

				// Multiply submatrices (inner product of row x column)
				for (kc=0; kc<_Nside; kc++)
				{
					// Core matmul:
					// for every core calculate the matmul
					// of its sample elements and accumulate with
					// previous partial products
					matmac(me.bank[_BankA][me.pingpong], me.bank[_BankB][me.pingpong], me.bank[_BankC][_PING], _Score);

					// After multiplying the submatrices in each core, rotate the rows of A and columns of B
					// If this is the last iteration of the inner product, skip the matrix swap
					// Rotate A banks horizontally
					while (*me.tgt_synch == 1) {};
					if (kc <= (_Nside - 1))
						dma0cpyc(me.bank[_BankA][me.pingpong], me.tgt_bk[_BankA][me.pingpong]);
					*me.tgt_synch = 1;

					// Rotate B banks vertically
					while (*me.tgt_syncv == 1) {};
					if (kc <= (_Nside - 1))
						dma0cpyc(me.bank[_BankB][me.pingpong], me.tgt_bk[_BankB][me.pingpong]);
					*me.tgt_syncv = 1;

					me.pingpong = 1 - me.pingpong;

					// Wait for peer sync
					while ((me.synch == 0) || (me.syncv == 0)) {};
					me.synch = me.syncv = 0;
				}
			}

			// Extract the chip result to DRAM
			icp = me.row * _Score;
			jcp = me.col * _Score;
			while (me.dma_sync == 0) {};
			me.dma_sync = 0;
			dma0cpyo(me.bank[_BankC][_PING], &(Mailbox.pC[(imp+icp)*_Smtx + (jmp+jcp)]));
			*me.tgt_dma_sync = 1;
		}

	return;
}


int dma0start(e_tcb_t *tcb)
{
	int           status;
	unsigned      start;
	e_irq_state_t irq_state;

	/* Read the interrupt enable state before modifying it. */
	__asm__ __volatile__ ("MOVFS %0, STATUS"  : "=r" (status) : );
	irq_state = (status & 0x2) ? E_IRQ_DISABLE : E_IRQ_ENABLE;
	__asm__("gid");


	/* wait for the DMA engine to be idle */
	status = ~ 0x0;
	while (status)
	{
		__asm__ __volatile__ ("MOVFS %0, DMA0STATUS"  : "=r" (status) : );
		status = status & 0xf;
	}

	start = ((volatile int) (tcb) << 16) | E_DMA_STARTUP;
	__asm__ __volatile__ ("MOVTS DMA0CONFIG, %0"  : : "r" (start) );

	/* wait for the DMA engine to be idle */
	status = ~ 0x0;
	while (status)
	{
		__asm__ __volatile__ ("MOVFS %0, DMA0STATUS"  : "=r" (status) : );
		status = status & 0xf;
	}

	switch (irq_state)
	{
	case E_IRQ_ENABLE:
		__asm__("gie");
		break;
	case E_IRQ_DISABLE:
		__asm__("gid");
		break;
	}

	return 0;
}


int dma0cpyi(void *src, void *dst)
{
	unsigned stride_i_src;
	unsigned stride_i_dst;
	unsigned stride_o_src;
	unsigned stride_o_dst;
	unsigned count_i;
	unsigned count_o;
	unsigned config;

	config       = (1 << 10) | E_DMA_DOUBLE | E_DMA_MASTER | E_DMA_ENABLE;
	stride_i_src = (1 << 3);
	stride_i_dst = (1 << 3);
	stride_o_src = ((_Smtx - _Score) << 2) + (1 << 3);
	stride_o_dst = (1 << 3);
	count_i      = _Score >> 1;
	count_o      = _Score;

	_tcb.config       = config;
	_tcb.inner_stride = (stride_i_dst << 16) | (stride_i_src);
	_tcb.count        = (count_o << 16)      | (count_i);
	_tcb.outer_stride = (stride_o_dst << 16) | (stride_o_src);
	_tcb.src_addr     = src;
	_tcb.dst_addr     = dst;

	dma0start((e_tcb_t *) (&_tcb));

	return 0;
}


int dma0cpyo(void *src, void *dst)
{
	unsigned stride_i_src;
	unsigned stride_i_dst;
	unsigned stride_o_src;
	unsigned stride_o_dst;
	unsigned count_i;
	unsigned count_o;
	unsigned config;

	config       = (1 << 10) | E_DMA_DOUBLE | E_DMA_MASTER | E_DMA_ENABLE;
	stride_i_src = (1 << 3);
	stride_i_dst = (1 << 3);
	stride_o_src = (1 << 3);
	stride_o_dst = ((_Smtx - _Score) << 2) + (1 << 3);
	count_i      = _Score >> 1;
	count_o      = _Score;

	_tcb.config       = config;
	_tcb.inner_stride = (stride_i_dst << 16) | (stride_i_src);
	_tcb.count        = (count_o << 16)      | (count_i);
	_tcb.outer_stride = (stride_o_dst << 16) | (stride_o_src);
	_tcb.src_addr     = src;
	_tcb.dst_addr     = dst;

	dma0start((e_tcb_t *) (&_tcb));

	return 0;
}


int dma0cpyc(void *src, void *dst)
{
	unsigned stride_i_src;
	unsigned stride_i_dst;
	unsigned stride_o_src;
	unsigned stride_o_dst;
	unsigned count_i;
	unsigned count_o;
	unsigned config;

	config       = (1 << 10) | E_DMA_DOUBLE | E_DMA_MASTER | E_DMA_ENABLE;
	stride_i_src = (1 << 3);
	stride_i_dst = (1 << 3);
	stride_o_src = (1 << 3);
	stride_o_dst = (1 << 3);
	count_i      = _Score >> 1;
	count_o      = _Score;

	_tcb.config       = config;
	_tcb.inner_stride = (stride_i_dst << 16) | (stride_i_src);
	_tcb.count        = (count_o << 16)      | (count_i);
	_tcb.outer_stride = (stride_o_dst << 16) | (stride_o_src);
	_tcb.src_addr     = src;
	_tcb.dst_addr     = dst;

	dma0start((e_tcb_t *) (&_tcb));

	return 0;
}

