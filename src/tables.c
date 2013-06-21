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

#include "dogma.h"
#include "dogma_internal.h"
#include "tables.h"
#include "tables-data.h"

#define ATT_Mass 4
#define ATT_Capacity 38
#define ATT_Volume 161

array_t types_by_id = NULL;
array_t attributes_by_id = NULL;
array_t effects_by_id = NULL;
array_t expressions_by_id = NULL;
array_t type_attributes_by_typeid = NULL;
array_t type_effects_by_typeid = NULL;

#define DOGMA_INIT_GENERIC(NAME, TYPE, TABLE, INDEX, ARRAY)	  \
	static void dogma_init_ ## NAME(void) { \
		const TYPE ** value; \
		for(int i = 0; TABLE[i].INDEX != 0; ++i) { \
			JLI(value, ARRAY, TABLE[i].INDEX); \
			*value = &TABLE[i]; \
		} \
	}

DOGMA_INIT_GENERIC(types, dogma_type_t, dogma_table_types, id, types_by_id)
DOGMA_INIT_GENERIC(attributes, dogma_attribute_t, dogma_table_attributes, id, attributes_by_id)
DOGMA_INIT_GENERIC(effects, dogma_effect_t, dogma_table_effects, id, effects_by_id)
DOGMA_INIT_GENERIC(expressions, dogma_expression_t, dogma_table_expressions, id, expressions_by_id)

#define DOGMA_INIT_GENERIC_2D(NAME, TYPE, KEYTYPE1, TABLE, INDEX1, INDEX2, ARRAY) \
	static void dogma_init_ ## NAME(void) { \
		KEYTYPE1 id = 0; \
		array_t* nested; \
		const TYPE** value; \
		for(int i = 0; TABLE[i].INDEX1 != 0; ++i) { \
			if(TABLE[i].INDEX1 != id) { \
				JLI(nested, ARRAY, TABLE[i].INDEX1); \
				*nested = NULL; \
				id = TABLE[i].INDEX1; \
			} \
			JLI(value, *nested, TABLE[i].INDEX2); \
			*value = &TABLE[i]; \
		} \
	}

DOGMA_INIT_GENERIC_2D(type_attributes, dogma_type_attribute_t, typeid_t, \
                      dogma_table_type_attributes, typeid, attributeid, type_attributes_by_typeid)
DOGMA_INIT_GENERIC_2D(type_effects, dogma_type_effect_t, typeid_t, \
                      dogma_table_type_effects, typeid, effectid, type_effects_by_typeid)

void dogma_init_tables(void) {
	dogma_init_types();
	dogma_init_attributes();
	dogma_init_effects();
	dogma_init_expressions();
	dogma_init_type_attributes();
	dogma_init_type_effects();
}

#define DOGMA_GET_GENERIC(NAME, INDEXTYPE, TYPE, ARRAY)	  \
	int dogma_get_ ## NAME(INDEXTYPE id, const TYPE ** out) { \
		const TYPE ** value; \
		JLG(value, ARRAY, id); \
		if(value == NULL) return DOGMA_NOT_FOUND; \
		*out = *value; \
		return DOGMA_OK; \
	}

DOGMA_GET_GENERIC(type, typeid_t, dogma_type_t, types_by_id)
DOGMA_GET_GENERIC(attribute, attributeid_t, dogma_attribute_t, attributes_by_id)
DOGMA_GET_GENERIC(effect, effectid_t, dogma_effect_t, effects_by_id)
DOGMA_GET_GENERIC(expression, expressionid_t, dogma_expression_t, expressions_by_id)

int dogma_get_type_attributes(typeid_t id, array_t* out) {
	array_t* value;

	JLG(value, type_attributes_by_typeid, id);
	if(value != NULL) {
		*out = *value;
	} else {
		*out = NULL;
	}

	return DOGMA_OK;
}

int dogma_get_type_attribute(typeid_t tid, attributeid_t aid, double* out) {
	array_t type_attributes;
	const dogma_type_attribute_t** ta;

	if(aid == ATT_Mass) {
		const dogma_type_t* t;
		DOGMA_ASSUME_OK(dogma_get_type(tid, &t));

		*out = t->mass;
		return DOGMA_OK;
	} else if(aid == ATT_Capacity) {
		const dogma_type_t* t;
		DOGMA_ASSUME_OK(dogma_get_type(tid, &t));

		*out = t->capacity;
		return DOGMA_OK;
	} else if(aid == ATT_Volume) {
		const dogma_type_t* t;
		DOGMA_ASSUME_OK(dogma_get_type(tid, &t));

		*out = t->volume;
		return DOGMA_OK;
	}

	DOGMA_ASSUME_OK(dogma_get_type_attributes(tid, &type_attributes));

	JLG(ta, type_attributes, aid);
	if(ta == NULL) {
		/* Use the default attribute value */

		const dogma_attribute_t** a;

		JLG(a, attributes_by_id, aid);
		if(a == NULL) return DOGMA_NOT_FOUND;

		*out = (*a)->defaultvalue;
	} else {
		*out = (*ta)->value;
	}

	return DOGMA_OK;
}

int dogma_get_type_effects(typeid_t id, array_t* out) {
	array_t* value;

	JLG(value, type_effects_by_typeid, id);
	if(value != NULL) {
		*out = *value;
	} else {
		*out = NULL;
	}

	return DOGMA_OK;
}

int dogma_get_type_effect(typeid_t tid, effectid_t eid, const dogma_type_effect_t** out) {
	array_t type_effects;
	const dogma_type_effect_t** te;

	DOGMA_ASSUME_OK(dogma_get_type_effects(tid, &type_effects));

	JLG(te, type_effects, eid);
	if(te == NULL) {
		return DOGMA_NOT_FOUND;
	}

	*out = *te;
	return DOGMA_OK;
}
