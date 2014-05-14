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
		dogma_add_module_s(ctx[i], TYPE_SmallRemoteCapacitorTransmitterII, slots + 5 * i, DOGMA_STATE_Active);
		dogma_add_module_s(ctx[i], TYPE_SmallRemoteCapacitorTransmitterII, slots + 5 * i + 1, DOGMA_STATE_Active);
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

	for(size_t i = 0; i < 4; ++i) {
		assert(dogma_get_capacitor_all(ctx[i], true, &list, &len) == DOGMA_OK);
		assert(len == 4);

		for(size_t j = 0; j < len; ++j) {
			printf(
				"context:%p\tstable:%i\tcapacity:%f\tdelta:%f\ts:%f\n",
				list[j].context,
				list[j].stable,
				list[j].capacity,
				list[j].delta,
				list[j].stable ? list[j].stable_fraction : list[j].depletion_time
			);
		}

		dogma_free_capacitor_list(list);
	}

	for(size_t i = 0; i < 4; ++i) {
		dogma_free_context(ctx[i]);
	}



	dogma_init_context(&ctx[0]);
	dogma_init_context(&ctx[1]);

	dogma_set_ship(ctx[0], TYPE_Dominix);
	dogma_set_ship(ctx[1], TYPE_Guardian);
	dogma_add_module_s(ctx[1], TYPE_LargeRemoteArmorRepairerII, &slots[0], DOGMA_STATE_Active);
	dogma_target(ctx[1], MOD(0) ,ctx[0]);

	assert(dogma_get_capacitor_all(ctx[0], true, &list, &len) == DOGMA_OK);
	dogma_free_capacitor_list(list);
	assert(dogma_get_capacitor_all(ctx[1], true, &list, &len) == DOGMA_OK);
	dogma_free_capacitor_list(list);

	dogma_free_context(ctx[0]);
	dogma_free_context(ctx[1]);



	dogma_init_context(&ctx[0]);
	dogma_init_context(&ctx[1]);

	dogma_set_ship(ctx[0], TYPE_Dominix);
	dogma_set_ship(ctx[1], TYPE_Dominix);

	dogma_add_module_s(ctx[0], TYPE_HeavyEnergyNeutralizerII, &slots[0], DOGMA_STATE_Active);
	dogma_add_module_s(ctx[0], TYPE_HeavyNosferatuII, &slots[1], DOGMA_STATE_Active);
	dogma_target(ctx[0], MOD(0), ctx[1]);
	dogma_target(ctx[0], MOD(1), ctx[1]);

	size_t len1, len2;
	dogma_simple_capacitor_t *list1, *list2;

	assert(dogma_get_capacitor_all(ctx[0], true, &list1, &len1) == DOGMA_OK);
	assert(dogma_get_capacitor_all(ctx[1], true, &list2, &len2) == DOGMA_OK);

	/* Test overall equality of result sets */
	assert(len1 == len2);
	size_t tested = 0;

	for(size_t i = 0; i < len1; ++i) {
		for(size_t j = 0; j < len1; ++j) {
			dogma_simple_capacitor_t *p1, *p2;
			p1 = list1 + i;
			p2 = list2 + j;

			if(p1->context != p2->context) continue;

			assert(abs(p1->capacity - p2->capacity) < 1e-300);
			assert(abs(p1->delta - p2->delta) < 1e-300);
			assert(p1->stable == p2->stable);
			assert((p1->stable_fraction - p2->stable_fraction) < 1e-300);

			++tested;
		}
	}

	assert(tested == len1);

	dogma_free_capacitor_list(list1);
	dogma_free_capacitor_list(list2);
	dogma_free_context(ctx[0]);
	dogma_free_context(ctx[1]);

	return 0;
}
