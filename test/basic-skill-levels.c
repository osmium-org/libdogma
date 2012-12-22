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
#include <stdio.h>

#define TYPE_Harbinger 24696
#define TYPE_Electronics 3426
#define TYPE_Engineering 3413
#define ATT_CpuOutput 48
#define ATT_PowerOutput 11

#define EPS 1e-10

int main(void) {
	dogma_context_t* ctx;
	double value;

	assert(dogma_init() == DOGMA_OK);
	assert(dogma_init_context(&ctx) == DOGMA_OK);

	assert(dogma_set_ship(ctx, TYPE_Harbinger) == DOGMA_OK);

	/* Pyfa 1.1.12 (Retribution 1.0) */

	assert(dogma_get_ship_attribute(ctx, ATT_PowerOutput, &value) == DOGMA_OK);
	assertf(value, 1875.00, EPS);
	assert(dogma_get_ship_attribute(ctx, ATT_CpuOutput, &value) == DOGMA_OK);
	assertf(value, 468.75, EPS);

	assert(dogma_set_default_skill_level(ctx, 1) == DOGMA_OK);

	assert(dogma_get_ship_attribute(ctx, ATT_PowerOutput, &value) == DOGMA_OK);
	assertf(value, 1500.00 * 1.05, EPS);
	assert(dogma_get_ship_attribute(ctx, ATT_CpuOutput, &value) == DOGMA_OK);
	assertf(value, 375.00 * 1.05, EPS);

	assert(dogma_set_skill_level(ctx, TYPE_Electronics, 4) == DOGMA_OK);
	assert(dogma_set_skill_level(ctx, TYPE_Engineering, 0) == DOGMA_OK);

	assert(dogma_get_ship_attribute(ctx, ATT_PowerOutput, &value) == DOGMA_OK);
	assertf(value, 1500.00, EPS);
	assert(dogma_get_ship_attribute(ctx, ATT_CpuOutput, &value) == DOGMA_OK);
	assertf(value, 375.00 * 1.20, EPS);

	assert(dogma_set_default_skill_level(ctx, 5) == DOGMA_OK);
	assert(dogma_reset_skill_levels(ctx) == DOGMA_OK);

	assert(dogma_get_ship_attribute(ctx, ATT_PowerOutput, &value) == DOGMA_OK);
	assertf(value, 1875.00, EPS);
	assert(dogma_get_ship_attribute(ctx, ATT_CpuOutput, &value) == DOGMA_OK);
	assertf(value, 468.75, EPS);

	assert(dogma_free_context(ctx) == DOGMA_OK);
	return 0;
}
