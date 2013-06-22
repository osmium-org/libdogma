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

#include "attribute.h"
#include "tables.h"
#include <assert.h>

#define ATT_SkillLevel 280

#define ATT_RequiredSkill1 182
#define ATT_RequiredSkill2 183
#define ATT_RequiredSkill3 184
#define ATT_RequiredSkill4 1285
#define ATT_RequiredSkill5 1289
#define ATT_RequiredSkill6 1290

#define MAX_PENALIZED_MODIFIERS 16

/* Values taken from Aenigma's guide:
 * http://eve.battleclinic.com/guide/9196-Aenigma-s-Stacking-Penalty-Guide.html */
static double penalized_coefficients[] = {
	1.000000000000,
	0.869119980800,
	0.570583143511,
	0.282955154023,
	0.105992649743,
	0.029991166533,
	0.006410183118,
	0.001034920483,
	0.000126212683,
	0.000011626754,
	0.000000809046,
	0.0,
};

static int dogma_apply_modifier(dogma_context_t*, dogma_env_t*, dogma_modifier_t*, double*);
static int dogma_apply_modifiers_from_env(dogma_context_t*, array_t, dogma_env_t*,
                                          double* penalized_positive, double* penalized_negative,
                                          size_t* penalized_pos_count, size_t* penalized_neg_count,
                                          double*);

static int compare(const void* left, const void* right) {
	return *(double*)left < *(double*)right ? -1 : 1;
}
static int rcompare(const void* left, const void* right) {
	return *(double*)left < *(double*)right ? 1 : -1;
}

int dogma_get_env_attribute(dogma_context_t* ctx, dogma_env_t* env, attributeid_t attributeid, double* out) {
	const dogma_attribute_t* attr;
	dogma_env_t* current_env;
	array_t* modifiers;

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

	DOGMA_ASSUME_OK(dogma_get_attribute(attributeid, &attr));
	DOGMA_ASSUME_OK(dogma_get_type_attribute(env->id, attributeid, out));

	for(dogma_association_t assoctype = DOGMA_ASSOC_PreAssignment;
	    assoctype <= DOGMA_ASSOC_PostAssignment;
	    ++assoctype) {
		double penalized_positive[MAX_PENALIZED_MODIFIERS];
		double penalized_negative[MAX_PENALIZED_MODIFIERS];
		size_t penalized_pos_count = 0, penalized_neg_count = 0;

		current_env = env;
		while(current_env != NULL) {
			JLG(modifiers, current_env->modifiers, attributeid);

			if(modifiers != NULL) {
				JLG(modifiers, *modifiers, assoctype);

				if(modifiers != NULL) {
					DOGMA_ASSUME_OK(dogma_apply_modifiers_from_env(
						ctx, *modifiers, env,
						penalized_positive, penalized_negative,
						&penalized_pos_count, &penalized_neg_count,
						out
					));
				}
			}

			current_env = current_env->parent;
		}

		if(penalized_pos_count == 0 && penalized_neg_count == 0) continue;

		if(attr->highisgood) {
			qsort(penalized_positive, penalized_pos_count, sizeof(double), rcompare);
			qsort(penalized_negative, penalized_neg_count, sizeof(double), rcompare);
		} else {
			qsort(penalized_positive, penalized_pos_count, sizeof(double), compare);
			qsort(penalized_negative, penalized_neg_count, sizeof(double), compare);
		}

		for(size_t i = 0; i < penalized_pos_count; ++i) {
			double p = penalized_coefficients[i];
			if(p == 0.0) break;

			*out *= (1 + p * penalized_positive[i]);
		}

		for(size_t i = 0; i < penalized_neg_count; ++i) {
			double p = penalized_coefficients[i];
			if(p == 0.0) break;

			*out *= (1 + p * penalized_negative[i]);
		}
	}

	return DOGMA_OK;
}

#define DOGMA_CHECK_SKILL_ATTRIBUTE(attid) do {	  \
		double value; \
		DOGMA_ASSUME_OK(dogma_get_env_attribute(ctx, env, attid, &value)); \
		if(value == skillid) { \
			*out = true; \
			return DOGMA_OK; \
		} \
	} while(0)

int dogma_env_requires_skill(dogma_context_t* ctx, dogma_env_t* env, typeid_t skillid, bool* out) {
	DOGMA_CHECK_SKILL_ATTRIBUTE(ATT_RequiredSkill1);
	DOGMA_CHECK_SKILL_ATTRIBUTE(ATT_RequiredSkill2);
	DOGMA_CHECK_SKILL_ATTRIBUTE(ATT_RequiredSkill3);
	DOGMA_CHECK_SKILL_ATTRIBUTE(ATT_RequiredSkill4);
	DOGMA_CHECK_SKILL_ATTRIBUTE(ATT_RequiredSkill5);
	DOGMA_CHECK_SKILL_ATTRIBUTE(ATT_RequiredSkill6);

	*out = false;
	return DOGMA_OK;
}

