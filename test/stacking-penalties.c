/* libdogma
 * Copyright (C) 2012, 2013 Romain "Artefact2" Dalmaso <artefact2@gmail.com>
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

static void expect_thermal_resistance(double resistance) {
	double value;

	dogma_get_ship_attribute(ctx, ATT_ArmorThermalDamageResonance, &value);
	assertf(1 - resistance * 0.01, value, 0.005); /* Pyfa tooltip only shows two digits */
}

static void expect_max_velocity(double velocity) {
	double value;

	dogma_get_ship_attribute(ctx, ATT_MaxVelocity, &value);
	assertf(velocity, value, 0.05);
}

int main(void) {
	dogma_key_t slot0, slot1, slot2, slot3, slot4, slot5, slot6, slot7, slot8, slot9;

	dogma_init();
	dogma_init_context(&ctx);

	/* Source: Pyfa-42efa48 (Jun 13 2013) */

	dogma_set_ship(ctx, TYPE_Abaddon);
	expect_thermal_resistance(48.0);

	dogma_add_module(ctx, TYPE_DamageControlII, &slot0);
	expect_thermal_resistance(48.0);
	dogma_set_module_state(ctx, slot0, DOGMA_STATE_Active);
	expect_thermal_resistance(55.80);
	dogma_set_module_state(ctx, slot0, DOGMA_STATE_Online);
	expect_thermal_resistance(48.0);
	dogma_set_module_state(ctx, slot0, DOGMA_STATE_Active);
	expect_thermal_resistance(55.80);

	dogma_add_module(ctx, TYPE_ReactiveArmorHardener, &slot1);
	dogma_set_module_state(ctx, slot1, DOGMA_STATE_Active);
	expect_thermal_resistance(61.56);

	dogma_add_module(ctx, TYPE_DraclirasModifiedEnergizedAdaptiveNanoMembrane, &slot2);
	dogma_add_module(ctx, TYPE_DraclirasModifiedEnergizedAdaptiveNanoMembrane, &slot3);
	dogma_add_module(ctx, TYPE_DraclirasModifiedEnergizedAdaptiveNanoMembrane, &slot4);
	dogma_add_module(ctx, TYPE_DraclirasModifiedEnergizedAdaptiveNanoMembrane, &slot5);
	dogma_add_module(ctx, TYPE_DraclirasModifiedEnergizedAdaptiveNanoMembrane, &slot6);
	expect_thermal_resistance(61.56);

	dogma_set_module_state(ctx, slot2, DOGMA_STATE_Online);
	expect_thermal_resistance(76.10);
	dogma_set_module_state(ctx, slot3, DOGMA_STATE_Online);
	expect_thermal_resistance(83.95);
	dogma_set_module_state(ctx, slot4, DOGMA_STATE_Online);
	expect_thermal_resistance(87.41);
	dogma_set_module_state(ctx, slot5, DOGMA_STATE_Online);
	expect_thermal_resistance(88.76);
	dogma_set_module_state(ctx, slot6, DOGMA_STATE_Online);
	expect_thermal_resistance(89.21);

	dogma_add_module(ctx, TYPE_LargeAntiThermalPumpII, &slot7);
	dogma_add_module(ctx, TYPE_LargeAntiThermalPumpII, &slot8);
	dogma_add_module(ctx, TYPE_LargeAntiThermalPumpII, &slot9);

	dogma_set_module_state(ctx, slot7, DOGMA_STATE_Online);
	expect_thermal_resistance(89.32);
	dogma_set_module_state(ctx, slot7, DOGMA_STATE_Online);
	expect_thermal_resistance(89.35);
	dogma_set_module_state(ctx, slot7, DOGMA_STATE_Online);
	expect_thermal_resistance(89.35);

	dogma_free_context(ctx);

	/* Now let's try something a bit more intricate. How about an
	 * attribute with both positive and negative modifiers. */
	/* Source: Pyfa-42efa48 (Jun 13 2013) */

	dogma_init_context(&ctx);
	dogma_set_ship(ctx, TYPE_Abaddon);
	expect_max_velocity(111.2);

	dogma_add_module_s(ctx, TYPE_OverdriveInjectorSystemII, &slot0, DOGMA_STATE_Online);
	dogma_add_module_s(ctx, TYPE_OverdriveInjectorSystemII, &slot1, DOGMA_STATE_Online);
	dogma_add_module_s(ctx, TYPE_OverdriveInjectorSystemII, &slot2, DOGMA_STATE_Online);
	dogma_add_module_s(ctx, TYPE_OverdriveInjectorSystemII, &slot3, DOGMA_STATE_Online);
	dogma_add_module_s(ctx, TYPE_OverdriveInjectorSystemII, &slot4, DOGMA_STATE_Online);
	dogma_add_module_s(ctx, TYPE_OverdriveInjectorSystemII, &slot5, DOGMA_STATE_Online);
	dogma_add_module_s(ctx, TYPE_OverdriveInjectorSystemII, &slot6, DOGMA_STATE_Online);
	expect_max_velocity(156.7);

	dogma_add_module_s(ctx, TYPE_LargeTrimarkArmorPumpII, &slot7, DOGMA_STATE_Online);
	dogma_add_module_s(ctx, TYPE_LargeTrimarkArmorPumpII, &slot8, DOGMA_STATE_Online);
	dogma_add_module_s(ctx, TYPE_LargeTrimarkArmorPumpII, &slot9, DOGMA_STATE_Online);
	expect_max_velocity(138.3);

	/* On the other hand, the capacity penalty of the Overdrives is
	 * not penalized. */
	double v;
	dogma_get_ship_attribute(ctx, ATT_Capacity, &v);
	assertf(110.1, v, 0.05);

	dogma_free_context(ctx);
	return 0;
}
