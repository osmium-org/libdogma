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

#include "attribute.h"
#include "tables.h"

#define ATT_SkillLevel 280

#define ATT_RequiredSkill1 182
#define ATT_RequiredSkill2 183
#define ATT_RequiredSkill3 184
#define ATT_RequiredSkill4 1285
#define ATT_RequiredSkill5 1289
#define ATT_RequiredSkill6 1290

static int dogma_apply_modifier(dogma_context_t*, dogma_env_t*, dogma_modifier_t*, double*);

/* FIXME: this does not take stacking penalties into account */
int dogma_get_env_attribute(dogma_context_t* ctx, dogma_env_t* env, attributeid_t attributeid, double* out) {
	dogma_env_t* current_env;
	array_t* modifiers;
	dogma_modifier_t** modifier;
	key_t index;

	if(attributeid == ATT_SkillLevel) {
		/* Special case: assume we want the skill level of a skill */
		uint8_t* level;

		JLG(level, ctx->skill_levels, env->id);
		if(level == NULL) {
			*out = (double)ctx->default_skill_level;
		} else {
			*out = (double)(*level);
		}

		return DOGMA_OK;
	}

	DOGMA_ASSUME_OK(dogma_get_type_attribute(env->id, attributeid, out));

	for(dogma_association_t assoctype = DOGMA_PreAssignment; assoctype <= DOGMA_PostAssignment; ++assoctype) {
		current_env = env;
		while(current_env != NULL) {
			JLG(modifiers, current_env->modifiers, attributeid);
			if(modifiers != NULL) {
				JLG(modifiers, *modifiers, assoctype);
				if(modifiers != NULL) {
					index = 0;
					JLF(modifier, *modifiers, index);
					while(modifier != NULL) {
						DOGMA_ASSUME_OK(dogma_apply_modifier(ctx, env, *modifier, out));

						JLN(modifier, *modifiers, index);
					}
				}
			}

			current_env = current_env->parent;
		}
	}

	return DOGMA_OK;
}

#define DOGMA_CHECK_SKILL_ATTRIBUTE(attid) \
	DOGMA_ASSUME_OK(dogma_get_env_attribute(ctx, env, attid, &value)); \
	if(value == skillid) { *out = true; return DOGMA_OK; }

int dogma_env_requires_skill(dogma_context_t* ctx, dogma_env_t* env, typeid_t skillid, bool* out) {
	double value;

	DOGMA_CHECK_SKILL_ATTRIBUTE(ATT_RequiredSkill1)
	DOGMA_CHECK_SKILL_ATTRIBUTE(ATT_RequiredSkill2)
	DOGMA_CHECK_SKILL_ATTRIBUTE(ATT_RequiredSkill3)
	DOGMA_CHECK_SKILL_ATTRIBUTE(ATT_RequiredSkill4)
	DOGMA_CHECK_SKILL_ATTRIBUTE(ATT_RequiredSkill5)
	DOGMA_CHECK_SKILL_ATTRIBUTE(ATT_RequiredSkill6)	

	*out = false;
	return DOGMA_OK;
}

static int dogma_apply_modifier(dogma_context_t* ctx, dogma_env_t* env, dogma_modifier_t* modifier, double* out) {
	const dogma_type_t* t;
	bool required;
	double value;

	switch(modifier->filter.type) {

	case DOGMA_FILTERTYPE_PASS:
		break;

	case DOGMA_FILTERTYPE_GROUP:
		DOGMA_ASSUME_OK(dogma_get_type(env->id, &t));
		if(t->groupid != modifier->filter.groupid) return DOGMA_OK;
		break;

	case DOGMA_FILTERTYPE_SKILL_REQUIRED:
		DOGMA_ASSUME_OK(dogma_env_requires_skill(ctx, env, modifier->filter.typeid, &required));
		if(!required) return DOGMA_OK;
		break;

	}

	DOGMA_ASSUME_OK(dogma_get_env_attribute(ctx, modifier->sourceenv, modifier->sourceattribute, &value));

	switch(modifier->assoctype) {

	case DOGMA_PreAssignment:
	case DOGMA_PostAssignment:
		*out = value;
		break;

	case DOGMA_PreMul:
	case DOGMA_PostMul:
		*out *= value;
		break;

	case DOGMA_PreDiv:
	case DOGMA_PostDiv:
		*out /= value;
		break;

	case DOGMA_ModAdd:
		*out += value;
		break;

	case DOGMA_ModSub:
		*out -= value;
		break;

	case DOGMA_PostPercent:
		*out *= (100 + value);
		*out /= 100;
		break;

	}

	return DOGMA_OK;
}
