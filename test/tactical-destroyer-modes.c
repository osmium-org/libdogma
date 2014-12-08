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
	dogma_context_t* ctx;
	dogma_key_t k;
	double v0, v1;

	assert(dogma_init() == DOGMA_OK);
	assert(dogma_init_context(&ctx) == DOGMA_OK);

	assert(dogma_set_ship(ctx, TYPE_Confessor) == DOGMA_OK);

	assert(dogma_get_ship_attribute(ctx, ATT_MaxVelocity, &v0) == DOGMA_OK);
	assert(dogma_add_module_s(ctx, TYPE_AmarrTacticalDestroyerPropulsionMode, &k, DOGMA_STATE_Online) == DOGMA_OK);
	assert(dogma_get_ship_attribute(ctx, ATT_MaxVelocity, &v1) == DOGMA_OK);

	printf("%f %f\n", v0, v1);
	
	assert(v1 > v0);

	assert(dogma_free_context(ctx) == DOGMA_OK);
	return 0;
}
