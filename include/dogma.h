/* libdogma
 * Copyright (C) 2012, 2013, 2015 Romain "Artefact2" Dalmaso <artefact2@gmail.com>
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

#pragma once
#ifndef _DOGMA_H
#define _DOGMA_H 1

#include <stdbool.h>
#include <stdint.h>
#include <Judy.h>

/* -------- Disclaimer -------- */

/* Most of the functions in libdogma that operate with types do no
 * checking at all. For example, dogma_set_ship() will not complain if
 * you pass a typeid that is not a ship. If you really want, you could
 * fit a titan skillbook to a piece of tritanium, and then use a
 * planet as a charge, then launch some corpses as drones. If you do,
 * things may blow up in your face!
 *
 * libdogma does NOT check the category of typeids it is given (is it
 * a ship? a module?), nor does it check any overflow (is there enough
 * CPU? powergrid? is there a launcher hardpoint available? etc.). If
 * you care about overflows and such, you will have to do it yourself.
 *
 * To keep the API simple, there are no methods to query what is
 * fitted to a context. You are expected to keep track of what you add
 * and what you remove on your own.
 *
 * A final note, libdogma is licensed under the GNU Affero General
 * Public License version 3 (or later). You are free to use it for
 * your program, but the AGPLv3 has a strong copyleft clause; make
 * sure you understand what it means for you and your program!
 *
 * @note to developers: be very careful when making changes to this
 * file. Do not break the ABI unless it's absolutely necessary.
 */





/* -------- Return codes -------- */

/* Used to indicate success. */
#define DOGMA_OK 0

/* Used when the requested object (type, expression, etc.) does not
 * exist. */
#define DOGMA_NOT_FOUND 1

/* Used when the given parameters do not make sense. */
#define DOGMA_NOT_APPLICABLE 2




/* -------- Data types -------- */

typedef Pvoid_t dogma_array_t;
typedef Word_t dogma_key_t;
typedef uint32_t dogma_typeid_t;
typedef uint16_t dogma_attributeid_t;
typedef int32_t dogma_effectid_t;



enum dogma_location_type_e {
	DOGMA_LOC_Char,
	DOGMA_LOC_Implant,
	DOGMA_LOC_Skill,
	DOGMA_LOC_Ship,
	DOGMA_LOC_Module,
	DOGMA_LOC_Charge,
	DOGMA_LOC_Drone,
};
typedef enum dogma_location_type_e dogma_location_type_t;

/* @note to developers: keep this structure small, it is always passed
 * by value */
struct dogma_location_s {
	dogma_location_type_t type;

	union {
		dogma_key_t implant_index; /* The index generated by
		                            * dogma_add_implant(). */
		dogma_key_t module_index; /* The index generated by
		                           * dogma_add_module(). */
		dogma_typeid_t skill_typeid; /* The typeid of the skillbook */
		dogma_typeid_t drone_typeid; /* The typeid of the drone */
	};
};
typedef struct dogma_location_s dogma_location_t;



enum dogma_state_s {
	/* These values are actually bitmasks: if bit of index i is set,
	 * it means effects with category i should be evaluated. */
	DOGMA_STATE_Unplugged = 0,   /* 0b00000000 */
	DOGMA_STATE_Offline = 1,     /* 0b00000001 */
	DOGMA_STATE_Online = 17,     /* 0b00010001 */
	DOGMA_STATE_Active = 31,     /* 0b00011111 */
	DOGMA_STATE_Overloaded = 63, /* 0b00111111 */
};
typedef enum dogma_state_s dogma_state_t;



/* A dogma context. Usually represents a character with a ship. */
struct dogma_context_s;
typedef struct dogma_context_s dogma_context_t;

/* A dogma fleet context.  */
struct dogma_fleet_context_s;
typedef struct dogma_fleet_context_s dogma_fleet_context_t;





/* -------- General functions -------- */

/* Initialize the static state needed by the library. Must only be
 * called exactly once before any other dogma_* function. */
int dogma_init(void);





/* -------- Context manipulation functions -------- */

/* Create a dogma context. A dogma context consists of a character and
 * his skills, a ship, its fitted modules, etc. By default the
 * character has all skills to level V. */
int dogma_init_context(dogma_context_t**);

/* Free a dogma context previously created by dogma_init_context(). */
int dogma_free_context(dogma_context_t*);



