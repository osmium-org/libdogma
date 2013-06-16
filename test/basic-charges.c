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

#include "test.h"

#define TYPE_Rifter (typeid_t)587
#define TYPE_AutocannonII (typeid_t)2873
#define TYPE_EMPS (typeid_t)185
#define TYPE_NuclearS (typeid_t)195
#define TYPE_BarrageS (typeid_t)12625

#define ATT_MaxRange (attributeid_t)54
#define ATT_Falloff (attributeid_t)158
#define ATT_TrackingSpeed (attributeid_t)160

dogma_context_t* ctx;
key_t slot;

static void expect_optimal_falloff_tracking(double optimal, double falloff, double tracking) {
	double v;

	assert(dogma_get_module_attribute(ctx, slot, ATT_MaxRange, &v) == DOGMA_OK);
	assertf(optimal, v, 0.5);

	assert(dogma_get_module_attribute(ctx, slot, ATT_Falloff, &v) == DOGMA_OK);
	assertf(falloff, v, 0.5);

	assert(dogma_get_module_attribute(ctx, slot, ATT_TrackingSpeed, &v) == DOGMA_OK);
	assertf(tracking, v, 0.000000005);
}

int main(void) {
	assert(dogma_init() == DOGMA_OK);
	assert(dogma_init_context(&ctx) == DOGMA_OK);

	/* Source: Pyfa-42efa48 (Jun 13 2013) */

	assert(dogma_set_ship(ctx, TYPE_Rifter) == DOGMA_OK);
	assert(dogma_add_module(ctx, TYPE_AutocannonII, &slot) == DOGMA_OK);
	assert(dogma_set_module_state(ctx, slot, DOGMA_Active) == DOGMA_OK);
	expect_optimal_falloff_tracking(1200.0, 5000.0, 0.71671875);

	assert(dogma_add_charge(ctx, slot, TYPE_EMPS) == DOGMA_OK);
	expect_optimal_falloff_tracking(600.0, 5000.0, 0.71671875);

	assert(dogma_add_charge(ctx, slot, TYPE_NuclearS) == DOGMA_OK);
	expect_optimal_falloff_tracking(1920.0, 5000.0, 0.7525546875);

	assert(dogma_add_charge(ctx, slot, TYPE_BarrageS) == DOGMA_OK);
	expect_optimal_falloff_tracking(1200.0, 7500.0, 0.5375390625);

	assert(dogma_free_context(ctx) == DOGMA_OK);
	return 0;
}
