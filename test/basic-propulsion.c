/* libdogma
 * Copyright (C) 2017 Romain "Artefact2" Dalmaso <artefact2@gmail.com>
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

int main(void) {
	dogma_context_t* ctx;
	double value;
	dogma_key_t m, n;

	/* Data source: pyfa 1.26.0 Stable - YC118.10 1.2 */
	
	assert(dogma_init() == DOGMA_OK);
	assert(dogma_init_context(&ctx) == DOGMA_OK);

	assert(dogma_set_ship(ctx, TYPE_Rifter) == DOGMA_OK);

	assert(dogma_get_ship_attribute(ctx, ATT_MaxVelocity, &value) == DOGMA_OK);
	assertf(456.25, value, .01);

	assert(dogma_add_module_s(ctx, TYPE_1MNAfterburnerII, &m, DOGMA_STATE_Active) == DOGMA_OK);
	assert(dogma_get_ship_attribute(ctx, ATT_MaxVelocity, &value) == DOGMA_OK);
	assertf(1193.25253299, value, .01);

	assert(dogma_add_module_s(ctx, TYPE_1600mmSteelPlatesII, &n, DOGMA_STATE_Online) == DOGMA_OK);
	assert(dogma_get_ship_attribute(ctx, ATT_MaxVelocity, &value) == DOGMA_OK);
	assertf(719.951977966, value, .01);

	assert(dogma_remove_module(ctx, m) == DOGMA_OK);
	assert(dogma_get_ship_attribute(ctx, ATT_MaxVelocity, &value) == DOGMA_OK);
	assertf(456.25, value, .01);
	
	assert(dogma_remove_module(ctx, n) == DOGMA_OK);
	
	assert(dogma_get_ship_attribute(ctx, ATT_SignatureRadius, &value) == DOGMA_OK);
	assertf(35.0, value, .01);

	assert(dogma_add_module_s(ctx, TYPE_5MNMicrowarpdriveII, &m, DOGMA_STATE_Active) == DOGMA_OK);
	assert(dogma_get_ship_attribute(ctx, ATT_MaxVelocity, &value) == DOGMA_OK);
	assertf(3240.48141353, value, .01);	
	assert(dogma_get_ship_attribute(ctx, ATT_SignatureRadius, &value) == DOGMA_OK);
	assertf(210.0, value, .01);

	assert(dogma_free_context(ctx) == DOGMA_OK);
	return 0;
}
