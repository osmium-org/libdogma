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

#include "dogma.h"
#include "dogma_internal.h"
#include "modifier.h"

int dogma_dump_modifiers(dogma_env_t* env) {
	key_t index = 0, index2, index3;
	array_t* modifiers_by_assoctype;
	array_t* modifiers;
	dogma_modifier_t** modifier;

	printf("modifiers of %p, type %i\n", env, env->id);
	printf("=========================================\n");

	JLF(modifiers_by_assoctype, env->modifiers, index);
	while(modifiers_by_assoctype != NULL) {
		printf("attribute %li:\n", index);

		index2 = 0;
		JLF(modifiers, *modifiers_by_assoctype, index2);
		while(modifiers != NULL) {
			printf("    assoc %li:\n", index2);

			index3 = 0;
			JLF(modifier, *modifiers, index3);
			while(modifier != NULL) {
				printf("        ");

				switch((*modifier)->filter.type) {
				case DOGMA_FILTERTYPE_PASS:
					printf("(filter: all)");
					break;
				case DOGMA_FILTERTYPE_GROUP:
					printf("(filter: group %i)", (*modifier)->filter.groupid);
					break;
				case DOGMA_FILTERTYPE_SKILL_REQUIRED:
					printf("(filter: requires skill %i)", (*modifier)->filter.typeid);
					break;
				}

				printf(" source attribute %hi of env %p, type %i\n",
				       (*modifier)->sourceattribute,
				       (*modifier)->sourceenv,
				       (*modifier)->sourceenv->id);

				JLN(modifier, *modifiers, index3);
			}

			JLN(modifiers, *modifiers_by_assoctype, index2);
		}

		JLN(modifiers_by_assoctype, env->modifiers, index);
	}

	return DOGMA_OK;
}
