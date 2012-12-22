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

#include <assert.h>
#include "modifier.h"

int dogma_add_modifier(dogma_modifier_t* modifier) {
	key_t index = 0;
	int result;
	array_t* modifiers_by_assoctype;
	array_t* modifiers;
	dogma_modifier_t** modifier_value;
	dogma_modifier_t* copy;

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
