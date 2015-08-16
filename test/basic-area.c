/* libdogma
 * Copyright (C) 2015 Romain "Artefact2" Dalmaso <artefact2@gmail.com>
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
	dogma_key_t beacon;
	double value;

	assertok(dogma_init());
	assertok(dogma_init_context(&ctx));
	assertok(dogma_set_ship(ctx, TYPE_AmarrShuttle));

	assertok(dogma_get_ship_attribute(ctx, ATT_MaxTargetRange, &value));
	assertf(43750, value, 0);

	assertok(dogma_add_area_beacon(ctx, TYPE_MagnetarEffectBeaconClass5, &beacon));

	assertok(dogma_get_ship_attribute(ctx, ATT_MaxTargetRange, &value));
	assertf(24937.5, value, 0);

	value = 0;
	assertok(dogma_get_area_beacon_attribute(ctx, beacon, ATT_MaxTargetRangeMultiplier, &value));
	assert(value > 0);
	
	assertok(dogma_remove_area_beacon(ctx, beacon));
	assertok(dogma_free_context(ctx));
	return 0;
}
