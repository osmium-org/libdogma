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

int main(void) {
	dogma_context_t *ctxA, *ctxB;
	dogma_fleet_context_t* fctx;
	dogma_key_t slots[20];
	double v;

	/* Test how fleet bonuses penalize with other modules on the
	 * target ship. */

	dogma_init();
	dogma_init_context(&ctxA);
	dogma_init_context(&ctxB);
	dogma_init_fleet_context(&fctx);

	dogma_add_squad_commander(fctx, 0, 0, ctxB);
	dogma_add_squad_member(fctx, 0, 0, ctxA);

	dogma_set_ship(ctxA, TYPE_Harbinger);
	dogma_set_ship(ctxB, TYPE_Damnation);

	dogma_add_module_s(ctxA, TYPE_EnergizedAdaptiveNanoMembraneII, &slots[0], DOGMA_STATE_Online);
	dogma_add_module_s(ctxA, TYPE_EnergizedAdaptiveNanoMembraneII, &slots[1], DOGMA_STATE_Online);
	dogma_add_module_s(ctxA, TYPE_EnergizedAdaptiveNanoMembraneII, &slots[2], DOGMA_STATE_Online);
	dogma_add_module_s(ctxA, TYPE_ArmorExplosiveHardenerII, &slots[3], DOGMA_STATE_Active);
	dogma_add_module_s(ctxA, TYPE_ArmorKineticHardenerII, &slots[3], DOGMA_STATE_Active);
	dogma_add_module_s(ctxA, TYPE_DamageControlII, &slots[3], DOGMA_STATE_Active);

	dogma_add_module_s(ctxB, TYPE_ArmoredWarfareLinkPassiveDefenseII, &slots[10], DOGMA_STATE_Active);
	dogma_add_module_s(ctxB, TYPE_ArmoredWarfareLinkRapidRepairII, &slots[10], DOGMA_STATE_Active);
	dogma_add_module_s(ctxB, TYPE_ArmoredWarfareLinkDamageControlII, &slots[10], DOGMA_STATE_Active);

	/* Source: Pyfa-42efa48 (Jun 13 2013) */

	dogma_get_ship_attribute(ctxA, ATT_ArmorEmDamageResonance, &v);
	assertf(0.200852324044, v, 0.0000000000005);
	dogma_get_ship_attribute(ctxA, ATT_ArmorExplosiveDamageResonance, &v);
	assertf(0.186459759416, v, 0.0000000000005);
	dogma_get_ship_attribute(ctxA, ATT_ArmorKineticDamageResonance, &v);
	assertf(0.174806024452, v, 0.0000000000005);
	dogma_get_ship_attribute(ctxA, ATT_ArmorThermalDamageResonance, &v);
	assertf(0.261108021257, v, 0.0000000000005);

	dogma_get_ship_attribute(ctxA, ATT_ArmorHP, &v);
	assertf(6875.0, v, 0.05);

	dogma_free_context(ctxA);
	dogma_free_context(ctxB);
	dogma_free_fleet_context(fctx);
	return 0;
}
