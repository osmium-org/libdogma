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
#ifndef _DOGMA_ATTRIBUTE_H
#define _DOGMA_ATTRIBUTE_H 1

#include "dogma.h"
#include "dogma_internal.h"
#include "modifier.h"

/* -------- Internal functions -------- */

/* Get an attribute value of an environment, applying all relevant
 * modifiers to it. */
int dogma_get_env_attribute(dogma_context_t*, dogma_env_t*, attributeid_t, double*);

/* Checks whether an environment requires a skill. */
int dogma_env_requires_skill(dogma_context_t*, dogma_env_t*, typeid_t, bool*);





#endif
