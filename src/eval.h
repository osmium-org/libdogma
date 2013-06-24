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

#pragma once
#ifndef _DOGMA_EVAL_H
#define _DOGMA_EVAL_H 1

#include "dogma.h"
#include "dogma_internal.h"
#include "modifier.h"
#include "operands.h"

/* -------- General -------- */

#define DOGMA_CTXTYPE_UNDEFINED 255
#define DOGMA_CTXTYPE_FLOAT 0
#define DOGMA_CTXTYPE_INT 1
#define DOGMA_CTXTYPE_ENV 2
#define DOGMA_CTXTYPE_STRING 3
#define DOGMA_CTXTYPE_ASSOCIATION 4
#define DOGMA_CTXTYPE_ATTRIBUTEID 5
#define DOGMA_CTXTYPE_TYPEID 6
#define DOGMA_CTXTYPE_GROUPID 7
#define DOGMA_CTXTYPE_BOOL 8
#define DOGMA_CTXTYPE_MODIFIER 9





/* -------- Data types -------- */

struct dogma_expctx_s {
	uint8_t type;

	union {
		double float_value;
		int int_value;
		bool bool_value;
		dogma_env_t* env_value;
		char* string_value;

		dogma_association_t assoc_value;
		attributeid_t attributeid_value;
		typeid_t typeid_value;
		groupid_t groupid_value;

		dogma_modifier_t modifier_value;
	};
};
typedef struct dogma_expctx_s dogma_expctx_t;





/* -------- Internal functions -------- */

/* Evaluate an expression. */
int dogma_eval_expression(dogma_context_t*, dogma_env_t*, expressionid_t, dogma_expctx_t*);





#endif
