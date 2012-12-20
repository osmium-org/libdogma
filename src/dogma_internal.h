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

#ifndef _DOGMA_INTERNAL_H
#define _DOGMA_INTERNAL_H 1

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <Judy.h>

/* -------- Data types -------- */

typedef uint16_t groupid_t;
typedef uint8_t  categoryid_t;
typedef uint16_t effectid_t;
typedef uint16_t expressionid_t;
typedef uint8_t  operandid_t;
typedef Pvoid_t array_t;
typedef Word_t key_t;

struct dogma_type_s {
	typeid_t id;
	double volume;
	double mass;
	double capacity;
	groupid_t groupid;
	categoryid_t categoryid;
};
typedef struct dogma_type_s dogma_type_t;

struct dogma_attribute_s {
	attributeid_t id;
	double defaultvalue;
	bool stackable;
	bool highisgood;
};
typedef struct dogma_attribute_s dogma_attribute_t;

struct dogma_effect_s {
	effectid_t id;
	uint8_t category;
	expressionid_t preexpressionid;
	expressionid_t postexpressionid;
	attributeid_t durationattributeid;
	attributeid_t trackingspeedattributeid;
	attributeid_t dischargeattributeid;
	attributeid_t rangeattributeid;
	attributeid_t falloffattributeid;
};
typedef struct dogma_effect_s dogma_effect_t;

struct dogma_type_attribute_s {
	typeid_t typeid;
	attributeid_t attributeid;
	double value;
};
typedef struct dogma_type_attribute_s dogma_type_attribute_t;

struct dogma_type_effect_s {
	typeid_t typeid;
	effectid_t effectid;
};
typedef struct dogma_type_effect_s dogma_type_effect_t;

struct dogma_expression_s {
	expressionid_t id;
	operandid_t operandid;
	expressionid_t arg1;
	expressionid_t arg2;

	union {
		groupid_t groupid;
		attributeid_t attributeid;
		typeid_t typeid;
		char* value;
	};
};
typedef struct dogma_expression_s dogma_expression_t;

struct dogma_env_s {
	typeid_t id;
	array_t children;
	array_t modifiers;
};
typedef struct dogma_env_s dogma_env_t;

struct dogma_context_s {
	dogma_env_t character;
	dogma_env_t ship;
	dogma_env_t* self;
	dogma_env_t* other;
	dogma_env_t* target;
	dogma_env_t* area;

	uint8_t default_skill_level;
	array_t skillpoints;
};

#endif
