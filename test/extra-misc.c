/* libdogma
 * Copyright (C) 2013, 2014 Romain "Artefact2" Dalmaso <artefact2@gmail.com>
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
	dogma_key_t slot;
	bool able, hasit, projectable;
	int ncycles;
	double out, duration, tracking, discharge, range, falloff, usagechance;
	dogma_effectid_t eff;

	dogma_init();

	assert(dogma_type_has_effect(
		TYPE_AdaptiveNanoPlatingII, DOGMA_STATE_Online, EFFECT_HiPower, &hasit
	) == DOGMA_OK);
	assert(hasit == false);
	assert(dogma_type_has_effect(
		TYPE_AdaptiveNanoPlatingII, DOGMA_STATE_Unplugged, EFFECT_LoPower, &hasit
	) == DOGMA_OK);
	assert(hasit == false);

	assert(dogma_type_has_active_effects(TYPE_AdaptiveNanoPlatingII, &able) == DOGMA_OK);
	assert(able == false);
	assert(dogma_type_has_overload_effects(TYPE_AdaptiveNanoPlatingII, &able) == DOGMA_OK);
	assert(able == false);

	assert(dogma_type_has_active_effects(TYPE_DamageControlII, &able) == DOGMA_OK);
	assert(able == true);
	assert(dogma_type_has_overload_effects(TYPE_DamageControlII, &able) == DOGMA_OK);
	assert(able == false);

	assert(dogma_type_has_active_effects(TYPE_1MNAfterburnerII, &able) == DOGMA_OK);
	assert(able == true);
	assert(dogma_type_has_overload_effects(TYPE_1MNAfterburnerII, &able) == DOGMA_OK);
	assert(able == true);

	assert(dogma_type_has_projectable_effects(TYPE_WarpScramblerI, &projectable) == DOGMA_OK);
	assert(projectable == true);
	assert(dogma_type_has_projectable_effects(TYPE_DamageControlII, &projectable) == DOGMA_OK);
	assert(projectable == false);
	assert(dogma_type_has_projectable_effects(TYPE_SmallEnergyNosferatuI, &projectable) == DOGMA_OK);
	assert(projectable == true);

	dogma_init_context(&ctx);

	dogma_set_ship(ctx, TYPE_Rifter);

	dogma_add_module_sc(ctx, TYPE_SmallFocusedPulseLaserII, &slot, DOGMA_STATE_Active, TYPE_MultifrequencyS);
	assert(dogma_get_number_of_module_cycles_before_reload(ctx, slot, &ncycles) == DOGMA_OK);
	assert(ncycles == -1);

	dogma_add_charge(ctx, slot, TYPE_ScorchS);
	assert(dogma_get_number_of_module_cycles_before_reload(ctx, slot, &ncycles) == DOGMA_OK);
	assert(ncycles == 1000); /* No need for a source, the math is easy enough */

	dogma_add_charge(ctx, slot, TYPE_ImperialNavyGammaS);
	assert(dogma_get_number_of_module_cycles_before_reload(ctx, slot, &ncycles) == DOGMA_OK);
	assert(ncycles == 4000);

	dogma_remove_module(ctx, slot);
	dogma_add_module_sc(ctx, TYPE_LightElectronBlasterII, &slot, DOGMA_STATE_Active, TYPE_IridiumChargeS);
	assert(dogma_get_number_of_module_cycles_before_reload(ctx, slot, &ncycles) == DOGMA_OK);
	assert(ncycles == 200);

	assert(dogma_type_base_attribute(TYPE_Rifter, ATT_TurretSlotsLeft, &out) == DOGMA_OK);
	assert(out == 3.0);
	assert(dogma_get_ship_attribute(ctx, ATT_TurretSlotsLeft, &out) == DOGMA_OK);
	assert(out == 2.0);

	assert(dogma_get_nth_type_effect_with_attributes(TYPE_LightElectronBlasterII, 1, &eff) == DOGMA_NOT_FOUND);
	assert(dogma_get_nth_type_effect_with_attributes(TYPE_LightElectronBlasterII, 0, &eff) == DOGMA_OK);
	assert(dogma_get_location_effect_attributes(
		ctx, (dogma_location_t){ .type = DOGMA_LOC_Module, .module_index = slot }, eff,
		&duration, &tracking, &discharge, &range, &falloff, &usagechance
	) == DOGMA_OK);
	/* Source: Pyfa-1.10.0 */
	assertf(1440.0, duration, 0.05);
	assertf(0.5475, tracking, 0.00005);
	assertf(0.374262, discharge, 0.0000005);
	assertf(range, 1800.0, 0.05);
	assertf(falloff, 1875.0, 0.05);

	dogma_remove_module(ctx, slot);
	dogma_add_module_sc(ctx, TYPE_ArbalestCompactLightMissileLauncher,
	                    &slot, DOGMA_STATE_Active, TYPE_NovaLightMissile);
	assert(dogma_get_number_of_module_cycles_before_reload(ctx, slot, &ncycles) == DOGMA_OK);
	assert(ncycles == 40);

	dogma_remove_module(ctx, slot);
	dogma_add_module_sc(ctx, TYPE_TrackingComputerII, &slot, DOGMA_STATE_Active, TYPE_TrackingSpeedScript);
	assert(dogma_get_number_of_module_cycles_before_reload(ctx, slot, &ncycles) == DOGMA_OK);
	assert(ncycles == -1);

	dogma_free_context(ctx);
	return 0;
}
