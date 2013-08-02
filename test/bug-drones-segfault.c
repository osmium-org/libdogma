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
	double out;

	/* Inspired from a DNA string generated from a carrier kill, which
	 * releaved the issue in the first place. */

	dogma_init();
	dogma_init_context(&ctx);

	/* Oddly enough, for the crash to happen, ALL the lines below have
	 * to be present (if you remove any of them, it dosen't crash) and
	 * the order has to be the same too. */

	dogma_add_drone(ctx, TYPE_InfiltratorI, 1);
	
	dogma_add_drone(ctx, TYPE_AcolyteI, 1);
	dogma_add_drone(ctx, TYPE_MiningDroneI, 1);
	dogma_add_drone(ctx, TYPE_BerserkerI, 1);

	dogma_add_drone(ctx, TYPE_WaspI, 1);
	dogma_remove_drone_partial(ctx, TYPE_WaspI, 1);

	/* This call shouldn't segfault/abort, in fact it shouldn't even return an error. */
	assert(dogma_get_drone_attribute(ctx, TYPE_InfiltratorI, ATT_DroneBandwidthUsed, &out) == DOGMA_OK);

	dogma_free_context(ctx);
	return 0;
}
