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
#include <stdio.h>

/* Osmium 0.6.4 (Retribution 1.1) */
#define HARB_BaseCPU 375.00
#define HARB_BasePG 1425.00

#define EPS 1e-10

int main(void) {
	dogma_context_t* ctx;
	double value;

	assert(dogma_init() == DOGMA_OK);
	assert(dogma_init_context(&ctx) == DOGMA_OK);

	assert(dogma_set_ship(ctx, TYPE_Harbinger) == DOGMA_OK);

	assert(dogma_get_ship_attribute(ctx, ATT_PowerOutput, &value) == DOGMA_OK);
	assertf(value, HARB_BasePG * 1.25, EPS);
	assert(dogma_get_ship_attribute(ctx, ATT_CpuOutput, &value) == DOGMA_OK);
	assertf(value, HARB_BaseCPU * 1.25, EPS);

	assert(dogma_set_default_skill_level(ctx, 1) == DOGMA_OK);

	assert(dogma_get_ship_attribute(ctx, ATT_PowerOutput, &value) == DOGMA_OK);
	assertf(value, HARB_BasePG * 1.05, EPS);
	assert(dogma_get_ship_attribute(ctx, ATT_CpuOutput, &value) == DOGMA_OK);
	assertf(value, HARB_BaseCPU * 1.05, EPS);

	assert(dogma_set_skill_level(ctx, TYPE_CPUManagement, 4) == DOGMA_OK);
	assert(dogma_set_skill_level(ctx, TYPE_PowerGridManagement, 0) == DOGMA_OK);

	assert(dogma_get_ship_attribute(ctx, ATT_PowerOutput, &value) == DOGMA_OK);
	assertf(value, HARB_BasePG, EPS);
	assert(dogma_get_ship_attribute(ctx, ATT_CpuOutput, &value) == DOGMA_OK);
	assertf(value, HARB_BaseCPU * 1.20, EPS);

	assert(dogma_set_default_skill_level(ctx, 5) == DOGMA_OK);
	assert(dogma_reset_skill_levels(ctx) == DOGMA_OK);

	assert(dogma_get_ship_attribute(ctx, ATT_PowerOutput, &value) == DOGMA_OK);
	assertf(value, HARB_BasePG * 1.25, EPS);
	assert(dogma_get_ship_attribute(ctx, ATT_CpuOutput, &value) == DOGMA_OK);
	assertf(value, HARB_BaseCPU * 1.25, EPS);

	assert(dogma_free_context(ctx) == DOGMA_OK);
	return 0;
}