static int dogma_apply_modifiers_from_env(dogma_context_t* ctx, array_t modifiers, dogma_env_t* env,
                                          double* penalized_positive, double* penalized_negative,
                                          size_t* penalized_pos_count, size_t* penalized_neg_count,
                                          double* out) {
	dogma_modifier_t** modifier;
	key_t index = 0;
	int ret;

	JLG(modifier, modifiers, index);
	while(modifier != NULL) {
		if((*modifier)->penalized) {
			/* NB: assumed here that all penalized modifiers are for
			 * multiplicative operations. This is currently the case
			 * (ModAdd/ModSub are never penalized), but if this
			 * changes in the future this could become a serious
			 * problem. */
			double v = 1.0;
			ret = dogma_apply_modifier(ctx, env, *modifier, &v);
			if(ret == DOGMA_OK) {
				v -= 1.0;

				if(v >= 0) {
					if(*penalized_pos_count >= MAX_PENALIZED_MODIFIERS) {
						DOGMA_WARN(
							"reached maximum amount of + penalized modifiers, ignoring %f",
							v
						);
					} else {
						penalized_positive[(*penalized_pos_count)++] = v;
					}
				} else {
					if(*penalized_neg_count >= MAX_PENALIZED_MODIFIERS) {
						DOGMA_WARN(
							"reached maximum amount of - penalized modifiers, ignoring %f",
							v
						);
					} else {
						penalized_negative[(*penalized_neg_count)++] = v;
					}
				}
			} else {
				assert(ret == DOGMA_SKIPPED);
			}
		} else {
			ret = dogma_apply_modifier(ctx, env, *modifier, out);
			assert(ret == DOGMA_OK || ret == DOGMA_SKIPPED);
		}

		JLN(modifier, modifiers, index);
	}

	return DOGMA_OK;
}

static int dogma_apply_modifier(dogma_context_t* ctx, dogma_env_t* env,
                                dogma_modifier_t* modifier, double* out) {
	const dogma_type_t* t;
	bool required;
	double value;

	switch(modifier->scope) {

	case DOGMA_SCOPE_Item:
		/* Item modifiers (added with AIM and similar operands) only
		 * affect the target environment, not its children */
		if(env != modifier->targetenv) return DOGMA_SKIPPED;
		break;

	case DOGMA_SCOPE_Location:
		/* Location modifiers only affect environments with a certain
		 * location (parent) */
		if(env->parent != modifier->targetenv) return DOGMA_SKIPPED;
		break;

	case DOGMA_SCOPE_Owner:
		/* Owner modifiers (added with AORSM etc.) only affect
		 * environments owned by the same character */
		if(env->owner != modifier->sourceenv->owner) return DOGMA_SKIPPED;
		break;

	}

	switch(modifier->filter.type) {

	case DOGMA_FILTERTYPE_PASS:
		break;

	case DOGMA_FILTERTYPE_GROUP:
		if(env->id == 0) return DOGMA_SKIPPED;
		DOGMA_ASSUME_OK(dogma_get_type(env->id, &t));
		if(t->groupid != modifier->filter.groupid) return DOGMA_SKIPPED;
		break;

	case DOGMA_FILTERTYPE_SKILL_REQUIRED:
		if(env->id == 0) return DOGMA_SKIPPED;
		DOGMA_ASSUME_OK(dogma_env_requires_skill(ctx, env, modifier->filter.typeid, &required));
		if(!required) return DOGMA_SKIPPED;
		break;

	}

	DOGMA_ASSUME_OK(dogma_get_env_attribute(ctx, modifier->sourceenv, modifier->sourceattribute, &value));

	switch(modifier->assoctype) {

	case DOGMA_ASSOC_PreAssignment:
	case DOGMA_ASSOC_PostAssignment:
		*out = value;
		break;

	case DOGMA_ASSOC_PreMul:
	case DOGMA_ASSOC_PostMul:
		*out *= value;
		break;

	case DOGMA_ASSOC_PreDiv:
	case DOGMA_ASSOC_PostDiv:
		*out /= value;
		break;

	case DOGMA_ASSOC_ModAdd:
		*out += value;
		break;

	case DOGMA_ASSOC_ModSub:
		*out -= value;
		break;

	case DOGMA_ASSOC_PostPercent:
		*out *= (100 + value);
		*out /= 100;
		break;

	}

	return DOGMA_OK;
}
