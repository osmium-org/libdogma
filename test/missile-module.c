/* libdogma
 * Copyright (C) 2015 Romain "Artefact2" Dalmaso <artefact2@gmail.com>
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
	dogma_key_t slot0, slot1;
	double v1, v2;

	dogma_init();
	dogma_init_context(&ctx);

	assertok(dogma_add_module_sc(ctx, TYPE_LightMissileLauncherI, &slot0, DOGMA_STATE_Active, TYPE_ScourgeLightMissile));
	assertok(dogma_get_charge_attribute(ctx, slot0, ATT_ExplosionDelay, &v1));

	assertok(dogma_add_module_s(ctx, TYPE_MissileGuidanceComputerI, &slot1, DOGMA_STATE_Active));
	assertok(dogma_get_charge_attribute(ctx, slot0, ATT_ExplosionDelay, &v2));

	assert(v2 > v1);

	dogma_free_context(ctx);
	return 0;
}
