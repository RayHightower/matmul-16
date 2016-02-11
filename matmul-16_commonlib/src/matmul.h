/*
  matmul.h

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


#ifndef __MATMUL_H__
#define __MATMUL_H__

#define _Nchips 2                  // # of chips in operand matrix side
#define _Nside  4                  // # of cores in chip side
#define _Ncores (_Nside * _Nside)  // Num of cores = 16
#define _Score  16                 // side size of per-core sub-submatrix (max 32)
#define _Schip  (_Score * _Nside)  // side size of per-chip submatrix
#define _Smtx   (_Schip * _Nchips) // side size of operand matrix

#define _Nbanks 4                  // Num of SRAM banks on core

#define _BankP  0
#define _BankA  1
#define _BankB  2
#define _BankC  3
#define _PING   0
#define _PONG   1


typedef struct {
	int    coreID;
	int    corenum;
	int    row;
	int    col;
	int    coreIDh;
	int    coreIDv;
	int    coreIDn;

	int    go_sync;      // The "go" signal from prev core
	int    synch;        // Sync with horizontal peer core
	int    syncv;        // Sync with vertical peer core
	int    dma_sync;     // Sync external DMA to avoid thrashing
	int   *tgt_go_sync;  // ptr to go_sync of next core
	int   *tgt_synch;    // ptr to synch of target neighbor
	int   *tgt_syncv;    // ptr to syncv of target neighbor
	int   *tgt_dma_sync; // ptr to dma_sync of target neighbor

	void  *bank[_Nbanks][2];   // Ping Pong Bank local space pointer
	void  *tgt_bk[_Nbanks][2]; // Target Bank for matrix rotate in global space
	
	int    pingpong; // Ping-Pong bank select indicator

	int    count;
} core_t;


typedef struct {
	int      go[_Ncores];      // Call for matmul function
	int      ready;            // Core is ready after reset
	unsigned clocks;           // cycle count
} mbox_t;


typedef struct {
	float  A[_Smtx * _Smtx]; // Global A matrix 
	float  B[_Smtx * _Smtx]; // Global B matrix 
	float  C[_Smtx * _Smtx]; // Global C matrix 
	mbox_t core;
} shared_buf_t;


typedef struct {
	void   *pBase;       // ptr to base of shared buffers
	float  *pA;          // ptr to global A matrix
	float  *pB;          // ptr to global B matrix
	float  *pC;          // ptr to global C matrix
	mbox_t *pCore;       // ptr to cores mailbox
} shared_buf_ptr_t;




typedef enum
{
	E_CTIMER_0 = 0,
	E_CTIMER_1 = 1
} e_ctimer_id_t;

typedef enum
{
	E_CTIMER_OFF              = 0x0,
	E_CTIMER_CLK              = 0x1,
	E_CTIMER_IDLE             = 0x2,
	E_CTIMER_IALU_INST        = 0x4,
	E_CTIMER_FPU_INST         = 0x5,
	E_CTIMER_DUAL_INST        = 0x6,
	E_CTIMER_E1_STALLS        = 0x7,
	E_CTIMER_RA_STALLS        = 0x8,
	E_CTIMER_EXT_FETCH_STALLS = 0xc,
	E_CTIMER_EXT_LOAD_STALLS  = 0xd,
/*	E_CTIMER_EXTN_STALLS      = 0x3, */
} e_ctimer_config_t;

#define E_CTIMER_MAX (~0)



typedef enum {
	E_SYNC         = 0,
	E_SW_EXCEPTION = 1,
	E_MEM_FAULT    = 2,
	E_TIMER0_INT   = 3,
	E_TIMER1_INT   = 4,
	E_DMA0_INT     = 6,
	E_DMA1_INT     = 7,
	E_SW_INT       = 9
} e_irq_type_t;

typedef enum
{
	E_IRQ_DISABLE  = 0,
	E_IRQ_ENABLE   = 1
} e_irq_state_t;



