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

#ifndef _DOGMA_MODIFIER_H
#define _DOGMA_MODIFIER_H 1

#include "dogma.h"
#include "dogma_internal.h"
#include "attribute.h"

/* -------- General -------- */

#define DOGMA_FILTERTYPE_PASS 0
#define DOGMA_FILTERTYPE_GROUP 1
#define DOGMA_FILTERTYPE_SKILL_REQUIRED 2

#define DOGMA_PENALIZABLE_ASSOCTYPES (	  \
	(1 << DOGMA_ASSOC_PreMul) | \
	(1 << DOGMA_ASSOC_PostMul) | \
	(1 << DOGMA_ASSOC_PostPercent) | \
	(1 << DOGMA_ASSOC_PreDiv) | \
	(1 << DOGMA_ASSOC_PostDiv) \
)





/* -------- Data types -------- */

enum dogma_scope_e {
	DOGMA_SCOPE_Item,
	DOGMA_SCOPE_Location,
	DOGMA_SCOPE_Owner,
	DOGMA_SCOPE_Gang,
	DOGMA_SCOPE_Gang_Ship,
};
typedef enum dogma_scope_e dogma_scope_t;

struct dogma_filter_s {
	uint8_t type;

	union {
		groupid_t groupid;
		typeid_t typeid;
	};
};
typedef struct dogma_filter_s dogma_filter_t;

struct dogma_modifier_s {
	attributeid_t targetattribute;
	dogma_env_t* targetenv;
	dogma_association_t assoctype;
	bool penalized;
	bool singleton; /* Of all modifiers with singleton set to true
	                 * affecting the same attribute, only the "best"
	                 * will be chosen. (Used for gang bonuses.) */
	attributeid_t sourceattribute;
	dogma_env_t* sourceenv;
	dogma_filter_t filter;
	dogma_scope_t scope;
};
typedef struct dogma_modifier_s dogma_modifier_t;





/* -------- Internal functions -------- */

/* Add a modifier (to its targetenv). */
int dogma_add_modifier(dogma_modifier_t*);

/* Remove a modifier (from its targetenv). */
int dogma_remove_modifier(dogma_modifier_t*);

/* Checks whether a modifier applies to a given environment. */
int dogma_modifier_is_applicable(dogma_context_t*, dogma_env_t*, dogma_modifier_t*, bool*);

/* Apply a given modifier. */
int dogma_apply_modifier(dogma_context_t*, dogma_modifier_t*, double*);





#endif
