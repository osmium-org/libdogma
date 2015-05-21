/* libdogma
 * Copyright (C) 2013, 2015 Romain "Artefact2" Dalmaso <artefact2@gmail.com>
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
	dogma_fleet_context_t* fctx;
	double v;
	bool f;
	dogma_key_t slots[5];

	dogma_init();
	dogma_init_context(&ctx);
	dogma_init_fleet_context(&fctx);

	/* 24696:1978;2:2109;2:3520;1:28999;2:29005;2:12818;1:: */
	dogma_set_ship(ctx, TYPE_Harbinger);

	/* Source: Pyfa-1.10.0 */

	dogma_add_fleet_commander(fctx, ctx);

	/* Will remove ctx from fleet command and put it in wing command */
	dogma_add_wing_commander(fctx, 0, ctx);

	/* Will remove ctx from wing command and put it in squad command */
	dogma_add_squad_commander(fctx, 0, 0, ctx);

	/* Etc. */
	dogma_add_squad_member(fctx, 0, 0, ctx);

	/* At this point, the fleet should be just ctx in a squad, and no
	 * commanders or boosters. */
	dogma_get_ship_attribute(ctx, ATT_ScanResolution, &v);
	assertf(262.5, v, 0.05);

	dogma_set_squad_booster(fctx, 0, 0, ctx);

	/* The squad has a booster, but still dosen't have a commander so
	 * no gang boosts should be applied */
	dogma_get_ship_attribute(ctx, ATT_ScanResolution, &v);
	assertf(262.5, v, 0.05);

	/* This time, bonuses should be applied (even though the squad is
	 * empty…) */
	dogma_remove_fleet_member(fctx, ctx, &f);
	assert(f == true);
	dogma_add_squad_commander(fctx, 0, 0, ctx);
	dogma_get_ship_attribute(ctx, ATT_ScanResolution, &v);
	assertf(288.75, v, 0.005);

	dogma_set_squad_booster(fctx, 0, 0, NULL);

	dogma_add_module_sc(ctx, TYPE_HeavyPulseLaserII, &slots[0], DOGMA_STATE_Active, TYPE_ScorchM);
	dogma_add_module_sc(ctx, TYPE_TrackingComputerII, &slots[1], DOGMA_STATE_Active, TYPE_OptimalRangeScript);
	dogma_add_module_sc(ctx, TYPE_TrackingComputerII, &slots[2], DOGMA_STATE_Active, TYPE_OptimalRangeScript);
	dogma_add_module_sc(ctx, TYPE_TrackingDisruptorII, &slots[3], DOGMA_STATE_Active, TYPE_OptimalRangeDisruptionScript);
	dogma_add_module_sc(ctx, TYPE_TrackingDisruptorII, &slots[4], DOGMA_STATE_Active, TYPE_OptimalRangeDisruptionScript);

	dogma_get_module_attribute(ctx, slots[0], ATT_MaxRange, &v);
	assertf(28663.306487, v, 0.0000005);

	/* Self-projected fit, see if anything blows up! */
	dogma_target(ctx, (dogma_location_t){ .type = DOGMA_LOC_Module, .module_index = slots[3] }, ctx);
	dogma_target(ctx, (dogma_location_t){ .type = DOGMA_LOC_Module, .module_index = slots[4] }, ctx);

	/* Pyfa does not allow self-projected fits, this number was
	 * obtained by duplicating the fit then projecting one on the
	 * other */
	dogma_get_module_attribute(ctx, slots[0], ATT_MaxRange, &v);
	assertf(10240.2289768, v, 0.00000005);

	dogma_free_context(ctx);
	dogma_free_fleet_context(fctx);
	return 0;
}
