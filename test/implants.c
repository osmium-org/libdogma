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

#define TYPE_Venture 32880

#define TYPE_CrystalAlpha 20121
#define TYPE_CrystalBeta 20157
#define TYPE_CrystalGamma 20158
#define TYPE_CrystalDelta 20159
#define TYPE_CrystalEpsilon 20160
#define TYPE_CrystalOmega 20161
#define TYPE_SSBII 400
#define TYPE_SBAII 24443

#define TYPE_SnakeAlpha 19540
#define TYPE_SnakeBeta 19551
#define TYPE_SnakeGamma 19553
#define TYPE_SnakeDelta 19554
#define TYPE_SnakeEpsilon 19555
#define TYPE_SnakeOmega 19556
#define TYPE_OverdriveInjectorSystemII 1236
#define TYPE_SmallPolycarb 31177

#define TYPE_StrongBluePill 10156
#define EFFECT_BoosterShieldCapacityPenalty 2737
#define EFFECT_BoosterCapacitorCapacityPenalty 2745

#define ATT_ShieldBonus 68
#define ATT_MaxVelocity 37
#define ATT_ShieldCapacity 263
#define ATT_CapacitorCapacity 482

dogma_context_t* ctx;
key_t slot0, slot1, slot2, slot3;
key_t impslot0, impslot1, impslot2, impslot3, impslot4, impslot5;

