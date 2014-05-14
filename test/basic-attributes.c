/* libdogma
 * Copyright (C) 2012, 2013 Romain "Artefact2" Dalmaso <artefact2@gmail.com>
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
	dogma_context_t* ctx;
	double value;

	assert(dogma_init() == DOGMA_OK);
	assert(dogma_init_context(&ctx) == DOGMA_OK);

	assert(dogma_set_ship(ctx, TYPE_Rifter) == DOGMA_OK);

	assert(dogma_get_ship_attribute(ctx, ATT_HiSlots, &value) == DOGMA_OK);
	assert(value == 4);
	assert(dogma_get_ship_attribute(ctx, ATT_MedSlots, &value) == DOGMA_OK);
	assert(value == 3);
	assert(dogma_get_ship_attribute(ctx, ATT_LowSlots, &value) == DOGMA_OK);
	assert(value == 3);
	assert(dogma_get_ship_attribute(ctx, ATT_UpgradeSlotsLeft, &value) == DOGMA_OK);
	assert(value == 3);
	assert(dogma_get_ship_attribute(ctx, ATT_MaxSubSystems, &value) == DOGMA_OK);
	assert(value == 0);

	assert(dogma_free_context(ctx) == DOGMA_OK);
	return 0;
}
