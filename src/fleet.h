/* libdogma
 * Copyright (C) 2013 Romain "Artefact2" Dalmaso <artefact2@gmail.com>
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
#ifndef _DOGMA_FLEET_H
#define _DOGMA_FLEET_H 1

#include "dogma.h"
#include "dogma_internal.h"
#include <assert.h>

#define DOGMA_INIT_FLEET_CTX(fctxptr, _parent, _index) do {	  \
		(fctxptr) = malloc(sizeof(dogma_fleet_context_t)); \
		(fctxptr)->commander = NULL; \
		(fctxptr)->booster = NULL; \
		(fctxptr)->parent = (_parent); \
		(fctxptr)->index = (_index); \
		(fctxptr)->subfleets = NULL; \
		(fctxptr)->members = NULL; \
	} while(0)

#endif