int main(void) {
	double v;

	dogma_init();
	dogma_init_context(&ctx);

	/* Source: Pyfa-42efa48 (Jun 13 2013) */

	/* Let's test a whole implant set (which has bonuses affecting the
	 * set as a whole), and see how it reacts with stacking penalized
	 * modules. */

	dogma_add_implant(ctx, TYPE_CrystalAlpha, &impslot0);
	dogma_add_implant(ctx, TYPE_CrystalBeta, &impslot1);
	dogma_add_implant(ctx, TYPE_CrystalGamma, &impslot2);
	dogma_add_implant(ctx, TYPE_CrystalDelta, &impslot3);
	dogma_add_implant(ctx, TYPE_CrystalEpsilon, &impslot4);
	dogma_add_implant(ctx, TYPE_CrystalOmega, &impslot5);

	dogma_set_ship(ctx, TYPE_Venture);
	dogma_add_module(ctx, TYPE_SSBII, &slot0);
	dogma_add_module(ctx, TYPE_SBAII, &slot1);
	dogma_add_module(ctx, TYPE_SBAII, &slot2);
	dogma_set_module_state(ctx, slot0, DOGMA_Active);
	dogma_set_module_state(ctx, slot1, DOGMA_Online);
	dogma_set_module_state(ctx, slot2, DOGMA_Online);

	dogma_get_module_attribute(ctx, slot0, ATT_ShieldBonus, &v);
	assertf(82.2947996695, v, 0.00000000005);

	dogma_free_context(ctx);
	dogma_init_context(&ctx);

	/* Source: Pyfa-42efa48 (Jun 13 2013) */
	/* Source: EFT 2.19.1 */

	/* Some sources say the Snake set is stacking penalized. This is
	 * not the case (unless both Pyfa and EFT are wrong). */

	dogma_add_implant(ctx, TYPE_SnakeAlpha, &impslot0);
	dogma_add_implant(ctx, TYPE_SnakeBeta, &impslot1);
	dogma_add_implant(ctx, TYPE_SnakeGamma, &impslot2);
	dogma_add_implant(ctx, TYPE_SnakeDelta, &impslot3);
	dogma_add_implant(ctx, TYPE_SnakeEpsilon, &impslot4);
	dogma_add_implant(ctx, TYPE_SnakeOmega, &impslot5);

	dogma_set_ship(ctx, TYPE_Venture);
	dogma_add_module(ctx, TYPE_OverdriveInjectorSystemII, &slot0);
	dogma_add_module(ctx, TYPE_SmallPolycarb, &slot1);
	dogma_add_module(ctx, TYPE_SmallPolycarb, &slot2);
	dogma_add_module(ctx, TYPE_SmallPolycarb, &slot3);
	dogma_set_module_state(ctx, slot0, DOGMA_Online);
	dogma_set_module_state(ctx, slot1, DOGMA_Online);
	dogma_set_module_state(ctx, slot2, DOGMA_Online);
	dogma_set_module_state(ctx, slot3, DOGMA_Online);

	dogma_get_ship_attribute(ctx, ATT_MaxVelocity, &v);
	assertf(644.929066501, v, 0.0000000005);

	dogma_free_context(ctx);
	dogma_init_context(&ctx);

	/* Source: EFT 2.19.1 */

	dogma_add_implant(ctx, TYPE_StrongBluePill, &impslot0);

	dogma_set_ship(ctx, TYPE_Venture);
	dogma_add_module(ctx, TYPE_SSBII, &slot0);
	dogma_set_module_state(ctx, slot0, DOGMA_Active);

	const location_t imploc0 = {
		.type = DOGMA_LOC_Implant,
		.implant_index = impslot0,
	};

	dogma_get_chance_based_effect_chance(ctx, imploc0, EFFECT_BoosterShieldCapacityPenalty, &v);
	assertf(0.3, v, 0.05);
	dogma_get_chance_based_effect_chance(ctx, imploc0, EFFECT_BoosterCapacitorCapacityPenalty, &v);
	assertf(0.3, v, 0.05);

	dogma_get_module_attribute(ctx, slot0, ATT_ShieldBonus, &v);
	assertf(39, v, 0.5);
	dogma_get_ship_attribute(ctx, ATT_ShieldCapacity, &v);
	assertf(281.25, v, 0.005);
	dogma_get_ship_attribute(ctx, ATT_CapacitorCapacity, &v);
	assertf(312.5, v, 0.05);

	dogma_toggle_chance_based_effect(ctx, imploc0, EFFECT_BoosterShieldCapacityPenalty, true);

	dogma_get_module_attribute(ctx, slot0, ATT_ShieldBonus, &v);
	assertf(39, v, 0.5);
	dogma_get_ship_attribute(ctx, ATT_ShieldCapacity, &v);
	assertf(217.96875, v, 0.000005);
	dogma_get_ship_attribute(ctx, ATT_CapacitorCapacity, &v);
	assertf(312.5, v, 0.05);

	dogma_toggle_chance_based_effect(ctx, imploc0, EFFECT_BoosterCapacitorCapacityPenalty, true);

	dogma_get_module_attribute(ctx, slot0, ATT_ShieldBonus, &v);
	assertf(39, v, 0.5);
	dogma_get_ship_attribute(ctx, ATT_ShieldCapacity, &v);
	assertf(217.96875, v, 0.000005);
	dogma_get_ship_attribute(ctx, ATT_CapacitorCapacity, &v);
	assertf(242.1875, v, 0.00005);

	dogma_toggle_chance_based_effect(ctx, imploc0, EFFECT_BoosterShieldCapacityPenalty, false);

	dogma_get_module_attribute(ctx, slot0, ATT_ShieldBonus, &v);
	assertf(39, v, 0.5);
	dogma_get_ship_attribute(ctx, ATT_ShieldCapacity, &v);
	assertf(281.25, v, 0.005);
	dogma_get_ship_attribute(ctx, ATT_CapacitorCapacity, &v);
	assertf(242.1875, v, 0.00005);

	dogma_toggle_chance_based_effect(ctx, imploc0, EFFECT_BoosterCapacitorCapacityPenalty, false);

	dogma_get_module_attribute(ctx, slot0, ATT_ShieldBonus, &v);
	assertf(39, v, 0.5);
	dogma_get_ship_attribute(ctx, ATT_ShieldCapacity, &v);
	assertf(281.25, v, 0.005);
	dogma_get_ship_attribute(ctx, ATT_CapacitorCapacity, &v);
	assertf(312.5, v, 0.05);

	dogma_free_context(ctx);
	return 0;
}
