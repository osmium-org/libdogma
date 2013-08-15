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
	dogma_key_t key[10];
	double mv;

	dogma_init();
	dogma_init_context(&ctxA);
	dogma_init_context(&ctxB);

	dogma_set_ship(ctxA, TYPE_Myrmidon);
	dogma_set_ship(ctxB, TYPE_Kronos);

	dogma_add_module_s(ctxA, TYPE_MediumTrimarkArmorPumpI, &key[0], DOGMA_STATE_Online);
	dogma_add_module_s(ctxA, TYPE_MediumTrimarkArmorPumpI, &key[1], DOGMA_STATE_Online);
	dogma_add_module_s(ctxA, TYPE_MediumTrimarkArmorPumpI, &key[2], DOGMA_STATE_Online);

	dogma_add_module_s(ctxB, TYPE_TrueSanshaStasisWebifier, &key[3], DOGMA_STATE_Active);

	/* Source: Pyfa-96bb1b1 (Jul 30 2013) */
	dogma_get_ship_attribute(ctxA, ATT_MaxVelocity, &mv);
	assertf(160.006, mv, 0.0005);

	/* Source: Pyfa-96bb1b1 (Jul 30 2013) */
	/* Source: EFT-2.19.1 */
	dogma_target(ctxB, (dogma_location_t){ .type = DOGMA_LOC_Module, .module_index = key[3] }, ctxA);
	dogma_get_ship_attribute(ctxA, ATT_MaxVelocity, &mv);
	assertf(28.5, mv, 1); /* Pyfa says 29.05779, EFT says 28.00105 */

	dogma_free_context(ctxA);
	dogma_free_context(ctxB);
	return 0;
}
