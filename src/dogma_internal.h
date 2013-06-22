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

#ifndef _DOGMA_INTERNAL_H
#define _DOGMA_INTERNAL_H 1

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <Judy.h>

#include "operands.h"

/* -------- General -------- */

#define DOGMA_WARN(format, ...) fprintf(stderr, "%s: %s:%i: " format "\n", \
                                        PACKAGE_NAME, __FILE__, __LINE__, __VA_ARGS__)

#define DOGMA_ASSUME_OK(RESULT)	  \
	{ int call_result__ = RESULT; if(call_result__ != DOGMA_OK) return call_result__; }

#define DOGMA_INIT_ENV(envptr, _typeid, _parent, _index, _owner) do {	  \
		(envptr)->id = (_typeid); \
		(envptr)->parent = (_parent); \
		(envptr)->owner = (_owner); \
		(envptr)->index = (_index); \
		(envptr)->state = 0; \
		(envptr)->children = NULL; \
		(envptr)->modifiers = NULL; \
		(envptr)->chance_effects = NULL; \
	} while(0)

#define DOGMA_SAFE_CHAR_INDEXES 50000
#define DOGMA_SAFE_SHIP_INDEXES 0

_Static_assert(sizeof(key_t) >= sizeof(effectid_t), "Must be able to reliably use an effectid as an array index");
_Static_assert(sizeof(key_t) >= sizeof(typeid_t), "Must be able to reliably use a typeid as an array index");
_Static_assert(sizeof(Word_t) >= sizeof(void*), "Must be able to use a pointer as array value");





/* -------- Return codes -------- */

/* NOTE: only include here return codes used by internal
 * functions. Public return codes (values which may be returned by
 * exported methods) should go in dogma.h. */

/* Used by dogma_apply_modifier() when the modifier was not applied
 * because it did not match some filters. */
#define DOGMA_SKIPPED -1





/* -------- Data types -------- */

typedef uint16_t groupid_t;
typedef uint8_t  categoryid_t;
typedef int32_t expressionid_t;

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
	attributeid_t fittingusagechanceattributeid;
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

/* Different association types, sorted by evaluation order (sort of
 * like operator precedence).
 *
 * WARNING: if you change this, take a look in attribute.c, the enum
 * is traversed sequentially: make sure the new bounds match! */
enum dogma_association_e {
	DOGMA_ASSOC_PreAssignment,
	DOGMA_ASSOC_PreMul,
	DOGMA_ASSOC_PreDiv,
	DOGMA_ASSOC_ModAdd,
	DOGMA_ASSOC_ModSub,
	DOGMA_ASSOC_PostMul,
	DOGMA_ASSOC_PostDiv,
	DOGMA_ASSOC_PostPercent,
	DOGMA_ASSOC_PostAssignment,
};
typedef enum dogma_association_e dogma_association_t;

enum dogma_env_index_e {
	DOGMA_ENVIDX_Self,
	DOGMA_ENVIDX_Char,
	DOGMA_ENVIDX_Ship,
	DOGMA_ENVIDX_Target,
	DOGMA_ENVIDX_Area,
	DOGMA_ENVIDX_Other,
};
typedef enum dogma_env_index_e dogma_env_index_t;

struct dogma_expression_s {
	expressionid_t id;
	dogma_operand_t operand;
	expressionid_t arg1;
	expressionid_t arg2;

	union {
		groupid_t groupid;
		attributeid_t attributeid;
		typeid_t typeid;
		dogma_association_t assoctype;
		dogma_env_index_t envidx;
		double floatv;
		int intv;
		bool boolv;
		char* value;
	};
};
typedef struct dogma_expression_s dogma_expression_t;

struct dogma_env_s {
	typeid_t id;
	struct dogma_env_s* parent; /* Also known as location in dogma terminology */
	struct dogma_env_s* owner;
	key_t index; /* Index in parent->children array */
	state_t state;
	array_t children;
	array_t modifiers;
	array_t chance_effects;
};
typedef struct dogma_env_s dogma_env_t;

struct dogma_drone_context_s {
	dogma_env_t* drone;
	unsigned int quantity;
};
typedef struct dogma_drone_context_s dogma_drone_context_t;

struct dogma_context_s {
	dogma_env_t* character;

	/* The ship is actually a child of character, and it has index
	 * 0. This pointer is just an alias, for convenience. */
	dogma_env_t* ship;

	/* These two are unused for now and should always be NULL. */
	dogma_env_t* target;
	dogma_env_t* area;

	uint8_t default_skill_level;
	array_t skill_levels;

	/* Drones are children of character, with unpredictable
	 * indexes. This is a map where keys are drone typeids, and the
	 * values are pointers to dogma_drone_context_t. */
	array_t drone_map;
};

/* -------- Internal functions -------- */

/* Free an environment, including its modifiers and children,
 * recursively. This function is not smart and will not remove
 * obsolete modifiers on parents! */
int dogma_free_env(dogma_env_t*);

/* Set state of an environment and evaluate needed expressions based
 * on effect categories. */
int dogma_set_env_state(dogma_context_t*, dogma_env_t*, state_t);

/* Dump the modifiers of an environment to stdout. */
int dogma_dump_modifiers(dogma_env_t*);

/* Inject a skill in character. */
int dogma_inject_skill(dogma_context_t*, typeid_t);

#endif
