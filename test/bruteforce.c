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
#include "../src/tables.c"

#include <stdio.h>

#define CAT_Ship 6
#define CAT_Module 7
#define CAT_Subsystem 32

int main(void) {
	dogma_context_t* ctx;
	key_t module_index;
	int i;

	assert(dogma_init() == DOGMA_OK);
	assert(dogma_init_context(&ctx) == DOGMA_OK);

	for(i = 0; dogma_table_types[i].id != 0; ++i) {
		if(dogma_table_types[i].categoryid != CAT_Ship) continue;
		assert(dogma_set_ship(ctx, dogma_table_types[i].id) == DOGMA_OK);
	}

	/* Select a dummy ship to test modules */
	dogma_set_ship(ctx, 587);

	for(i = 0; dogma_table_types[i].id != 0; ++i) {
		if(dogma_table_types[i].categoryid != CAT_Module
		   && dogma_table_types[i].categoryid != CAT_Subsystem) continue;
		assert(dogma_add_module(ctx, dogma_table_types[i].id, &module_index) == DOGMA_OK);
		assert(dogma_set_module_state(ctx, module_index, DOGMA_Overloaded) == DOGMA_OK);
		assert(dogma_remove_module(ctx, module_index) == DOGMA_OK);
	}

	assert(dogma_free_context(ctx) == DOGMA_OK);
	return 0;
}
