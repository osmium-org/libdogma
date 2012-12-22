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

/* This is dirty, but we need the data. Don't do this in real
 * applications! */
#include "../src/tables.h"
#include "../src/tables.c"

#define CAT_Ship 6

int main(void) {
	dogma_context_t* ctx;
	const dogma_type_t** type;
	key_t index = 0;

	assert(dogma_init() == DOGMA_OK);
	assert(dogma_init_context(&ctx) == DOGMA_OK);

	JLF(type, types_by_id, index);
	while(type != NULL) {
		if((*type)->categoryid != CAT_Ship) continue;
		assert(dogma_set_ship(ctx, (*type)->id) == DOGMA_OK);

		JLN(type, types_by_id, index);
	}

	/* Also test postexpressions of the first ship */
	dogma_set_ship(ctx, 0);

	assert(dogma_free_context(ctx) == DOGMA_OK);
	return 0;
}
