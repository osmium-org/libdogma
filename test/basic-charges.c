/* libdogma
 * Copyright (C) 2013 Romain "Artefact2" Dalmaso <artefact2@gmail.com>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License
 * as published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public
 * License along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include "test.h"

dogma_context_t* ctx;
dogma_key_t slot;

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

	/* Source: Pyfa-1.10.0 */

	/* 587:2873;1:: */
	assert(dogma_set_ship(ctx, TYPE_Rifter) == DOGMA_OK);
	assert(dogma_add_module_s(ctx, TYPE_125mmGatlingAutoCannonII, &slot, DOGMA_STATE_Active) == DOGMA_OK);
	expect_optimal_falloff_tracking(1200.0, 8062.5, 0.52125);

	assert(dogma_add_charge(ctx, slot, TYPE_EMPS) == DOGMA_OK);
	expect_optimal_falloff_tracking(600.0, 8062.5, 0.52125);

	assert(dogma_add_charge(ctx, slot, TYPE_NuclearS) == DOGMA_OK);
	expect_optimal_falloff_tracking(1920.0, 8062.5, 0.5473125);

	assert(dogma_add_charge(ctx, slot, TYPE_BarrageS) == DOGMA_OK);
	expect_optimal_falloff_tracking(1200.0, 11287.5, 0.3909375);

	assert(dogma_free_context(ctx) == DOGMA_OK);
	return 0;
}
