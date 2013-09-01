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
	dogma_context_t *ctxA, *ctxB, *ctxC;
	dogma_key_t slots[30];
	double v;

	dogma_init();
	dogma_init_context(&ctxA);
	dogma_init_context(&ctxB);
	dogma_init_context(&ctxC);

	/* In this test, we have a Rifter with one autocannon and tracking
	 * enhancers. It is being targeted by another Rifter, and a
	 * Scimitar each with the maximum number of tracking links. */

	dogma_set_ship(ctxA, TYPE_Rifter);
	dogma_add_module_sc(ctxA, TYPE_125mmGatlingAutoCannonII, &slots[0], DOGMA_STATE_Active, TYPE_BarrageS);
	dogma_add_module_s(ctxA, TYPE_TrackingEnhancerII, &slots[1], DOGMA_STATE_Online);
	dogma_add_module_s(ctxA, TYPE_TrackingEnhancerII, &slots[2], DOGMA_STATE_Online);
	dogma_add_module_s(ctxA, TYPE_TrackingEnhancerII, &slots[3], DOGMA_STATE_Online);

	/* Source: Pyfa-42efa48 (Jun 13 2013) */

	dogma_get_module_attribute(ctxA, slots[0], ATT_TrackingSpeed, &v);
	assertf(0.671744203407, v, 0.0000000000005);

	dogma_set_ship(ctxB, TYPE_Rifter);
	dogma_add_module_s(ctxB, TYPE_RemoteTrackingComputerII, &slots[10], DOGMA_STATE_Active);
	dogma_add_module_s(ctxB, TYPE_RemoteTrackingComputerII, &slots[11], DOGMA_STATE_Active);
	dogma_add_module_s(ctxB, TYPE_RemoteTrackingComputerII, &slots[12], DOGMA_STATE_Active);
	dogma_target(ctxB, (dogma_location_t){ .type = DOGMA_LOC_Module, .module_index = slots[10] }, ctxA);
	dogma_target(ctxB, (dogma_location_t){ .type = DOGMA_LOC_Module, .module_index = slots[11] }, ctxA);
	dogma_target(ctxB, (dogma_location_t){ .type = DOGMA_LOC_Module, .module_index = slots[12] }, ctxA);

	/* Source: Pyfa-42efa48 (Jun 13 2013) */
	/* Source: EFT 2.19.1 */

	dogma_get_module_attribute(ctxA, slots[0], ATT_TrackingSpeed, &v);
	assertf(0.789040584043, v, 0.0000000000005);

	dogma_add_charge(ctxB, slots[10], TYPE_TrackingSpeedScript);
	dogma_add_charge(ctxB, slots[11], TYPE_TrackingSpeedScript);
	dogma_add_charge(ctxB, slots[12], TYPE_TrackingSpeedScript);

	/* Source: Pyfa-42efa48 (Jun 13 2013) */

	dogma_get_module_attribute(ctxA, slots[0], ATT_TrackingSpeed, &v);
	assertf(1.07326286262, v, 0.000000000005);

	dogma_set_ship(ctxC, TYPE_Scimitar);
	dogma_add_module_sc(ctxC, TYPE_RemoteTrackingComputerII, &slots[20], DOGMA_STATE_Active, TYPE_TrackingSpeedScript);
	dogma_add_module_sc(ctxC, TYPE_RemoteTrackingComputerII, &slots[21], DOGMA_STATE_Active, TYPE_TrackingSpeedScript);
	dogma_add_module_sc(ctxC, TYPE_RemoteTrackingComputerII, &slots[22], DOGMA_STATE_Active, TYPE_TrackingSpeedScript);
	dogma_add_module_sc(ctxC, TYPE_RemoteTrackingComputerII, &slots[23], DOGMA_STATE_Active, TYPE_TrackingSpeedScript);
	dogma_add_module_sc(ctxC, TYPE_RemoteTrackingComputerII, &slots[24], DOGMA_STATE_Active, TYPE_TrackingSpeedScript);
	dogma_target(ctxC, (dogma_location_t){ .type = DOGMA_LOC_Module, .module_index = slots[20] }, ctxA);
	dogma_target(ctxC, (dogma_location_t){ .type = DOGMA_LOC_Module, .module_index = slots[21] }, ctxA);
	dogma_target(ctxC, (dogma_location_t){ .type = DOGMA_LOC_Module, .module_index = slots[22] }, ctxA);
	dogma_target(ctxC, (dogma_location_t){ .type = DOGMA_LOC_Module, .module_index = slots[23] }, ctxA);
	dogma_target(ctxC, (dogma_location_t){ .type = DOGMA_LOC_Module, .module_index = slots[24] }, ctxA);

	/* Source: Pyfa-42efa48 (Jun 13 2013) */

	dogma_get_module_attribute(ctxA, slots[0], ATT_TrackingSpeed, &v);
	assertf(1.62757988293, v, 0.000000000005);

	dogma_clear_target(ctxC, (dogma_location_t){ .type = DOGMA_LOC_Module, .module_index = slots[20] });
	dogma_clear_target(ctxC, (dogma_location_t){ .type = DOGMA_LOC_Module, .module_index = slots[21] });
	dogma_clear_target(ctxC, (dogma_location_t){ .type = DOGMA_LOC_Module, .module_index = slots[22] });
	dogma_clear_target(ctxC, (dogma_location_t){ .type = DOGMA_LOC_Module, .module_index = slots[23] });
	dogma_clear_target(ctxC, (dogma_location_t){ .type = DOGMA_LOC_Module, .module_index = slots[24] });
	dogma_free_context(ctxC);

	dogma_get_module_attribute(ctxA, slots[0], ATT_TrackingSpeed, &v);
	assertf(1.07326286262, v, 0.000000000005);

	dogma_free_context(ctxA); /* Free the targetee context before the
	                           * targeter context. Oops! */

	/* Now try to remove the tracking links, which may blow up because
	 * it will try to remove the modifier on the target, which doesn't
	 * exist anymore */
	dogma_remove_module(ctxB,slots[10]);
	dogma_remove_module(ctxB,slots[11]);
	dogma_remove_module(ctxB,slots[12]);

	dogma_free_context(ctxB);
	return 0;
}
