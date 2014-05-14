/* libdogma
 * Copyright (C) 2013 Romain "Artefact2" Dalmaso <artefact2@gmail.com>
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
	dogma_key_t slot;
	dogma_location_t loc;
	dogma_simple_affector_t* aff;
	size_t num_aff1, num_aff2;

	dogma_init();
	dogma_init_context(&ctx);

	dogma_add_implant(ctx, TYPE_StrongBluePillBooster, &slot);
	loc = (dogma_location_t){ .type = DOGMA_LOC_Implant, .implant_index = slot };

	dogma_get_affectors(ctx, (dogma_location_t){ .type = DOGMA_LOC_Ship }, &aff, &num_aff1);
	dogma_free_affector_list(aff);
	dogma_toggle_chance_based_effect(ctx, loc, EFFECT_BoosterShieldCapacityPenalty, true);
	dogma_get_affectors(ctx, (dogma_location_t){ .type = DOGMA_LOC_Ship }, &aff, &num_aff2);
	dogma_free_affector_list(aff);
	assert(num_aff1 + 1 == num_aff2);

	dogma_free_context(ctx);
}
