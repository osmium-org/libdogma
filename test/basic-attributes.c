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

#define TYPE_Rifter (typeid_t)587
#define ATT_HighSlots (attributeid_t)14
#define ATT_MediumSlots (attributeid_t)13
#define ATT_LowSlots (attributeid_t)12
#define ATT_RigSlots (attributeid_t)1154
#define ATT_SubsystemSlots (attributeid_t)1367

int main(void) {
	dogma_context_t* ctx;
	double value;

	assert(dogma_init() == DOGMA_OK);
	assert(dogma_init_context(&ctx) == DOGMA_OK);

	assert(dogma_set_ship(ctx, TYPE_Rifter) == DOGMA_OK);

	assert(dogma_get_ship_attribute(ctx, ATT_HighSlots, &value) == DOGMA_OK);
	assert(value == 4);
	assert(dogma_get_ship_attribute(ctx, ATT_MediumSlots, &value) == DOGMA_OK);
	assert(value == 3);
	assert(dogma_get_ship_attribute(ctx, ATT_LowSlots, &value) == DOGMA_OK);
	assert(value == 3);
	assert(dogma_get_ship_attribute(ctx, ATT_RigSlots, &value) == DOGMA_OK);
	assert(value == 3);
	assert(dogma_get_ship_attribute(ctx, ATT_SubsystemSlots, &value) == DOGMA_OK);
	assert(value == 0);

	assert(dogma_free_context(ctx) == DOGMA_OK);
	return 0;
}
