/* libdogma
 * Copyright (C) 2012 Romain "Artefact2" Dalmaso <artefact2@gmail.com>
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
#include "../src/dogma_internal.h"
#include "../src/tables-data.c"

int main(void) {
	dogma_context_t* ctx;

	assert(dogma_init() == DOGMA_OK);
	assert(dogma_init_context(&ctx) == DOGMA_OK);

	for(int i = 0; dogma_table_types[i].id != 0; ++i) {
		if(dogma_table_types[i].categoryid != 6) continue;

		assert(dogma_set_ship(ctx, dogma_table_types[i].id) == DOGMA_OK);
	}

	/* Also test postexpressions of the first ship */
	dogma_set_ship(ctx, 0);

	assert(dogma_free_context(ctx) == DOGMA_OK);
	return 0;
}
