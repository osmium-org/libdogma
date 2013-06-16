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

#include <assert.h>
#include "modifier.h"
#include "tables.h"

#define EFF_LoPower 11
#define EFF_HiPower 12
#define EFF_MedPower 13
#define EFF_RigSlot 2663

static bool dogma_modifier_is_penalized(dogma_modifier_t*);

static bool dogma_modifier_is_penalized(dogma_modifier_t* modifier) {
	const dogma_attribute_t* attribute;
	const dogma_type_effect_t* te;

	if(!((1 << modifier->assoctype) & DOGMA_PENALIZABLE_ASSOCTYPES)) {
		/* Assoctype is not penalizable */
		return false;
	}

	assert(dogma_get_attribute(modifier->targetattribute, &attribute) == DOGMA_OK);

	if(attribute->stackable) {
		/* Attribute is not penalized */
		return false;
	}

	if(dogma_get_type_effect(modifier->sourceenv->id, EFF_LoPower, &te) == DOGMA_OK
	|| dogma_get_type_effect(modifier->sourceenv->id, EFF_HiPower, &te) == DOGMA_OK
	|| dogma_get_type_effect(modifier->sourceenv->id, EFF_MedPower, &te) == DOGMA_OK
	|| dogma_get_type_effect(modifier->sourceenv->id, EFF_RigSlot, &te) == DOGMA_OK) {
		return true;
	}

	return false;
}

int dogma_add_modifier(dogma_modifier_t* modifier) {
	key_t index = 0;
	int result;
	array_t* modifiers_by_assoctype;
	array_t* modifiers;
	dogma_modifier_t** modifier_value;
	dogma_modifier_t* copy;

	modifier->penalized = dogma_modifier_is_penalized(modifier);

	copy = malloc(sizeof(dogma_modifier_t));
	*copy = *modifier;

	JLG(modifiers_by_assoctype, modifier->targetenv->modifiers, modifier->targetattribute);
	if(modifiers_by_assoctype == NULL) {
		JLI(modifiers_by_assoctype, modifier->targetenv->modifiers, modifier->targetattribute);
	}

	JLG(modifiers, *modifiers_by_assoctype, modifier->assoctype);
	if(modifiers == NULL) {
		JLI(modifiers, *modifiers_by_assoctype, modifier->assoctype);
	}

	JLFE(result, *modifiers, index);
	JLI(modifier_value, *modifiers, index);
	*modifier_value = copy;

	return DOGMA_OK;
}

int dogma_remove_modifier(dogma_modifier_t* modifier) {
	key_t index = 0;
	int result;
	array_t* modifiers_by_assoctype;
	array_t* modifiers;
	dogma_modifier_t** modifier_value;

	JLG(modifiers_by_assoctype, modifier->targetenv->modifiers, modifier->targetattribute);
	assert(modifiers_by_assoctype != NULL);

	JLG(modifiers, *modifiers_by_assoctype, modifier->assoctype);
	assert(modifiers != NULL);

	JLF(modifier_value, *modifiers, index);
	while(modifier_value != NULL) {
		if((*modifier_value)->sourceattribute == modifier->sourceattribute
		   && (*modifier_value)->sourceenv == modifier->sourceenv
		   && (*modifier_value)->scope == modifier->scope
			&&(*modifier_value)->filter.type == modifier->filter.type
		   && (modifier->filter.type != DOGMA_FILTERTYPE_GROUP
		       || (*modifier_value)->filter.groupid == modifier->filter.groupid)
		   && (modifier->filter.type != DOGMA_FILTERTYPE_SKILL_REQUIRED
		       || (*modifier_value)->filter.typeid == modifier->filter.typeid)) {
			free(*modifier_value);
			JLD(result, *modifiers, index);
			assert(result == 1);

			/* Eventually free the nested arrays if they are empty */
			JLC(result, *modifiers, 0, -1);
			if(result == 0) {
				JLFA(result, *modifiers);
				JLD(result, *modifiers_by_assoctype, modifier->assoctype);
				assert(result == 1);

				JLC(result, *modifiers_by_assoctype, 0, -1);
				if(result == 0) {
					JLFA(result, *modifiers_by_assoctype);
					JLD(result, modifier->targetenv->modifiers, modifier->targetattribute);
					assert(result == 1);
				}
			}

			return DOGMA_OK;
		} else {
			JLN(modifier_value, *modifiers, index);
		}
	}

	return DOGMA_NOT_FOUND;
}
