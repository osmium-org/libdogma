/* libdogma
 * Copyright (C) 2014 Romain "Artefact2" Dalmaso <artefact2@gmail.com>
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
	dogma_context_t* ctxa[2];
	dogma_key_t keys[2][5];
	dogma_simple_capacitor_t* list;
	size_t llength;

	dogma_init();
	dogma_init_context(&ctxa[0]); /* 23915:3616;3:12219;2:: */
	dogma_init_context(&ctxa[1]); /* 23915:3616;3:12219;2:: */

	for(int i = 0; i < 2; ++i) {
		dogma_context_t* ctx = ctxa[i];
		dogma_set_ship(ctx, TYPE_Chimera);

		for(int j = 0; j < 3; ++j) {
			dogma_add_module_s(
				ctx,
				TYPE_CapitalRemoteShieldBoosterI,
				&keys[i][j],
				DOGMA_STATE_Active
			);
		}
		for(int j = 3; j < 5; ++j) {
			dogma_add_module_s(
				ctx,
				TYPE_CapitalRemoteCapacitorTransmitterI,
				&keys[i][j],
				DOGMA_STATE_Active
			);
		}
		for(int j = 0; j < 5; ++j) {
			dogma_target(
				ctx,
				(dogma_location_t){
					.type = DOGMA_LOC_Module,
					.module_index = keys[i][j],
				},
				ctxa[1-i]
			);
		}
	}

	assert(dogma_get_capacitor_all(
		ctxa[0],
		false,
		&list,
		&llength
	) == DOGMA_OK);

	assert(llength == 2);

	/* Reproduces bug where libdogma reports obviously not stable
	 * loadouts as stable at 0%. */
	assert(list[0].stable == false || list[0].stable_fraction != .0);

	dogma_free_capacitor_list(list);
	dogma_free_context(ctxa[0]);
	dogma_free_context(ctxa[1]);
	return 0;
}