/* Add an effect beacon to the area. */
int dogma_add_area_beacon(dogma_context_t*, dogma_typeid_t, dogma_key_t*);

/* Remove an effect beacon. Use the index generated by
 * dogma_add_area_beacon(). */
int dogma_remove_area_beacon(dogma_context_t*, dogma_key_t);



/* Add an implant or a booster to this context. */
int dogma_add_implant(dogma_context_t*, dogma_typeid_t, dogma_key_t*);

/* Remove an implant (or a booster). Use the index generated by
 * dogma_add_implant(). */
int dogma_remove_implant(dogma_context_t*, dogma_key_t);



/* Set the default skill level of a context. */
int dogma_set_default_skill_level(dogma_context_t*, uint8_t);

/* Override the skill level of a particular skill. */
int dogma_set_skill_level(dogma_context_t*, dogma_typeid_t, uint8_t);

/* Forgets all overriden skill levels. This does not reset the default
 * skill level that was set with dogma_set_default_skill_level(). */
int dogma_reset_skill_levels(dogma_context_t*);



/* Set the ship of a dogma context. Use ID 0 to remove the current
 * ship (but not its fitted modules or anything else). */
int dogma_set_ship(dogma_context_t*, dogma_typeid_t);



/* Add a module to a ship. By default the module will not even be
 * offline, it will have state unplugged (which means that none of its
 * effects are evaluated). */
int dogma_add_module(dogma_context_t*, dogma_typeid_t, dogma_key_t*);

/* Combines the effects of dogma_add_module() followed by
 * dogma_set_module_state(), for brevity in hand-written programs. */
int dogma_add_module_s(dogma_context_t*, dogma_typeid_t, dogma_key_t*, dogma_state_t);

/* Combines the effects of dogma_add_module() followed by
 * dogma_add_charge(), for brevity in hand-written programs. */
int dogma_add_module_c(dogma_context_t*, dogma_typeid_t, dogma_key_t*, dogma_typeid_t);

/* Combines the effects of dogma_add_module(), dogma_set_module_state()
 * then dogma_add_charge(), for brevity in hand-written programs. */
int dogma_add_module_sc(dogma_context_t*, dogma_typeid_t, dogma_key_t*, dogma_state_t, dogma_typeid_t);

/* Remove a module. Use the index generated by dogma_add_module(). */
int dogma_remove_module(dogma_context_t*, dogma_key_t);

/* Set the state of a module. Use the index generated by
 * dogma_add_module(). */
int dogma_set_module_state(dogma_context_t*, dogma_key_t, dogma_state_t);



/* Add a charge to a module. Use the index generated by
 * dogma_add_module(). Added charges automatically have the online
 * state and there is no way to change it. If there is already a
 * charge in the specified module, it will be overwritten. */
int dogma_add_charge(dogma_context_t*, dogma_key_t, dogma_typeid_t);

/* Remove a charge. Use the index generated by dogma_add_module(). No
 * effect if there is no charge in the module. */
int dogma_remove_charge(dogma_context_t*, dogma_key_t);



/* Add a given number of drones to a loadout context. The drones are
 * assumed to be "launched" (ie not in the drone bay doing
 * nothing). */
int dogma_add_drone(dogma_context_t*, dogma_typeid_t, unsigned int);

/* Remove a given number of drones from a loadout context. */
int dogma_remove_drone_partial(dogma_context_t*, dogma_typeid_t, unsigned int);

/* Remove all drones (of a certain type) from the loadout, regardless
 * of how many were added. If trying to remove a nonexistent drone,
 * nothing is done and no error is returned. */
int dogma_remove_drone(dogma_context_t*, dogma_typeid_t);



/* Toggle a chance-based effect (for example, a booster side-effect)
 * on or off. These effects are automatically turned off if the source
 * is removed. */
int dogma_toggle_chance_based_effect(dogma_context_t*, dogma_location_t, dogma_effectid_t, bool);



/* Have something target another ship. Note that a location on a
 * context (usually a module) can only have one target! Calling
 * dogma_target() when a target already exists will silently
 * overwrite the old target. */
int dogma_target(dogma_context_t* targeter, dogma_location_t, dogma_context_t* targetee);

/* Clear a target. This is automatically done if the target disappears
 * (for example if you free the target context). */
int dogma_clear_target(dogma_context_t* targeter, dogma_location_t);





/* -------- Fleet manipulation -------- */

/* Create a new empty fleet. */
int dogma_init_fleet_context(dogma_fleet_context_t**);

