/* libdogma
 * Copyright (C) 2012, 2013, 2015, 2016 Romain "Artefact2" Dalmaso <artefact2@gmail.com>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License
 * as published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public
 * License along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#pragma once
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
                                        __func__, __FILE__, __LINE__, __VA_ARGS__)

#define DOGMA_ASSUME_OK(RESULT)	  \
	{ int call_result__ = RESULT; if(call_result__ != DOGMA_OK) return call_result__; }

#define DOGMA_INIT_ENV(envptr, _typeid, _parent, _index, _owner) do {	  \
		(envptr)->id = (_typeid); \
		(envptr)->parent = (_parent); \
		(envptr)->owner = (_owner); \
		(envptr)->target.context = NULL; \
		(envptr)->target.env = NULL; \
		(envptr)->index = (_index); \
		(envptr)->state = 0; \
		(envptr)->children = NULL; \
		(envptr)->modifiers = NULL; \
		(envptr)->chance_effects = NULL; \
		(envptr)->targeted_by = NULL; \
	} while(0)

/* Index 0 is for char */
#define DOGMA_SAFE_AREA_INDEXES 1

/* Any skill typeid is an index */
#define DOGMA_SAFE_CHAR_INDEXES 50000

/* Nothing needs a reserved index in ships */
#define DOGMA_SAFE_SHIP_INDEXES 0





/* -------- Data types -------- */

_Static_assert(sizeof(dogma_key_t) >= sizeof(dogma_attributeid_t), "Must be able to use an attributeid as an array index");
_Static_assert(sizeof(dogma_key_t) >= sizeof(dogma_effectid_t), "Must be able to use an effectid as an array index");
_Static_assert(sizeof(dogma_key_t) >= sizeof(dogma_typeid_t), "Must be able to use a typeid as an array index");
_Static_assert(sizeof(dogma_key_t) >= sizeof(intptr_t), "Must be able to use a pointer as an array index");
_Static_assert(sizeof(Word_t) >= sizeof(void*), "Must be able to use a pointer as array value");



struct dogma_type_s {
	dogma_typeid_t id;
	double volume;
	double mass;
	double capacity;
	dogma_groupid_t groupid;
	dogma_categoryid_t categoryid;
};
typedef struct dogma_type_s dogma_type_t;



struct dogma_attribute_s {
	dogma_attributeid_t id;
	double defaultvalue;
	bool stackable;
	bool highisgood;
};
typedef struct dogma_attribute_s dogma_attribute_t;



struct dogma_effect_s {
	dogma_effectid_t id;
	uint8_t category;
	dogma_expressionid_t preexpressionid;
	dogma_expressionid_t postexpressionid;
	dogma_attributeid_t durationattributeid;
	dogma_attributeid_t trackingspeedattributeid;
	dogma_attributeid_t dischargeattributeid;
	dogma_attributeid_t rangeattributeid;
	dogma_attributeid_t falloffattributeid;
	dogma_attributeid_t fittingusagechanceattributeid;
};
typedef struct dogma_effect_s dogma_effect_t;



struct dogma_type_attribute_s {
	dogma_typeid_t typeid;
	dogma_attributeid_t attributeid;
	double value;
};
typedef struct dogma_type_attribute_s dogma_type_attribute_t;



struct dogma_type_effect_s {
	dogma_typeid_t typeid;
	dogma_effectid_t effectid;
};
typedef struct dogma_type_effect_s dogma_type_effect_t;



/* Different association types, sorted by evaluation order (sort of
 * like operator precedence).
 *
 * WARNING: if you change this, take a look in attribute.c and
 * extra.c, the enum is traversed sequentially: make sure the new
 * bounds match! */
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

struct dogma_operand_s {
	dogma_operandid_t id;
	const char* key;
	const char* description;
	const char* format;
};
typedef struct dogma_operand_s dogma_operand_t;

struct dogma_expression_s {
	dogma_expressionid_t id;
	dogma_operandid_t operandid;
	dogma_expressionid_t arg1;
	dogma_expressionid_t arg2;

