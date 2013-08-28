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
#include "dogma.h"
#include "dogma_internal.h"
#include "modifier.h"
#include "tables.h"
#include "eval.h"

int dogma_free_env(dogma_context_t* ctx, dogma_env_t* env) {
	int ret;
	key_t index = -1, index2, index3;
	dogma_env_t** child;
	array_t* modifiers;
	array_t* modifiers2;
	dogma_modifier_t** modifier;

	/* Clear our own target */
	if(env->target.context != NULL) {
		assert(dogma_set_target(ctx, env, NULL, NULL) == DOGMA_OK);
	}

	/* Clear any targets of things that have what we're about do
	 * delete as a target */
	if(env->targeted_by != NULL) {
		key_t index = 0;
		dogma_context_t** targeter;
		dogma_env_t* source;

		JLF(targeter, env->targeted_by, index);
		while(targeter != NULL) {
			source = (dogma_env_t*)index;
			assert(dogma_set_target(*targeter, source, NULL, NULL) == DOGMA_OK);

			JLN(targeter, env->targeted_by, index);
		}

		JLC(ret, env->targeted_by, 0, -1);
		assert(ret == 0);
		JLFA(ret, env->targeted_by);
		assert(ret == 0);
	}

	/* Clear any chance-based effects */
	if(env->chance_effects != NULL) {
		key_t effectid = 0;
		bool* val;

		JLF(val, env->chance_effects, effectid);
		while(val != NULL) {
			DOGMA_ASSUME_OK(dogma_toggle_chance_based_effect_env(ctx, env, effectid, false));
			JLN(val, env->chance_effects, effectid);
		}

		JLFA(ret, env->chance_effects);
	}

	JLL(child, env->children, index);
	while(child != NULL) {
		dogma_free_env(ctx, *child);
		JLP(child, env->children, index);
	}
	JLFA(ret, env->children);

	index = 0;
	JLF(modifiers, env->modifiers, index);
	while(modifiers != NULL) {
		index2 = 0;
		JLF(modifiers2, *modifiers, index2);
		while(modifiers2 != NULL) {
			index3 = 0;
			JLF(modifier, *modifiers2, index3);
			while(modifier != NULL) {
				free(*modifier);
				JLN(modifier, *modifiers2, index3);
			}

			JLFA(ret, *modifiers2);
			JLN(modifiers2, *modifiers, index2);
		}

		JLFA(ret, *modifiers);
		JLN(modifiers, env->modifiers, index);
	}
	JLFA(ret, env->modifiers);

	free(env);

	return DOGMA_OK;
}

int dogma_set_env_state(dogma_context_t* ctx, dogma_env_t* env, state_t newstate) {
	array_t enveffects;
	key_t index = 0;
	const dogma_type_effect_t** te;
	const dogma_effect_t* e;
	dogma_expctx_t result;

	if(env->state == newstate) return DOGMA_OK;

	DOGMA_ASSUME_OK(dogma_get_type_effects(env->id, &enveffects));
	JLF(te, enveffects, index);
	while(te != NULL) {
		DOGMA_ASSUME_OK(dogma_get_effect((*te)->effectid, &e));
		JLN(te, enveffects, index);

		if(e->fittingusagechanceattributeid > 0) {
			/* Effect is chance-based */

			if(newstate > 0) {
				continue;
			}

			/* When unplugging the environment, turn off all
			 * chance-based effects as a precautionary measure */
			bool* v;
			int ret;
			JLG(v, env->chance_effects, e->id);
			if(v != NULL) {
				assert(*v == true);
				JLD(ret, env->chance_effects, e->id);
				DOGMA_ASSUME_OK(dogma_eval_expression(
					ctx, env,
					e->postexpressionid,
					&result
				));
			}

			continue;
		}

		if((newstate >> e->category) & 1) {
			if(!((env->state >> e->category) & 1)) {
				DOGMA_ASSUME_OK(dogma_eval_expression(
					ctx, env,
					e->preexpressionid,
					&result
				));
			}
		} else if((env->state >> e->category) & 1) {
			DOGMA_ASSUME_OK(dogma_eval_expression(
				ctx, env,
				e->postexpressionid,
				&result
			));
		}
	}

	env->state = newstate;

	return DOGMA_OK;
}

int dogma_inject_skill(dogma_context_t* ctx, typeid_t skillid) {
	dogma_env_t* skill_env = malloc(sizeof(dogma_env_t));
	dogma_env_t** value;

	JLI(value, ctx->character->children, skillid);
	*value = skill_env;

	DOGMA_INIT_ENV(skill_env, skillid, ctx->character, skillid, ctx);

	assert(skillid < DOGMA_SAFE_CHAR_INDEXES);

	DOGMA_ASSUME_OK(dogma_set_env_state(ctx, skill_env, DOGMA_STATE_Online));

	return DOGMA_OK;
}

int dogma_set_target(dogma_context_t* targeter, dogma_env_t* source,
                     dogma_context_t* targetee, dogma_env_t* target) {
	state_t s = source->state;

	DOGMA_ASSUME_OK(dogma_set_env_state(targeter, source, DOGMA_STATE_Unplugged));

	if(source->target.env != NULL) {
		/* Remove targeter from targetee */
		key_t index = (intptr_t)source;
		int ret;
		JLD(ret, source->target.env->targeted_by, index);
		assert(ret == 1);
	}

	source->target.context = targetee;
	source->target.env = target;

	if(source->target.env != NULL) {
		/* Add targeter to targetee */
		key_t index = (intptr_t)source;
		dogma_context_t** val;
		JLI(val, source->target.env->targeted_by, index);
		*val = targeter;
	}

	DOGMA_ASSUME_OK(dogma_set_env_state(targeter, source, s));

	return DOGMA_OK;
}

int dogma_toggle_chance_based_effect_env(dogma_context_t* ctx, dogma_env_t* env, effectid_t id, bool on) {
	const dogma_type_effect_t* te;
	const dogma_effect_t* e;
	bool* val;
	int ret;
	dogma_expctx_t result;

	DOGMA_ASSUME_OK(dogma_get_type_effect(env->id, id, &te));
	DOGMA_ASSUME_OK(dogma_get_effect(id, &e));

	if(e->fittingusagechanceattributeid == 0) {
		/* Effect is not chance-based */
		return DOGMA_NOT_APPLICABLE;
	}

	JLG(val, env->chance_effects, id);
	if(val == NULL) {
		/* Effect is off */
		if(!on) return DOGMA_OK;

		JLI(val, env->chance_effects, id);
		*val = true;

		DOGMA_ASSUME_OK(dogma_eval_expression(ctx, env, e->preexpressionid, &result));
	} else {
		/* Effect is on */
		assert(*val == true);
		if(on) return DOGMA_OK;

		JLD(ret, env->chance_effects, id);

		DOGMA_ASSUME_OK(dogma_eval_expression(ctx, env, e->postexpressionid, &result));
	}

	return DOGMA_OK;
}
