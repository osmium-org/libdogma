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
#include "dogma.h"
#include "dogma_internal.h"
#include "tables.h"
#include "eval.h"
#include "attribute.h"

#define DOGMA_MIN_SKILL_LEVEL 0
#define DOGMA_MAX_SKILL_LEVEL 5

#define CAT_Skill 16

int dogma_init(void) {
	dogma_init_tables();

	assert(DOGMA_MIN_SKILL_LEVEL <= DOGMA_MAX_SKILL_LEVEL);

	return DOGMA_OK;
}

int dogma_init_context(dogma_context_t** ctx) {
	dogma_context_t* new_ctx = malloc(sizeof(dogma_context_t));
	dogma_env_t** value;
	key_t index = 0;
	const dogma_type_t** type;

	new_ctx->character = malloc(sizeof(dogma_env_t));
	new_ctx->ship = malloc(sizeof(dogma_env_t));

	new_ctx->character->id = 0;
	new_ctx->character->parent = NULL;
	new_ctx->character->index = 0;
	new_ctx->character->children = (array_t)NULL;
	new_ctx->character->modifiers = (array_t)NULL;

	JLI(value, new_ctx->character->children, 0);
	*value = new_ctx->ship;

	new_ctx->ship->id = 0;
	new_ctx->ship->parent = new_ctx->character;
	new_ctx->ship->index = 0;
	new_ctx->ship->children = (array_t)NULL;
	new_ctx->ship->modifiers = (array_t)NULL;

	new_ctx->target = NULL;
	new_ctx->area = NULL;

	new_ctx->default_skill_level = DOGMA_MAX_SKILL_LEVEL;
	new_ctx->skill_levels = (array_t)NULL;

	*ctx = new_ctx;

	/* Inject all skills. This is somewhat costly, maybe there is a
	 * way to do it lazily? */
	JLF(type, types_by_id, index);
	while(type != NULL) {
		if((*type)->categoryid == CAT_Skill) {
			dogma_inject_skill(*ctx, (*type)->id);
		}
		JLN(type, types_by_id, index);
	}

	return DOGMA_OK;
}

int dogma_free_context(dogma_context_t* ctx) {
	dogma_free_env(ctx->character);
	dogma_reset_skill_levels(ctx);
	free(ctx);

	return DOGMA_OK;
}

int dogma_set_default_skill_level(dogma_context_t* ctx, uint8_t default_level) {
	if(default_level < DOGMA_MIN_SKILL_LEVEL) default_level = DOGMA_MIN_SKILL_LEVEL;
	if(default_level > DOGMA_MAX_SKILL_LEVEL) default_level = DOGMA_MAX_SKILL_LEVEL;

	ctx->default_skill_level = default_level;

	return DOGMA_OK;
}

int dogma_set_skill_level(dogma_context_t* ctx, typeid_t skillid, uint8_t level) {
	uint8_t* value;

	if(level < DOGMA_MIN_SKILL_LEVEL) level = DOGMA_MIN_SKILL_LEVEL;
	if(level > DOGMA_MAX_SKILL_LEVEL) level = DOGMA_MAX_SKILL_LEVEL;

	JLI(value, ctx->skill_levels, skillid);
	*value = level;

	return DOGMA_OK;
}

int dogma_reset_skill_levels(dogma_context_t* ctx) {
	int ret;
	JLFA(ret, ctx->skill_levels);

	return DOGMA_OK;
}

int dogma_set_ship(dogma_context_t* ctx, typeid_t ship_typeid) {
	array_t shipeffects;
	key_t index;
	const dogma_type_effect_t** te;
	const dogma_effect_t* e;
	dogma_expctx_t result;

	if(ship_typeid == ctx->ship->id) {
		/* Be lazy */
		return DOGMA_OK;
	}

	if(ctx->ship->id != 0) {
		/* Eval (old) ship postExpressions */
		DOGMA_ASSUME_OK(dogma_get_type_effects(ctx->ship->id, &shipeffects));
		index = 0;
		JLF(te, shipeffects, index);
		while(te != NULL) {
			DOGMA_ASSUME_OK(dogma_get_effect((*te)->effectid, &e));
			DOGMA_ASSUME_OK(dogma_eval_expression(ctx, ctx->ship, NULL, e->postexpressionid, &result));
			JLN(te, shipeffects, index);
		}
	}

	ctx->ship->id = ship_typeid;

	if(ctx->ship->id != 0) {
		/* Eval (new) ship preExpressions */
		DOGMA_ASSUME_OK(dogma_get_type_effects(ctx->ship->id, &shipeffects));
		index = 0;
		JLF(te, shipeffects, index);
		while(te != NULL) {
			DOGMA_ASSUME_OK(dogma_get_effect((*te)->effectid, &e));
			DOGMA_ASSUME_OK(dogma_eval_expression(ctx, ctx->ship, NULL, e->preexpressionid, &result));
			JLN(te, shipeffects, index);
		}
	}

	return DOGMA_OK;
}

int dogma_get_ship_attribute(dogma_context_t* ctx, attributeid_t attributeid, double* out) {
	return dogma_get_env_attribute(ctx, ctx->ship, attributeid, out);
}
