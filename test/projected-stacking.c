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

int main(void) {
	dogma_context_t *ctxA, *ctxB;
	dogma_key_t key[10];
	double mv;

	dogma_init();
	dogma_init_context(&ctxA);
	dogma_init_context(&ctxB);

	/* 24700:31055;3:: */
	dogma_set_ship(ctxA, TYPE_Myrmidon);
	dogma_add_module_s(ctxA, TYPE_MediumTrimarkArmorPumpI, &key[0], DOGMA_STATE_Online);
	dogma_add_module_s(ctxA, TYPE_MediumTrimarkArmorPumpI, &key[1], DOGMA_STATE_Online);
	dogma_add_module_s(ctxA, TYPE_MediumTrimarkArmorPumpI, &key[2], DOGMA_STATE_Online);

	/* 17740:14268;1:: */
	dogma_set_ship(ctxB, TYPE_Vindicator);
	dogma_add_module_s(ctxB, TYPE_TrueSanshaStasisWebifier, &key[3], DOGMA_STATE_Active);

	/* Source: Pyfa-1.1.22 (2014-03-15) */
	dogma_get_ship_attribute(ctxA, ATT_MaxVelocity, &mv);
	assertf(160.00602761, mv, 0.000000005);

	dogma_target(ctxB, (dogma_location_t){ .type = DOGMA_LOC_Module, .module_index = key[3] }, ctxA);
	dogma_get_ship_attribute(ctxA, ATT_MaxVelocity, &mv);
	assertf(29.0577923081, mv, 0.00000000005);

	dogma_free_context(ctxA);
	dogma_free_context(ctxB);
	return 0;
}