/* Free a fleet context created by dogma_create_fleet_context(). This
 * does not free the fleet members, only the fleet itself. */
int dogma_free_fleet_context(dogma_fleet_context_t*);



/* Add a fleet member as fleet commander. This will fail if there is
 * already a fleet commander. If the fleet does not have a booster,
 * the new fleet commander will be used as fleet booster. If the
 * member is already in a different fleet, it will be removed from the
 * old fleet then added to the new one. */
int dogma_add_fleet_commander(dogma_fleet_context_t*, dogma_context_t*);

/* Add a fleet member as wing commander. This will fail if there is
 * already a commander in the same wing. If the wing does not have a
 * booster, the new wing commander will be used as wing booster. If
 * the member is already in a different fleet, it will be removed from
 * the old fleet then added to the new one. */
int dogma_add_wing_commander(dogma_fleet_context_t*, dogma_key_t wing, dogma_context_t*);

/* Add a fleet member as squad commander. This will fail if there is
 * already a commander in the same squad. If the squad does not have a
 * booster, the new squad commander will be used as squad
 * commander. If the member is already in a different fleet, it will
 * be removed from the old fleet then added to the new one. */
int dogma_add_squad_commander(dogma_fleet_context_t*, dogma_key_t wing, dogma_key_t squad, dogma_context_t*);

/* Add a fleet member. If the member is already in a different fleet,
 * it will be removed from the old fleet then added to the new one. */
int dogma_add_squad_member(dogma_fleet_context_t*, dogma_key_t wing, dogma_key_t squad, dogma_context_t*);



/* Remove a fleet member. Found will be set to true if the member was
 * in fleet, or false if not. */
int dogma_remove_fleet_member(dogma_fleet_context_t*, dogma_context_t*, bool* found);



/* Set a booster for the fleet. If there is already a fleet booster,
 * it will be silently overwritten. Assumes the given member is a
 * member of the fleet. You can use NULL to disable the fleet
 * booster. */
int dogma_set_fleet_booster(dogma_fleet_context_t*, dogma_context_t*);

/* Set a booster for the wing. If there is already a wing booster, it
 * will be silently overwritten. Assumes the given member is a member
 * of the wing. You can use NULL to disable the wing booster. */
int dogma_set_wing_booster(dogma_fleet_context_t*, dogma_key_t wing, dogma_context_t*);

/* Set a booster for the squad. If there is already a squad booster,
 * it will be silently overwritten. Assumes the given member is a
 * member of the squad. You can use NULL to disable the squad
 * booster. */
int dogma_set_squad_booster(dogma_fleet_context_t*, dogma_key_t wing, dogma_key_t squad, dogma_context_t*);





/* -------- Attribute getters -------- */

/* Get an attribute of something. All the fuctions below are just
 * shorthands of this version. */
int dogma_get_location_attribute(dogma_context_t*, dogma_location_t, dogma_attributeid_t, double*);

/* Get an attribute of the character. */
int dogma_get_character_attribute(dogma_context_t*, dogma_attributeid_t, double*);

/* Get an attribute of an implant (or booster). Use the index
 * generated by dogma_add_implant(). */
int dogma_get_implant_attribute(dogma_context_t*, dogma_key_t, dogma_attributeid_t, double*);

/* Get an attribute of a skill. Use the typeid of the skillbook. */
int dogma_get_skill_attribute(dogma_context_t*, dogma_typeid_t, dogma_attributeid_t, double*);

/* Get an attribute of the ship. */
int dogma_get_ship_attribute(dogma_context_t*, dogma_attributeid_t, double*);

/* Get an attribute of a module. Use the index generated by
 * dogma_add_module(). */
int dogma_get_module_attribute(dogma_context_t*, dogma_key_t, dogma_attributeid_t, double*);

/* Get an attribute of a charge. Use the index generated by
 * dogma_add_module(). */
int dogma_get_charge_attribute(dogma_context_t*, dogma_key_t, dogma_attributeid_t, double*);

/* Get an attribute of a drone. */
int dogma_get_drone_attribute(dogma_context_t*, dogma_typeid_t, dogma_attributeid_t, double*);



/* Get the chance (between 0 (never) and 1 (always)) of a chance-based
 * effect. */
int dogma_get_chance_based_effect_chance(dogma_context_t*, dogma_location_t, dogma_effectid_t, double*);





#endif
