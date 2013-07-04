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
	dogma_key_t slots[4];
	double powerload;

	dogma_init();
	dogma_init_context(&ctx);

	dogma_set_ship(ctx, TYPE_Kronos);
	for(size_t i = 0; i < 4; ++i)
		dogma_add_module_s(ctx, TYPE_425mmRailgunII, &slots[i], DOGMA_STATE_Active);

	assert(dogma_get_ship_attribute(ctx, ATT_PowerLoad, &powerload) == DOGMA_OK);
	assert(powerload > 0);

	dogma_set_module_state(ctx, slots[0], DOGMA_STATE_Offline);
	assert(dogma_get_ship_attribute(ctx, ATT_PowerLoad, &powerload) == DOGMA_OK);
	assert(powerload > 0);

	dogma_free_context(ctx);
	return 0;
}
