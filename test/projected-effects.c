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

#define TYPE_Rifter 587
#define TYPE_Scimitar 11978
#define TYPE_TrackingLinkII 2104
#define TYPE_TrackingSpeedScript 29001
#define TYPE_125mmAutocannonII  2873
#define TYPE_BarrageS 12625
#define TYPE_TrackingEnhancerII 1999

#define ATT_TrackingSpeed 160

int main(void) {
	dogma_context_t *ctxA, *ctxB, *ctxC;
	key_t slots[30];
	double v;

	dogma_init();
	dogma_init_context(&ctxA);
	dogma_init_context(&ctxB);
	dogma_init_context(&ctxC);

	/* In this test, we have a Rifter with one autocannon and tracking
	 * enhancers. It is being targeted by another Rifter, and a
	 * Scimitar each with the maximum number of tracking links. */

	dogma_set_ship(ctxA, TYPE_Rifter);
	dogma_add_module(ctxA, TYPE_125mmAutocannonII, &slots[0]);
	dogma_add_charge(ctxA, slots[0], TYPE_BarrageS);
	dogma_add_module(ctxA, TYPE_TrackingEnhancerII, &slots[1]);
	dogma_add_module(ctxA, TYPE_TrackingEnhancerII, &slots[2]);
	dogma_add_module(ctxA, TYPE_TrackingEnhancerII, &slots[3]);
	dogma_set_module_state(ctxA, slots[0], DOGMA_Active);
	dogma_set_module_state(ctxA, slots[1], DOGMA_Online);
	dogma_set_module_state(ctxA, slots[2], DOGMA_Online);
	dogma_set_module_state(ctxA, slots[3], DOGMA_Online);


	/* Source: Pyfa-42efa48 (Jun 13 2013) */

	dogma_get_module_attribute(ctxA, slots[0], ATT_TrackingSpeed, &v);
	assertf(0.671744203407, v, 0.0000000000005);

	dogma_set_ship(ctxB, TYPE_Rifter);
	dogma_add_module(ctxB, TYPE_TrackingLinkII, &slots[10]);
	dogma_add_module(ctxB, TYPE_TrackingLinkII, &slots[11]);
	dogma_add_module(ctxB, TYPE_TrackingLinkII, &slots[12]);
	dogma_set_module_state(ctxB, slots[10], DOGMA_Active);
	dogma_set_module_state(ctxB, slots[11], DOGMA_Active);
	dogma_set_module_state(ctxB, slots[12], DOGMA_Active);
	dogma_target(ctxB, (location_t){ .type = DOGMA_LOC_Module, .module_index = slots[10] }, ctxA);
	dogma_target(ctxB, (location_t){ .type = DOGMA_LOC_Module, .module_index = slots[11] }, ctxA);
	dogma_target(ctxB, (location_t){ .type = DOGMA_LOC_Module, .module_index = slots[12] }, ctxA);

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
	dogma_add_module(ctxC, TYPE_TrackingLinkII, &slots[20]);
	dogma_add_module(ctxC, TYPE_TrackingLinkII, &slots[21]);
	dogma_add_module(ctxC, TYPE_TrackingLinkII, &slots[22]);
	dogma_add_module(ctxC, TYPE_TrackingLinkII, &slots[23]);
	dogma_add_module(ctxC, TYPE_TrackingLinkII, &slots[24]);
	dogma_add_charge(ctxC, slots[20], TYPE_TrackingSpeedScript);
	dogma_add_charge(ctxC, slots[21], TYPE_TrackingSpeedScript);
	dogma_add_charge(ctxC, slots[22], TYPE_TrackingSpeedScript);
	dogma_add_charge(ctxC, slots[23], TYPE_TrackingSpeedScript);
	dogma_add_charge(ctxC, slots[24], TYPE_TrackingSpeedScript);
	dogma_set_module_state(ctxC, slots[20], DOGMA_Active);
	dogma_set_module_state(ctxC, slots[21], DOGMA_Active);
	dogma_set_module_state(ctxC, slots[22], DOGMA_Active);
	dogma_set_module_state(ctxC, slots[23], DOGMA_Active);
	dogma_set_module_state(ctxC, slots[24], DOGMA_Active);
	dogma_target(ctxC, (location_t){ .type = DOGMA_LOC_Module, .module_index = slots[20] }, ctxA);
	dogma_target(ctxC, (location_t){ .type = DOGMA_LOC_Module, .module_index = slots[21] }, ctxA);
	dogma_target(ctxC, (location_t){ .type = DOGMA_LOC_Module, .module_index = slots[22] }, ctxA);
	dogma_target(ctxC, (location_t){ .type = DOGMA_LOC_Module, .module_index = slots[23] }, ctxA);
	dogma_target(ctxC, (location_t){ .type = DOGMA_LOC_Module, .module_index = slots[24] }, ctxA);

	/* Source: Pyfa-42efa48 (Jun 13 2013) */

	dogma_get_module_attribute(ctxA, slots[0], ATT_TrackingSpeed, &v);
	assertf(1.62757988293, v, 0.000000000005);

	dogma_clear_target(ctxC, (location_t){ .type = DOGMA_LOC_Module, .module_index = slots[20] });
	dogma_clear_target(ctxC, (location_t){ .type = DOGMA_LOC_Module, .module_index = slots[21] });
	dogma_clear_target(ctxC, (location_t){ .type = DOGMA_LOC_Module, .module_index = slots[22] });
	dogma_clear_target(ctxC, (location_t){ .type = DOGMA_LOC_Module, .module_index = slots[23] });
	dogma_clear_target(ctxC, (location_t){ .type = DOGMA_LOC_Module, .module_index = slots[24] });
	dogma_free_context(ctxC);

	dogma_get_module_attribute(ctxA, slots[0], ATT_TrackingSpeed, &v);
	assertf(1.07326286262, v, 0.000000000005);

	dogma_clear_target(ctxB, (location_t){ .type = DOGMA_LOC_Module, .module_index = slots[10] });
	dogma_clear_target(ctxB, (location_t){ .type = DOGMA_LOC_Module, .module_index = slots[11] });
	dogma_clear_target(ctxB, (location_t){ .type = DOGMA_LOC_Module, .module_index = slots[12] });
	dogma_free_context(ctxB);

	dogma_get_module_attribute(ctxA, slots[0], ATT_TrackingSpeed, &v);
	assertf(0.671744203407, v, 0.0000000000005);

	dogma_free_context(ctxA);
	return 0;
}
