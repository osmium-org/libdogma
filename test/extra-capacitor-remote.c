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

#define MOD(i) (dogma_location_t){ .type = DOGMA_LOC_Module, .module_index = slots[i] }

int main(void) {
	dogma_key_t slots[20];
	dogma_context_t* ctx[4];

	dogma_init();

	for(size_t i = 0; i < 4; ++i) {
		dogma_init_context(ctx + i);
		dogma_set_ship(ctx[i], TYPE_Rifter);
		dogma_add_module_s(ctx[i], TYPE_SmallEnergyTransferArrayII, slots + 5 * i, DOGMA_STATE_Active);
		dogma_add_module_s(ctx[i], TYPE_SmallEnergyTransferArrayII, slots + 5 * i + 1, DOGMA_STATE_Active);
		dogma_add_module_s(ctx[i], TYPE_SmallCapacitorBatteryII, slots + 5 * i + 4, DOGMA_STATE_Online);
	}

	dogma_target(ctx[0], MOD(0), ctx[1]);
	dogma_target(ctx[0], MOD(1), ctx[1]);
	dogma_target(ctx[1], MOD(5), ctx[0]);
	dogma_target(ctx[1], MOD(6), ctx[0]);

	dogma_target(ctx[2], MOD(10), ctx[3]);
	dogma_target(ctx[2], MOD(11), ctx[3]);
	dogma_target(ctx[3], MOD(15), ctx[2]);
	dogma_target(ctx[3], MOD(16), ctx[2]);

	for(size_t i = 0; i < 4; ++i) {
		double delta, s;
		bool stable;

		assert(dogma_get_capacitor(ctx[i], true, &delta, &stable, &s) == DOGMA_OK);
		assert(stable == true);
		assert(delta < 0);
	}

	for(size_t i = 0; i < 4; ++i) {
		dogma_free_context(ctx[i]);
	}

	return 0;
}
