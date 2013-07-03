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
	bool able, hasit;

	dogma_init();

	assert(dogma_type_has_effect(TYPE_AdaptiveNanoPlatingII, EFFECT_HiPower, &hasit) == DOGMA_OK);
	assert(hasit == false);
	assert(dogma_type_has_effect(TYPE_AdaptiveNanoPlatingII, EFFECT_LoPower, &hasit) == DOGMA_OK);
	assert(hasit == true);

	assert(dogma_type_has_active_effects(TYPE_AdaptiveNanoPlatingII, &able) == DOGMA_OK);
	assert(able == false);
	assert(dogma_type_has_overload_effects(TYPE_AdaptiveNanoPlatingII, &able) == DOGMA_OK);
	assert(able == false);

	assert(dogma_type_has_active_effects(TYPE_DamageControlII, &able) == DOGMA_OK);
	assert(able == true);
	assert(dogma_type_has_overload_effects(TYPE_DamageControlII, &able) == DOGMA_OK);
	assert(able == false);

	assert(dogma_type_has_active_effects(TYPE_1MNAfterburnerII, &able) == DOGMA_OK);
	assert(able == true);
	assert(dogma_type_has_overload_effects(TYPE_1MNAfterburnerII, &able) == DOGMA_OK);
	assert(able == true);



	return 0;
}
