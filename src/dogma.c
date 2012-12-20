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

#include "dogma.h"
#include "dogma_internal.h"
#include "tables.h"

int dogma_init(void) {
	dogma_init_tables();

	return DOGMA_OK;
}

int dogma_init_context(dogma_context_t** ctx) {
	dogma_context_t* new = malloc(sizeof(dogma_context_t));

	new->character.id = 0;
	new->character.children = (array_t)NULL;
	new->character.modifiers = (array_t)NULL;

	new->ship.id = 0;
	new->ship.children = (array_t)NULL;
	new->ship.modifiers = (array_t)NULL;

	new->self = NULL;
	new->other = NULL;
	new->target = NULL;
	new->area = NULL;

	new->default_skill_level = 0;
	new->skillpoints = (array_t)NULL;

	*ctx = new;
	return DOGMA_OK;
}

int dogma_free_context(dogma_context_t* ctx) {
	free(ctx);

	return DOGMA_OK;
}
