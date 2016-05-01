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
#include "bruteforce-data.h"

#include <stdio.h>

dogma_context_t* ctx;
dogma_key_t module_slot, implant_slot;

static void try_all_char_attribs(void);
static void try_all_implant_attribs(void);
static void try_all_skill_attribs(dogma_typeid_t);
static void try_all_ship_attribs(void);
static void try_all_module_attribs(void);
static void try_all_charge_attribs(void);
static void try_all_drone_attribs(dogma_typeid_t);

int main(void) {
	int i;

	dogma_init();
	dogma_init_context(&ctx);

	try_all_char_attribs();
	try_all_skill_attribs(TYPE_Gunnery);

	/* To be perfectly thorough, some of these for loops should be
	 * nested in one another. Try it if you have spare time! */

	for(i = 0; bf_implants[i] != 0; ++i) {
		printf("implant %i\n", bf_implants[i]);
		dogma_add_implant(ctx, bf_implants[i], &implant_slot);
		try_all_char_attribs();
		try_all_implant_attribs();
		dogma_remove_implant(ctx, implant_slot);
	}

	for(i = 0; bf_boosters[i] != 0; ++i) {
		printf("booster %i\n", bf_boosters[i]);
		dogma_add_implant(ctx, bf_boosters[i], &implant_slot);
		try_all_char_attribs();
		try_all_implant_attribs();
		dogma_remove_implant(ctx, implant_slot);
	}

	for(i = 0; bf_drones[i] != 0; ++i) {
		printf("drone %i\n", bf_drones[i]);
		dogma_add_drone(ctx, bf_drones[i], 1);
		try_all_drone_attribs(bf_drones[i]);
		dogma_remove_drone(ctx, bf_drones[i]);
	}

	dogma_add_drone(ctx, bf_drones[0], 1);

	for(i = 0; bf_ships[i] != 0; ++i) {
		printf("ship %i\n", bf_ships[i]);
		dogma_set_ship(ctx, bf_ships[i]);
		try_all_char_attribs();
		try_all_ship_attribs();
	}

	dogma_set_ship(ctx, bf_ships[0]);

	for(i = 0; bf_modules[i] != 0; ++i) {
		printf("module %i\n", bf_modules[i]);
		dogma_add_module(ctx, bf_modules[i], &module_slot);
		dogma_set_module_state(ctx, module_slot, DOGMA_STATE_Overloaded);
		try_all_char_attribs();
		try_all_ship_attribs();
		try_all_drone_attribs(bf_drones[0]);
		try_all_module_attribs();
		dogma_remove_module(ctx, module_slot);
	}

	for(i = 0; bf_subsystems[i] != 0; ++i) {
		printf("subsystems %i\n", bf_subsystems[i]);
		dogma_add_module(ctx, bf_subsystems[i], &module_slot);
		dogma_set_module_state(ctx, module_slot, DOGMA_STATE_Overloaded);
		try_all_char_attribs();
		try_all_ship_attribs();
		try_all_drone_attribs(bf_drones[0]);
		try_all_module_attribs();
		dogma_remove_module(ctx, module_slot);
	}

	dogma_add_module(ctx, bf_modules[0], &module_slot);

	for(i = 0; bf_charges[i] != 0; ++i) {
		printf("charge %i\n", bf_charges[i]);
		dogma_add_charge(ctx, module_slot, bf_charges[i]);
		try_all_char_attribs();
		try_all_ship_attribs();
		try_all_module_attribs();
		try_all_charge_attribs();
		dogma_remove_charge(ctx, module_slot);
	}

	dogma_free_context(ctx);
	return 0;
}

static void try_all_char_attribs(void) {
	int i;
	double v;

	for(i = 0; bf_attributes[i] != 0; ++i) {
		dogma_get_character_attribute(ctx, bf_attributes[i], &v);
	}
}

static void try_all_implant_attribs(void) {
	int i;
	double v;

	for(i = 0; bf_attributes[i] != 0; ++i) {
		dogma_get_implant_attribute(ctx, implant_slot, bf_attributes[i], &v);
	}
}

static void try_all_skill_attribs(dogma_typeid_t id) {
	int i;
	double v;

	for(i = 0; bf_attributes[i] != 0; ++i) {
		dogma_get_skill_attribute(ctx, id, bf_attributes[i], &v);
	}
}


static void try_all_ship_attribs(void) {
	int i;
	double v;

	for(i = 0; bf_attributes[i] != 0; ++i) {
		dogma_get_ship_attribute(ctx, bf_attributes[i], &v);
	}
}

static void try_all_module_attribs(void) {
	int i;
	double v;

	for(i = 0; bf_attributes[i] != 0; ++i) {
		dogma_get_module_attribute(ctx, module_slot, bf_attributes[i], &v);
	}
}

static void try_all_charge_attribs(void) {
	int i;
	double v;

	for(i = 0; bf_attributes[i] != 0; ++i) {
		dogma_get_charge_attribute(ctx, module_slot, bf_attributes[i], &v);
	}
}

static void try_all_drone_attribs(dogma_typeid_t drone) {
	int i;
	double v;

	for(i = 0; bf_attributes[i] != 0; ++i) {
		dogma_get_drone_attribute(ctx, drone, bf_attributes[i], &v);
	}
}