extern const unsigned _NUM_ROWS_IN_CHIP_;
extern const unsigned _NUM_COLS_IN_CHIP_;
extern const unsigned _FIRST_CORE_ROW_;
extern const unsigned _FIRST_CORE_COL_;
#define E_ROWS_IN_CHIP   ((unsigned)(&_NUM_ROWS_IN_CHIP_))
#define E_COLS_IN_CHIP   ((unsigned)(&_NUM_COLS_IN_CHIP_))
#define E_FIRST_CORE_ROW ((unsigned)(&_FIRST_CORE_ROW_))
#define E_FIRST_CORE_COL ((unsigned)(&_FIRST_CORE_COL_))

typedef unsigned int e_coreid_t;

typedef enum
{
/* e_neighbor_id() wrap constants */
	E_CHIP_WRAP = 0,
	E_ROW_WRAP  = 1,
	E_COL_WRAP  = 2,
/* e_neighbor_id() dir constants */
	E_NEXT_CORE = 0,
	E_PREV_CORE = 1
} e_coreid_wrap_t;


typedef enum
{
	/* eCore Registers */
	E_CONFIG = 0xf0400,
	E_STATUS = 0xf0404,
	E_PC     = 0xf0408,
	E_IRET   = 0xf0420,
	E_IMASK  = 0xf0424,
	E_ILAT   = 0xf0428,
	E_ILATST = 0xf042C,
	E_ILATCL = 0xf0430,
	E_IPEND  = 0xf0434,

	/* DMA registers */
	E_DMA0CONFIG   = 0xf0500,
	E_DMA0STRIDE   = 0xf0504,
	E_DMA0COUNT    = 0xf0508,
	E_DMA0SRCADDR  = 0xf050C,
	E_DMA0DSTADDR  = 0xf0510,
	E_DMA0AUTODMA0 = 0xf0514,
	E_DMA0AUTODMA1 = 0xf0518,
	E_DMA0STATUS   = 0xf051C,
	E_DMA1CONFIG   = 0xf0520,
	E_DMA1STRIDE   = 0xf0524,
	E_DMA1COUNT    = 0xf0528,
	E_DMA1SRCADDR  = 0xf052C,
	E_DMA1DSTADDR  = 0xf0530,
	E_DMA1AUTODMA0 = 0xf0534,
	E_DMA1AUTODMA1 = 0xf0538,
	E_DMA1STATUS   = 0xf053C,

	/* Event Timer Registers */
	E_CTIMER0 = 0xf0438,
	E_CTIMER1 = 0xf043C,

	/* Processor Control Registers */
	E_MEMPROTECT = 0xf0608,
	E_COREID     = 0xf0704
} e_reg_id_t;


typedef enum
{
	E_DMA_ENABLE  = (1<<0),
	E_DMA_MASTER  = (1<<1),
	E_DMA_CHAIN   = (1<<2),
	E_DMA_STARTUP = (1<<3),
	E_DMA_IRQEN   = (1<<4),
	E_DMA_BYTE    = (0<<5),
	E_DMA_SHORT   = (1<<5),
	E_DMA_LONG    = (2<<5),
	E_DMA_DOUBLE  = (3<<5)
} e_dma_tcb_cfg_t;

typedef enum
{
	E_DMA_0 = 0,
	E_DMA_1 = 1
} e_dma_id_t;

typedef enum
{
	E_ALIGN_BYTE,
	E_ALIGN_SHORT,
	E_ALIGN_LONG,
	E_ALIGN_DOUBLE,
	E_ALIGN_AUTO
} e_dma_align_t;

typedef struct
{
	unsigned long config;
	unsigned long inner_stride;
	unsigned long count;
	unsigned long outer_stride;
	void         *src_addr;
	void         *dst_addr;
} e_tcb_t;


#endif // __MATMUL_H__
