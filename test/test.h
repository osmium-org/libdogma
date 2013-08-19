/* libdogma
 * Copyright (C) 2012, 2013 Romain "Artefact2" Dalmaso <artefact2@gmail.com>
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

#pragma once
#ifndef __DOGMA_TEST_H
#define __DOGMA_TEST_H

#include <dogma.h>
#include <dogma-names.h>
#include <dogma-extra.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* This header is intended to be used for test programs and debugging
 * only. Do not use it in your applications! */

#define stringify(s) stringify2(s)
#define stringify2(s) #s

#define failf(expected, result) do {	  \
		fprintf(stderr, "%s:%i: Expected: %f, Got: %f\n", \
		        __FILE__, __LINE__, (expected), (result)); \
		assert(0); \
		exit(1); \
	} while(0)

#define assertf(expected, result, epsilon)  do {	  \
		double e = (expected), r = (result), eps = (epsilon); \
		if(!((e - r <= eps) && (r - e <= eps))) { \
			failf(e, r); \
		} \
	} while(0)

#define debug_affectors(ctx, location) do {	  \
		dogma_simple_affector_t* aff; \
		size_t len; \
		size_t k = 0; \
		dogma_get_affectors((ctx), (location), &aff, &len); \
		printf("Affectors of %s\n", stringify(location)); \
		for(size_t i = 0; i < len; ++i) { \
			printf( \
				"%c%c%1i (source %-7i) (attribute %-6i) %c= %g\n", \
				(aff[i].flags & DOGMA_AFFECTOR_PENALIZED) ? 'P' : ' ', \
				(aff[i].flags & DOGMA_AFFECTOR_SINGLETON) ? 'S' : ' ', \
				aff[i].order, \
				aff[i].id, aff[i].destid, \
				aff[i].operator, aff[i].value \
			); \
			++k; \
		} \
		printf("%zu modifier%c\n", k, k == 1 ? ' ' : 's'); \
		dogma_free_affector_list(aff); \
	} while(0)

#define debug_affectors_of_attribute(ctx, location, att) do {	  \
		dogma_simple_affector_t* aff; \
		size_t len; \
		size_t k = 0; \
		dogma_get_affectors((ctx), (location), &aff, &len); \
		printf("Affectors of %s on attribute %i\n", stringify(location), (att)); \
		for(size_t i = 0; i < len; ++i) { \
			if(aff[i].destid != (att)) continue; \
			printf( \
				"%c%c%1i (source %-7i) %c= %g\n", \
				(aff[i].flags & DOGMA_AFFECTOR_PENALIZED) ? 'P' : ' ', \
				(aff[i].flags & DOGMA_AFFECTOR_SINGLETON) ? 'S' : ' ', \
				aff[i].order, \
				aff[i].id, \
				aff[i].operator, aff[i].value \
			); \
			++k; \
		} \
		printf("%zu modifier%c\n", k, k == 1 ? ' ' : 's'); \
		dogma_free_affector_list(aff); \
	} while(0)





#endif
