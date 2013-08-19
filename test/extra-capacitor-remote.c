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
	double delta, s;
	bool stable;

	dogma_init();

	/* Initialize two sets of two rifters, each using two energy
	 * transferrers on the other: this creates two independent "loops"
	 * of two ships each. */

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
		/* Source: Pyfa-96bb1b1 (2013-07-30) for the delta numbers,
		 * common sense for the stability */
		assert(dogma_get_capacitor(ctx[i], true, &delta, &stable, &s) == DOGMA_OK);
		assert(stable == true);
		assertf(2 * (6.15 - 7.80) - 11.8, 1000 * delta, 0.1);
	}

	/* Now equip neutralizers on the first set of rifters, and project
	 * them on the other set. Equip nosferatus on teh other set and
	 * project them on the first set. */

	dogma_add_module_s(ctx[0], TYPE_SmallEnergyNeutralizerII, slots + 2, DOGMA_STATE_Active);
	dogma_add_module_s(ctx[0], TYPE_SmallEnergyNeutralizerII, slots + 3, DOGMA_STATE_Active);
	dogma_add_module_s(ctx[1], TYPE_SmallEnergyNeutralizerII, slots + 7, DOGMA_STATE_Active);
	dogma_add_module_s(ctx[1], TYPE_SmallEnergyNeutralizerII, slots + 8, DOGMA_STATE_Active);
	dogma_target(ctx[0], MOD(2), ctx[2]);
	dogma_target(ctx[0], MOD(3), ctx[3]);
	dogma_target(ctx[1], MOD(7), ctx[2]);
	dogma_target(ctx[1], MOD(8), ctx[3]);

	dogma_add_module_s(ctx[2], TYPE_SmallNosferatuII, slots + 12, DOGMA_STATE_Active);
	dogma_add_module_s(ctx[2], TYPE_SmallNosferatuII, slots + 13, DOGMA_STATE_Active);
	dogma_add_module_s(ctx[3], TYPE_SmallNosferatuII, slots + 17, DOGMA_STATE_Active);
	dogma_add_module_s(ctx[3], TYPE_SmallNosferatuII, slots + 18, DOGMA_STATE_Active);
	dogma_target(ctx[2], MOD(12), ctx[0]);
	dogma_target(ctx[2], MOD(13), ctx[1]);
	dogma_target(ctx[3], MOD(17), ctx[0]);
	dogma_target(ctx[3], MOD(18), ctx[1]);

	dogma_simple_capacitor_t* list;
	size_t len;

	assert(dogma_get_capacitor_all(ctx[0], true, &list, &len) == DOGMA_OK);
	assert(len == 4);

	for(size_t i = 0; i < len; ++i) {
		/* Not much we can assert here, no other tools reliably
		 * simulate capacitor in situations like this. If there's no
		 * infinite loops or segfaults at least, that's a good
		 * thing. */
		printf(
			"context:%p stable:%i capacity:%f delta:%f s:%f\n",
			list[i].context,
			list[i].stable,
			list[i].capacity,
			list[i].delta,
			list[i].stable ? list[i].stable_fraction : list[i].depletion_time
		);
	}

	dogma_free_capacitor_list(list);

	for(size_t i = 0; i < 4; ++i) {
		dogma_free_context(ctx[i]);
	}

	return 0;
}
