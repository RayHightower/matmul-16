/*
  main.c

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


/* Add in this module code that is specific to this core */

#include "matmul.h"

extern volatile core_t me;

int matmul_unit(void);

int main(void)
{
	int status;

	status = 0;

	me.coreID    = 0x8a4;
	me.row       = 2;
	me.col       = 0;
	me.corenum   = 8;
	me.coreIDh   = 0x8a7;
	me.coreIDv   = 0x864;
	me.coreIDn   = 0x8a5;

	/* jump to multicore common code */
	matmul_unit();

	return status;
}
