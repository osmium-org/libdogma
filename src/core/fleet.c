/* libdogma
 * Copyright (C) 2013 Romain "Artefact2" Dalmaso <artefact2@gmail.com>
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

#include "fleet.h"

static dogma_fleet_context_t* dogma_get_subfleet(dogma_fleet_context_t*, key_t);

static int dogma_add_subfleet_commander(dogma_fleet_context_t* subfctx, dogma_context_t*);

int dogma_init_fleet_context(dogma_fleet_context_t** fctx) {
	dogma_fleet_context_t* new_fctx;
	DOGMA_INIT_FLEET_CTX(new_fctx, NULL, 0);
	*fctx = new_fctx;
	return DOGMA_OK;
}

int dogma_free_fleet_context(dogma_fleet_context_t* fctx) {
	dogma_fleet_context_t** subfctx;
	dogma_context_t** memberctx;
	key_t index = 0;
	int ret;

	if(fctx->parent != NULL) {
		JLD(ret, fctx->parent->subfleets, fctx->index);
		assert(ret == 1);
	}

	if(fctx->commander != NULL) {
		assert(fctx->commander->fleet == fctx);
		fctx->commander->fleet = NULL;
	}

	JLF(memberctx, fctx->members, index);
	while(memberctx != NULL) {
		assert((*memberctx)->fleet == fctx);
		(*memberctx)->fleet = NULL;
		JLN(memberctx, fctx->members, index);
	}

	index = 0;
	JLF(subfctx, fctx->subfleets, index);
	while(subfctx != NULL) {
		DOGMA_ASSUME_OK(dogma_free_fleet_context(*subfctx));
		JLN(subfctx, fctx->subfleets, index);
	}
	JLFA(ret, fctx->subfleets);
	JLFA(ret, fctx->members);

	free(fctx);
	return DOGMA_OK;
}

int dogma_add_fleet_commander(dogma_fleet_context_t* fctx, dogma_context_t* ctx) {
	return dogma_add_subfleet_commander(fctx, ctx);
}

int dogma_add_wing_commander(dogma_fleet_context_t* fctx, key_t wing, dogma_context_t* ctx) {
	return dogma_add_subfleet_commander(
		dogma_get_subfleet(fctx, wing),
		ctx
	);
}

int dogma_add_squad_commander(dogma_fleet_context_t* fctx, key_t wing, key_t squad, dogma_context_t* ctx) {
	return dogma_add_subfleet_commander(
		dogma_get_subfleet(
			dogma_get_subfleet(fctx, wing),
			squad
		),
		ctx
	);
}

int dogma_add_squad_member(dogma_fleet_context_t* fctx, key_t wing, key_t squad, dogma_context_t* ctx) {
	key_t index = (intptr_t)ctx;
	dogma_context_t** memberctx;
	dogma_fleet_context_t* subfctx = dogma_get_subfleet(
		dogma_get_subfleet(fctx, wing),
		squad
	);

	if(ctx->fleet != NULL) {
		bool found;
		DOGMA_ASSUME_OK(dogma_remove_fleet_member(ctx->fleet, ctx, &found));
		assert(found == true && ctx->fleet == NULL);
	}

	JLG(memberctx, subfctx->members, index);
	if(memberctx != NULL) {
		assert(*memberctx == ctx && ctx->fleet == fctx);
		return DOGMA_OK;
	}

	JLI(memberctx, subfctx->members, index);
	*memberctx = ctx;
	ctx->fleet = subfctx;

	return DOGMA_OK;
}

int dogma_remove_fleet_member(dogma_fleet_context_t* fctx, dogma_context_t* ctx, bool* found) {
	key_t index = (intptr_t)ctx;
	dogma_fleet_context_t** subfleetctx;
	int ret;

	if(fctx->booster == ctx) {
		fctx->booster = NULL;
	}

	if(fctx->commander == ctx) {
		fctx->commander = NULL;
		ctx->fleet = NULL;
		*found = true;
		return DOGMA_OK;
	}

	JLD(ret, fctx->members, index);
	if(ret == 1) {
		ctx->fleet = NULL;
		*found = true;
		return DOGMA_OK;
	}

	index = 0;
	JLF(subfleetctx, fctx->subfleets, index);
	while(subfleetctx != NULL) {
		DOGMA_ASSUME_OK(dogma_remove_fleet_member(*subfleetctx, ctx, found));
		if(*found == true) {
			assert(ctx->fleet == NULL);
			return DOGMA_OK;
		}

		JLN(subfleetctx, fctx->subfleets, index);
	}

	*found = false;
	return DOGMA_OK;
}

int dogma_set_fleet_booster(dogma_fleet_context_t* fctx, dogma_context_t* ctx) {
	fctx->booster = ctx;
	return DOGMA_OK;
}

int dogma_set_wing_booster(dogma_fleet_context_t* fctx, key_t wing, dogma_context_t* ctx) {
	dogma_fleet_context_t* wing_fctx = dogma_get_subfleet(fctx, wing);
	wing_fctx->booster = ctx;
	return DOGMA_OK;
}

int dogma_set_squad_booster(dogma_fleet_context_t* fctx, key_t wing, key_t squad, dogma_context_t* ctx) {
	dogma_fleet_context_t* squad_fctx = dogma_get_subfleet(
		dogma_get_subfleet(fctx, wing),
		squad
	);

	squad_fctx->booster = ctx;
	return DOGMA_OK;
}

static dogma_fleet_context_t* dogma_get_subfleet(dogma_fleet_context_t* fctx, key_t subidx) {
	dogma_fleet_context_t** sub_fctx;
	JLG(sub_fctx, fctx->subfleets, subidx);
	if(sub_fctx == NULL) {
		JLI(sub_fctx, fctx->subfleets, subidx);
		DOGMA_INIT_FLEET_CTX(*sub_fctx, fctx, subidx);
	}

	return *sub_fctx;
}

static int dogma_add_subfleet_commander(dogma_fleet_context_t* subfctx, dogma_context_t* ctx) {
	if(subfctx->commander != NULL) {
		return DOGMA_NOT_APPLICABLE;
	}

	if(ctx->fleet != NULL) {
		bool found;
		DOGMA_ASSUME_OK(dogma_remove_fleet_member(ctx->fleet, ctx, &found));
		assert(found == true && ctx->fleet == NULL);
	}

	ctx->fleet = subfctx;

	subfctx->commander = ctx;
	if(subfctx->booster == NULL) {
		subfctx->booster = ctx;
	}

	return DOGMA_OK;
}
