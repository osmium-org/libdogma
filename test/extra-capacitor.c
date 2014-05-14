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

static void assert_capacitor_unstable(bool reload, double expected_delta,
                                      double expected_mins, double expected_secs,
									  double precision_ms) {
	double global_delta;
	bool stable;
	double param;

	assert(dogma_get_capacitor(ctx, reload, &global_delta, &stable, &param) == DOGMA_OK);
	assertf(expected_delta, global_delta * 1000, 0.1);
	assert(stable == false);
	assertf((expected_secs + expected_mins * 60) * 1000, param, precision_ms);
}

static void assert_capacitor_stable(bool reload, double expected_delta,
                                    double expected_percentage) {
	double global_delta;
	bool stable;
	double param;

	assert(dogma_get_capacitor(ctx, reload, &global_delta, &stable, &param) == DOGMA_OK);
	assertf(expected_delta, global_delta * 1000, 0.1);
	assert(stable == true);
	assertf(expected_percentage, param, 2.5);
}

int main(void) {
	dogma_key_t slots[10];
	double global_delta;
	bool stable;
	double param;

	dogma_init();
	dogma_init_context(&ctx);

	/* Source: Pyfa-c67034e (2013-07-01) */

	/* Try without a ship, to see if stuff blows up */
	dogma_add_module_sc(ctx, TYPE_MegaPulseLaserII, &slots[0], DOGMA_STATE_Active, TYPE_MultifrequencyL);
	assert_capacitor_unstable(false, 4.76, 0, 0, 0.000001);
	assert_capacitor_unstable(true, 4.76, 0, 0, 0.000001);

	dogma_add_module_s(ctx, TYPE_MegaPulseLaserII, &slots[1], DOGMA_STATE_Active);
	assert(dogma_get_capacitor(ctx, false, &global_delta, &stable, &param) == DOGMA_OK);
	assert_capacitor_unstable(false, 4.76 * 2, 0, 0, 0.000001);
	assert_capacitor_unstable(true, 4.76 * 2, 0, 0, 0.000001);

	dogma_set_module_state(ctx, slots[0], DOGMA_STATE_Online);
	dogma_set_module_state(ctx, slots[1], DOGMA_STATE_Online);
	assert_capacitor_stable(false, 0.0, 100.0);
	assert_capacitor_stable(true, 0.0, 100.0);

	/* Now test real stuff */
	dogma_set_ship(ctx, TYPE_Abaddon);
	dogma_set_module_state(ctx, slots[0], DOGMA_STATE_Active);
	dogma_set_module_state(ctx, slots[1], DOGMA_STATE_Active);
	dogma_add_charge(ctx, slots[1], TYPE_MultifrequencyL);
	assert_capacitor_stable(false, 4.76 * 2 - 21.3, 76.0);
	assert_capacitor_stable(true, 4.76 * 2 - 21.3, 76.0);

	dogma_add_module_sc(ctx, TYPE_MegaPulseLaserII, &slots[2], DOGMA_STATE_Active, TYPE_MultifrequencyL);
	dogma_add_module_sc(ctx, TYPE_MegaPulseLaserII, &slots[3], DOGMA_STATE_Active, TYPE_MultifrequencyL);
	assert_capacitor_stable(false, 4.76 * 4 - 21.3, 43.8);
	assert_capacitor_stable(true, 4.76 * 4 - 21.3, 43.8);

	dogma_add_charge(ctx, slots[0], TYPE_StandardL);
	dogma_add_charge(ctx, slots[1], TYPE_StandardL);
	dogma_add_charge(ctx, slots[2], TYPE_StandardL);
	dogma_add_charge(ctx, slots[3], TYPE_StandardL);
	dogma_add_module_sc(ctx, TYPE_MegaPulseLaserII, &slots[4], DOGMA_STATE_Active, TYPE_StandardL);
	dogma_add_module_sc(ctx, TYPE_MegaPulseLaserII, &slots[5], DOGMA_STATE_Active, TYPE_StandardL);
	dogma_add_module_sc(ctx, TYPE_MegaPulseLaserII, &slots[6], DOGMA_STATE_Active, TYPE_StandardL);
	dogma_add_module_sc(ctx, TYPE_MegaPulseLaserII, &slots[7], DOGMA_STATE_Active, TYPE_StandardL);
	assert_capacitor_stable(false, 2.62 * 8 - 21.3, 31.7);
	assert_capacitor_stable(true, 2.62 * 8 - 21.3, 31.7);

	dogma_add_charge(ctx, slots[0], TYPE_InfraredL);
	assert_capacitor_unstable(false, 2.62 * 7 + 3.1 - 21.3, 100, 49, 60000);
	assert_capacitor_unstable(true, 2.62 * 7 + 3.1 - 21.3, 100, 49, 60000);


	for(size_t i = 0; i < 8; ++i) dogma_add_charge(ctx, slots[i], TYPE_MultifrequencyL);
	assert_capacitor_unstable(false, 4.76 * 8 - 21.3, 5, 53, 5000);
	assert_capacitor_unstable(true, 4.76 * 8 - 21.3, 5, 53, 5000);

	/* Now test cap boosters. Sadly Pyfa does not allow not including
	 * recharge time for them. */
	dogma_add_module_sc(ctx, TYPE_HeavyCapacitorBoosterII, &slots[8], DOGMA_STATE_Active, TYPE_CapBooster800);
	assert_capacitor_stable(true, 4.76 * 8 - 78.4, 89.8);

	dogma_add_charge(ctx, slots[8], TYPE_CapBooster400);
	assert_capacitor_stable(true, 4.76 * 8 - 52.0, 77.2);

	dogma_add_charge(ctx, slots[8], TYPE_CapBooster200);
	assert_capacitor_unstable(true, 4.76 * 8 - 37.3, 41, 37, 120000);

	dogma_add_charge(ctx, slots[8], TYPE_CapBooster100);
	assert_capacitor_unstable(true, 4.76 * 8 - 29.4, 9, 41, 20000);

	dogma_add_module_sc(ctx, TYPE_HeavyCapacitorBoosterII, &slots[9], DOGMA_STATE_Active, TYPE_CapBooster75);
	dogma_add_charge(ctx, slots[8], TYPE_CapBooster100);
	assert_capacitor_unstable(true, 4.76 * 8 - 35.6, 22, 13, 20000);

	/* XXX: this test is somewhat idiotic, considering it's impossible
	 * to overload the booster for this long. */
	dogma_set_module_state(ctx, slots[9], DOGMA_STATE_Overloaded);
	assert_capacitor_unstable(true, 4.76 * 8 - 37.1, 37, 36, 120000);

	dogma_free_context(ctx);
	dogma_init_context(&ctx);

	/* Test reload time on something that is not a cap booster */
	dogma_set_ship(ctx, TYPE_Megathron);

	for(size_t i = 0; i < 7; ++i)
		dogma_add_module_sc(ctx, TYPE_NeutronBlasterCannonII, &slots[i], DOGMA_STATE_Active, TYPE_NullL);

	assert_capacitor_stable(false, 2.25 * 7 - 21.7, 58.4);
	assert_capacitor_stable(true, 2.21 * 7 - 21.7,  58.3 /* XXX: 65.8 */);

	dogma_free_context(ctx);
	return 0;
}
