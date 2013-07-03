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

#include "dogma-names.h"

#define MAX_PENALIZED_MODIFIERS 16

#define MULTIPLICATIVE_ASSOC_TYPES (	  \
	(1 << DOGMA_ASSOC_PreMul) | \
	(1 << DOGMA_ASSOC_PostMul) | \
	(1 << DOGMA_ASSOC_PostPercent) | \
	(1 << DOGMA_ASSOC_PreDiv) | \
	(1 << DOGMA_ASSOC_PostDiv)	\
)

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

static int dogma_apply_modifiers_from_env(dogma_context_t*, array_t, dogma_env_t*,
                                          double* penalized_positive, double* penalized_negative,
                                          size_t* penalized_pos_count, size_t* penalized_neg_count,
                                          bool highisgood, double* singleton_val,
                                          int* singleton_penalized, double* out);

static int compare(const void* left, const void* right) {
	return *(double*)left < *(double*)right ? -1 : 1;
}
static int rcompare(const void* left, const void* right) {
	return *(double*)left < *(double*)right ? 1 : -1;
}

int dogma_get_env_attribute(dogma_context_t* ctx, dogma_env_t* env, attributeid_t attributeid, double* out) {
	const dogma_attribute_t* attr;
	dogma_env_t* current_env;
	dogma_fleet_context_t* fleet;
	array_t* modifiers;
	double penalized_positive[MAX_PENALIZED_MODIFIERS];
	double penalized_negative[MAX_PENALIZED_MODIFIERS];
	size_t penalized_pos_count, penalized_neg_count;
	double singleton_val;
	int singleton_penalized;

	if(attributeid == ATT_SkillLevel) {
		/* Special case: assume we want the skill level of a skill */
		uint8_t* level;

		JLG(level, env->owner->skill_levels, env->id);
		if(level == NULL) {
			*out = (double)env->owner->default_skill_level;
		} else {
			*out = (double)(*level);
		}

		return DOGMA_OK;
	}

	DOGMA_ASSUME_OK(dogma_get_attribute(attributeid, &attr));
	DOGMA_ASSUME_OK(dogma_get_type_attribute(env->id, attributeid, out));

	/* Code below is somewhat similar to the traverse code in extra.c */

	for(dogma_association_t assoctype = DOGMA_ASSOC_PreAssignment;
	    assoctype <= DOGMA_ASSOC_PostAssignment;
	    ++assoctype) {
		singleton_val = 1.0;
		singleton_penalized = -1; /* Unknown or bool value */
		penalized_pos_count = 0;
		penalized_neg_count = 0;

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
						attr->highisgood, &singleton_val,
						&singleton_penalized, out
					));
				}
			}

			current_env = current_env->parent;
		}

		fleet = ctx->fleet;
		while(fleet != NULL) {
			if(fleet->booster == NULL || fleet->commander == NULL) {
				break;
			}

			JLG(modifiers, fleet->booster->gang->modifiers, attributeid);

			if(modifiers != NULL) {
				JLG(modifiers, *modifiers, assoctype);

				if(modifiers != NULL) {
					DOGMA_ASSUME_OK(dogma_apply_modifiers_from_env(
						ctx, *modifiers, env,
						penalized_positive, penalized_negative,
						&penalized_pos_count, &penalized_neg_count,
						attr->highisgood, &singleton_val,
						&singleton_penalized, out
					));
				}
			}

			fleet = fleet->parent;
		}

		if(singleton_penalized) {
			if(singleton_val >= 1.0) {
				if(penalized_pos_count >= MAX_PENALIZED_MODIFIERS) {
					DOGMA_WARN(
						"reached maximum amount of + penalized modifiers, ignoring singleton %f",
						singleton_val
					);
				} else {
					penalized_positive[penalized_pos_count++] = singleton_val - 1.0;
				}
			} else {
				if(penalized_neg_count >= MAX_PENALIZED_MODIFIERS) {
					DOGMA_WARN(
						"reached maximum amount of - penalized modifiers, ignoring singleton %f",
						singleton_val
					);
				} else {
					penalized_negative[penalized_neg_count++] = singleton_val - 1.0;
				}
			}
		} else {
			*out *= singleton_val;
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
                                          bool highisgood, double* singleton_val,
                                          int* singleton_penalized, double* out) {
	dogma_modifier_t** modifier;
	key_t index = 0;
	bool applicable;

	JLG(modifier, modifiers, index);
	while(modifier != NULL) {
		DOGMA_ASSUME_OK(dogma_modifier_is_applicable(ctx, env, *modifier, &applicable));
		if(!applicable) {
			JLN(modifier, modifiers, index);
			continue;
		}

		if((*modifier)->singleton) {
			assert((MULTIPLICATIVE_ASSOC_TYPES >> (*modifier)->assoctype) & 1);

			double v = 1.0;
			DOGMA_ASSUME_OK(dogma_apply_modifier(ctx, *modifier, &v));

			if(*singleton_penalized == -1) {
				*singleton_val = v;
				*singleton_penalized = (*modifier)->penalized;
			} else {
				if(highisgood) {
					if(v > *singleton_val) *singleton_val = v;
				} else {
					if(v < *singleton_val) *singleton_val = v;
				}

				if(*singleton_penalized != (*modifier)->penalized) {
					DOGMA_WARN(
						"singleton modifiers have different penalized values, attribute %i on type %i",
						(*modifier)->targetattribute,
						(*modifier)->targetenv->id
					);
				}
			}
		} else if((*modifier)->penalized) {
			assert((MULTIPLICATIVE_ASSOC_TYPES >> (*modifier)->assoctype) & 1);

			double v = 1.0;
			DOGMA_ASSUME_OK(dogma_apply_modifier(ctx, *modifier, &v));
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
			DOGMA_ASSUME_OK(dogma_apply_modifier(ctx, *modifier, out));
		}

		JLN(modifier, modifiers, index);
	}

	return DOGMA_OK;
}
