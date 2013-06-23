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
	dogma_context_t* ctx;
	dogma_key_t slot;
	double v;

	dogma_init();
	dogma_init_context(&ctx);

	/* Source: Pyfa-42efa48 (Jun 13 2013) */

	dogma_set_ship(ctx, TYPE_Golem);
	dogma_add_module(ctx, TYPE_LargeAncillaryShieldBooster, &slot);
	dogma_set_module_state(ctx, slot, DOGMA_STATE_Active);

	dogma_get_module_attribute(ctx, slot, ATT_CapacitorNeed, &v);
	assertf(475.2, v, 0.05);
	dogma_get_module_attribute(ctx, slot, ATT_ShieldBonus, &v);
	assertf(536.25, v, 0.005);

	dogma_add_charge(ctx, slot, TYPE_CapBooster150);
	dogma_get_module_attribute(ctx, slot, ATT_CapacitorNeed, &v);
	assertf(0.0, v, 0.0);
	dogma_get_module_attribute(ctx, slot, ATT_ShieldBonus, &v);
	assertf(536.25, v, 0.005);

	dogma_remove_charge(ctx, slot);
	dogma_get_module_attribute(ctx, slot, ATT_CapacitorNeed, &v);
	assertf(475.2, v, 0.05);
	dogma_get_module_attribute(ctx, slot, ATT_ShieldBonus, &v);
	assertf(536.25, v, 0.005);

	dogma_free_context(ctx);

	dogma_init_context(&ctx);
	dogma_set_ship(ctx, TYPE_Kronos);
	dogma_add_module(ctx, TYPE_LargeAncillaryArmorRepairer, &slot);
	dogma_set_module_state(ctx, slot, DOGMA_STATE_Active);

	dogma_get_module_attribute(ctx, slot, ATT_CapacitorNeed, &v);
	assertf(400.0, v, 0.05);
	dogma_get_module_attribute(ctx, slot, ATT_ArmorDamageAmount, &v);
	assertf(618.75, v, 0.005);

	dogma_add_charge(ctx, slot, TYPE_NaniteRepairPaste);
	dogma_get_module_attribute(ctx, slot, ATT_CapacitorNeed, &v);
	assertf(400.0, v, 0.05);
	dogma_get_module_attribute(ctx, slot, ATT_ArmorDamageAmount, &v);
	assertf(1856.25, v, 0.005);

	dogma_remove_charge(ctx, slot);
	dogma_get_module_attribute(ctx, slot, ATT_CapacitorNeed, &v);
	assertf(400.0, v, 0.05);
	dogma_get_module_attribute(ctx, slot, ATT_ArmorDamageAmount, &v);
	assertf(618.75, v, 0.005);

	dogma_free_context(ctx);
	return 0;
}
