/* libdogma
 * Copyright (C) 2012, 2013 Romain "Artefact2" Dalmaso <artefact2@gmail.com>
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

/* This is dirty, but we need the data. Don't do this in real
 * applications! */
#include "../src/tables.c"

#include <stdio.h>

#define CAT_Ship 6
#define CAT_Module 7
#define CAT_Charge 8
#define CAT_Drone 18
#define CAT_Subsystem 32

#define TYPE_Drone 2488

dogma_context_t* ctx;
key_t slot;

static void try_all_char_attribs(void);
static void try_all_ship_attribs(void);
static void try_all_module_attribs(void);
static void try_all_charge_attribs(void);
static void try_all_drone_attribs(typeid_t);

int main(void) {
	int i;

	dogma_init();
	dogma_init_context(&ctx);

	try_all_char_attribs();

	/* To be perfectly thorough, some of these for loops should be
	 * nested in one another. Try it if you have spare time! */

	for(i = 0; dogma_table_types[i].id != 0; ++i) {
		if(dogma_table_types[i].categoryid != CAT_Drone) continue;

		dogma_add_drone(ctx, dogma_table_types[i].id, 1);
		try_all_drone_attribs(dogma_table_types[i].id);
		dogma_remove_drone(ctx, dogma_table_types[i].id);
	}

	dogma_add_drone(ctx, TYPE_Drone, 1);

	for(i = 0; dogma_table_types[i].id != 0; ++i) {
		if(dogma_table_types[i].categoryid != CAT_Ship) continue;

		dogma_set_ship(ctx, dogma_table_types[i].id);
		try_all_char_attribs();
		try_all_ship_attribs();
	}

	dogma_set_ship(ctx, 587);

	for(i = 0; dogma_table_types[i].id != 0; ++i) {
		if(dogma_table_types[i].categoryid != CAT_Module
		   && dogma_table_types[i].categoryid != CAT_Subsystem) continue;

		dogma_add_module(ctx, dogma_table_types[i].id, &slot);
		dogma_set_module_state(ctx, slot, DOGMA_Overloaded);
		try_all_char_attribs();
		try_all_ship_attribs();
		try_all_drone_attribs(TYPE_Drone);
		try_all_module_attribs();
		dogma_remove_module(ctx, slot);
	}

	dogma_add_module(ctx, 2873, &slot);

	for(i = 0; dogma_table_types[i].id != 0; ++i) {
		if(dogma_table_types[i].categoryid != CAT_Charge) continue;

		dogma_add_charge(ctx, slot, dogma_table_types[i].id);
		try_all_char_attribs();
		try_all_ship_attribs();
		try_all_module_attribs();
		try_all_charge_attribs();
		dogma_remove_charge(ctx, slot);
	}

	dogma_free_context(ctx);
	return 0;
}

static void try_all_char_attribs(void) {
	int i;
	double v;

	for(i = 0; dogma_table_attributes[i].id != 0; ++i) {
		dogma_get_character_attribute(ctx, dogma_table_attributes[i].id, &v);
	}
}

static void try_all_ship_attribs(void) {
	int i;
	double v;

	for(i = 0; dogma_table_attributes[i].id != 0; ++i) {
		dogma_get_ship_attribute(ctx, dogma_table_attributes[i].id, &v);
	}
}

static void try_all_module_attribs(void) {
	int i;
	double v;

	for(i = 0; dogma_table_attributes[i].id != 0; ++i) {
		dogma_get_module_attribute(ctx, slot, dogma_table_attributes[i].id, &v);
	}
}

static void try_all_charge_attribs(void) {
	int i;
	double v;

	for(i = 0; dogma_table_attributes[i].id != 0; ++i) {
		dogma_get_charge_attribute(ctx, slot, dogma_table_attributes[i].id, &v);
	}
}

static void try_all_drone_attribs(typeid_t drone) {
	int i;
	double v;

	for(i = 0; dogma_table_attributes[i].id != 0; ++i) {
		dogma_get_drone_attribute(ctx, drone, dogma_table_attributes[i].id, &v);
	}
}
