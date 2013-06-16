/* libdogma
 * Copyright (C) 2012 Romain "Artefact2" Dalmaso <artefact2@gmail.com>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <assert.h>
#include <dogma.h>
#include <stdio.h>
#include <stdlib.h>

#define failf(expected, result) do {										\
		fprintf(stderr, "Expected: %f, Got: %f\n",						\
				(expected), (result));									\
		exit(1);														\
	} while(0)

#define assertf(expected, result, epsilon)  do {						\
		if(!(((expected) - (result) <= (epsilon))						\
			 && ((result) - (expected) <= (epsilon)))) {				\
			failf((expected), (result));									\
		}																\
	} while(0)
