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
	dogma_key_t slots[10];
	bool able;

	dogma_init();
	dogma_init_context(&ctx);

	dogma_set_ship(ctx, TYPE_Punisher);
	dogma_add_module(ctx, TYPE_AdaptiveNanoPlatingII, &slots[0]);
	dogma_add_module(ctx, TYPE_DamageControlII, &slots[1]);
	dogma_add_module(ctx, TYPE_1MNAfterburnerII, &slots[2]);

	assert(dogma_location_has_active_effects(ctx, (dogma_location_t){
			.type = DOGMA_LOC_Module, .module_index = slots[0]
					}, &able) == DOGMA_OK);
	assert(able == false);
	assert(dogma_location_has_overload_effects(ctx, (dogma_location_t){
			.type = DOGMA_LOC_Module, .module_index = slots[0]
					}, &able) == DOGMA_OK);
	assert(able == false);

	assert(dogma_location_has_active_effects(ctx, (dogma_location_t){
			.type = DOGMA_LOC_Module, .module_index = slots[1]
					}, &able) == DOGMA_OK);
	assert(able == true);
	assert(dogma_location_has_overload_effects(ctx, (dogma_location_t){
			.type = DOGMA_LOC_Module, .module_index = slots[1]
					}, &able) == DOGMA_OK);
	assert(able == false);

	assert(dogma_location_has_active_effects(ctx, (dogma_location_t){
			.type = DOGMA_LOC_Module, .module_index = slots[2]
					}, &able) == DOGMA_OK);
	assert(able == true);
	assert(dogma_location_has_overload_effects(ctx, (dogma_location_t){
			.type = DOGMA_LOC_Module, .module_index = slots[2]
					}, &able) == DOGMA_OK);
	assert(able == true);

	dogma_free_context(ctx);
	return 0;
}