	union {
		dogma_groupid_t groupid;
		dogma_attributeid_t attributeid;
		dogma_typeid_t typeid;
		dogma_association_t assoctype;
		dogma_env_index_t envidx;
		double floatv;
		int intv;
		bool boolv;
		char* value;
	};
};
typedef struct dogma_expression_s dogma_expression_t;



struct dogma_env_s;
typedef struct dogma_env_s dogma_env_t;

struct dogma_context_s;
typedef struct dogma_context_s dogma_context_t;

struct dogma_fleet_context_s;
typedef struct dogma_fleet_context_s dogma_fleet_context_t;

struct dogma_target_s {
	dogma_context_t* context;
	dogma_env_t* env;
};
typedef struct dogma_target_s dogma_target_t;

struct dogma_env_s {
	dogma_typeid_t id;
	dogma_env_t* parent; /* Also known as location in dogma terminology */
	dogma_context_t* owner;
	dogma_target_t target; /* context and env set to NULL if no target */
	dogma_key_t index; /* Index in parent->children array */
	dogma_state_t state;
	dogma_array_t children;
	dogma_array_t modifiers; /* targetattribute -> assoctype -> (index) -> modifier */
	dogma_array_t chance_effects;
	dogma_array_t targeted_by; /* dogma_env_t* -> dogma_context_t* */
};

struct dogma_drone_context_s {
	dogma_env_t* drone;
	unsigned int quantity;
};
typedef struct dogma_drone_context_s dogma_drone_context_t;

struct dogma_context_s {
	dogma_fleet_context_t* fleet; /* This is the subfleet this context
	                               * is a member of, so this is not
	                               * always the "root" fleet
	                               * context */

	dogma_env_t* gang; /* Where gang modifiers live */

	/* The root environment. Contains the character at index 0 and
	 * possibly more effect beacons. */
	dogma_env_t* area;

	/* The character piloting the ship. Contains the ship at index
	 * 0, skillbooks (index is the skill typeid), drones and
	 * implants as direct children. */
	dogma_env_t* character;

	/* The ship being piloted. Contains the modules… */
	dogma_env_t* ship;

	uint8_t default_skill_level;
	dogma_array_t skill_levels; /* dogma_typeid_t -> uint8_t */

	/* Drones are children of character, with unpredictable
	 * indexes. This is a map where keys are drone typeids, and the
	 * values are pointers to dogma_drone_context_t. */
	dogma_array_t drone_map;
};

struct dogma_fleet_context_s {
	dogma_context_t* commander;
	dogma_context_t* booster; /* Assumed to be a member of this fleet
	                           * context (or its subfleets) */

	dogma_fleet_context_t* parent;
	dogma_key_t index; /* Index in parent->subfleets, typically the
	                    * squad/wing number */
	dogma_array_t subfleets; /* Wings, squads, etc. */
	dogma_array_t members; /* Array of dogma_context_t* (keys are the same
	                        * as values to enforce uniqueness) */
};





/* -------- Internal functions -------- */

/* Free an environment, including its modifiers and children,
 * recursively. This function is not smart and will not remove
 * obsolete modifiers on parents! */
int dogma_free_env(dogma_context_t*, dogma_env_t*);

/* Set state of an environment and evaluate needed expressions based
 * on effect categories. */
int dogma_set_env_state(dogma_context_t*, dogma_env_t*, dogma_state_t);

/* Inject a skill in character. */
int dogma_inject_skill(dogma_context_t*, dogma_typeid_t);

/* Set a target. */
int dogma_set_target(dogma_context_t*, dogma_env_t*, dogma_context_t*, dogma_env_t*);

/* Get a dogma_env_t* based on its "simpler" dogma_location_t description. */
int dogma_get_location_env(dogma_context_t*, dogma_location_t, dogma_env_t**);

/* Toggle a chance based effect on or off. */
int dogma_toggle_chance_based_effect_env(dogma_context_t*, dogma_env_t*, dogma_effectid_t, bool);





#endif
