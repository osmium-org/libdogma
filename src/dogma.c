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
#include "tables.h"
#include "eval.h"

int dogma_init(void) {
	dogma_init_tables();

	return DOGMA_OK;
}

int dogma_init_context(dogma_context_t** ctx) {
	dogma_context_t* new_ctx = malloc(sizeof(dogma_context_t));
	dogma_env_t** value;

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

	new_ctx->default_skill_level = 0;
	new_ctx->skillpoints = (array_t)NULL;

	*ctx = new_ctx;
	return DOGMA_OK;
}

int dogma_free_context(dogma_context_t* ctx) {
	dogma_free_env(ctx->character);
	free(ctx);

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
		dogma_get_type_effects(ctx->ship->id, &shipeffects);
		index = 0;
		JLF(te, shipeffects, index);
		while(te != NULL) {
			dogma_get_effect((*te)->effectid, &e);
			dogma_eval_expression(ctx, ctx->ship, NULL, e->postexpressionid, &result);
			JLN(te, shipeffects, index);
		}
	}

	ctx->ship->id = ship_typeid;

	if(ctx->ship->id != 0) {
		/* Eval (new) ship preExpressions */
		dogma_get_type_effects(ctx->ship->id, &shipeffects);
		index = 0;
		JLF(te, shipeffects, index);
		while(te != NULL) {
			dogma_get_effect((*te)->effectid, &e);
			dogma_eval_expression(ctx, ctx->ship, NULL, e->preexpressionid, &result);
			JLN(te, shipeffects, index);
		}
	}

	return DOGMA_OK;
}
