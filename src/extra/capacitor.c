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

#include "dogma.h"
#include "dogma-extra.h"
#include "dogma-names.h"
#include "dogma_internal.h"
#include "tables.h"
#include <math.h>

/* Number of milliseconds between each integration step, higher value
 * will mean a faster result, at the cost of accuracy. NB: The game
 * servers internally use ticks of 1 second. */
#define DOGMA_CAPACITOR_STEP 5000

/* Maximum time, in milliseconds, to simulate capacitor. Such a limit
 * is necessary because some fits may be cap-unstable but have
 * ridiculously long lasting time, also some fits may be detected as
 * unstable but may work out to be stable after all (because cap usage
 * is not continuous, and the sc_delta method assumes it is). */
#define DOGMA_MAX_CAPACITOR_TIME (DOGMA_CAPACITOR_STEP * 5000)

#define EFFECT_TOUCHES_ENERGY(e) ((e)->id == EFFECT_PowerBooster ||	\
		((e)->dischargeattributeid != 0 && (e)->durationattributeid != 0))

struct dogma_simple_energy_entity_s {
	double cycle_time;
	double amount_used; /* Used energy per cycle, in GJ */
	uint32_t num_cycles_per_reload; /* Typically the number of charges (except crystals) */
	double reload_time;

	bool nooverflow; /* If true, don't do the action if it will
	                  * overflow the capacitor (used by cap
	                  * boosters) */
	bool reloading; /* If true, the module is reloading */

	uint32_t remaining_cycles; /* Remaining cycles before a reload triggers */
	double timer; /* Time left till end of cycle or reload */
};
typedef struct dogma_simple_energy_entity_s dogma_simple_energy_entity_t;


static inline int dogma_fill_entity(dogma_context_t* ctx, dogma_env_t* source,
                                    const dogma_effect_t* e, bool reload,
                                    dogma_simple_energy_entity_t* ent) {
	int num_cycles;

	DOGMA_ASSUME_OK(dogma_get_module_attribute(
		ctx, source->index, e->durationattributeid, &(ent->cycle_time)
	));


	/* TODO: projected energy transfers, projected energy
	 * neutralizers, projected nosferatus (eek) */
	if(e->id == EFFECT_PowerBooster) {
		double bonus;
		int ret;

		ret = dogma_get_charge_attribute(ctx, source->index, ATT_CapacitorBonus, &bonus);
		if(ret == DOGMA_NOT_FOUND) {
			bonus = 0;
		} else if(ret != DOGMA_OK) {
			return ret;
		}

		ent->amount_used = -bonus;
		ent->nooverflow = true;
	} else {
		DOGMA_ASSUME_OK(dogma_get_module_attribute(
			ctx, source->index, e->dischargeattributeid, &(ent->amount_used)
		));
		ent->nooverflow = false;
	}

	if(!reload) {
		ent->num_cycles_per_reload = (uint32_t)-1;
		ent->reload_time = .0;
		return DOGMA_OK;
	}

	DOGMA_ASSUME_OK(dogma_get_number_of_module_cycles_before_reload(ctx, source->index, &num_cycles));

	if(num_cycles == -1) {
		ent->num_cycles_per_reload = (uint32_t)-1;
		ent->reload_time = .0;
		return DOGMA_OK;	
	}

	ent->num_cycles_per_reload = num_cycles;

	DOGMA_ASSUME_OK(dogma_get_module_attribute(
		ctx, source->index, ATT_ReloadTime, &(ent->reload_time)
	));

	return DOGMA_OK;
}

static inline double runge_kutta_step(double capacitor, double capacity, double tau) {
	double c = (capacitor > 0) ? (capacity > 0 ? (capacitor / capacity) : 0) : 0;
	return (sqrt(c) - (c)) * 2 * capacity / tau;
}


int dogma_get_capacitor(dogma_context_t* ctx, bool reload, double* delta, bool* stable, double* s) {
	size_t nentities = 0, i = 0;
	dogma_env_t** m;
	key_t index = 0, sub ;
	array_t effects;
	const dogma_type_effect_t** te;
	const dogma_effect_t* e;
	double global_average_delta = 0, capacity, tau, st_delta, X;

	/* Pass 1: count number of entities */
	JLG(m, ctx->ship->children, index);
	while(m != NULL) {
		/* Check active effects of module with non-zero dischargeattributeid */
		dogma_get_type_effects((*m)->id, &effects);

		sub = 0;
		JLF(te, effects, sub);
		while(te != NULL) {
			DOGMA_ASSUME_OK(dogma_get_effect((*te)->effectid, &e));
			if((((*m)->state >> e->category) & 1) && EFFECT_TOUCHES_ENERGY(e)) {
				++nentities;
			}

			JLN(te, effects, sub);
		}

		JLN(m, ctx->ship->children, index);
	}

	/* Pass 2: allocate VLA and fill it */
	dogma_simple_energy_entity_t entities[nentities];
	index = 0;
	JLG(m, ctx->ship->children, index);
	while(m != NULL) {
		/* Check active effects of module with non-zero dischargeattributeid */
		dogma_get_type_effects((*m)->id, &effects);

		sub = 0;
		JLF(te, effects, sub);
		while(te != NULL) {
			DOGMA_ASSUME_OK(dogma_get_effect((*te)->effectid, &e));

			if((((*m)->state >> e->category) & 1) && EFFECT_TOUCHES_ENERGY(e)) {
				dogma_fill_entity(ctx, *m, e, reload, entities + i);
				entities[i].reloading = false;
				entities[i].remaining_cycles = entities[i].num_cycles_per_reload;
				entities[i].timer = 0;
				global_average_delta += entities[i].amount_used / (
					entities[i].cycle_time + entities[i].reload_time / entities[i].num_cycles_per_reload
				);
				++i;
			}

			JLN(te, effects, sub);
		}

		JLN(m, ctx->ship->children, index);
	}

	/* Pass 3: math */

	/* Base formula taken from: http://wiki.eveuniversity.org/Capacitor_Recharge_Rate */
	DOGMA_ASSUME_OK(dogma_get_ship_attribute(ctx, ATT_CapacitorCapacity, &capacity));
	DOGMA_ASSUME_OK(dogma_get_ship_attribute(ctx, ATT_RechargeRate, &tau));
	tau *= .2;
	if(tau == 0.0) tau = 1.0;

	/* I got the solution for cap stability by solving the quadratic equation:
	   dC   /       C        C   \   2Cmax
	   -- = |sqrt(-----) - ----- | x -----
	   dt   \     Cmax     Cmax  /    Tau

	           Cmax - X*Tau + sqrt(Cmax^2 - 2X*Tau*Cmax)          dC
	   ==> C = ----------------------------------------- with X = --
	                             2                                dt
	   
	   A simple check is that, for dC/dt = 0, the two solutions should be 0 and Cmax. */

	X = (global_average_delta > 0) ? global_average_delta : 0.0;

	/* Peak natural capacitor recharge rate, at 25% capacitor */
	global_average_delta -= (sqrt(.25) - .25) * 2.0 * capacity / tau;
	*delta = global_average_delta;

	/* You may recognize the discriminant of the above equation */
	st_delta = capacity * (capacity - 2 * tau * X);

	/* Technically, if you have zero capacitor, any configuration will
	 * be stable at any percentage, but we want to return false
	 * if there is actual usage. */
	if(capacity == 0.0 && global_average_delta > 0.0) st_delta = -1.0;

	if(st_delta >= 0) {
		/* Discriminant is positive, so there are roots (which are cap
		 * stability points). We pick the highest one because most
		 * users expect to be cap stable above 25% capacitor (but
		 * there is also a stability point below 25%). */
		*stable = true;
		*s = (capacity > 0.0) ? (100 / capacity * (0.5 * (capacity - tau * X + sqrt(st_delta)))) : 100.0;
		return DOGMA_OK;
	}

	/* Discriminant is (strictly) negative, so there are no real
	 * roots. In this case, run a simulation of the capacitor
	 * level over time and see how long it lasts. */
	double t = 0, k1, k2, k3, k4, increment;
	double capacitor = capacity; /* Start simulation with full capacitor */

	while(capacitor >= 0 && t < DOGMA_MAX_CAPACITOR_TIME) {
		increment = 0;

		for(i = 0; i < nentities; ++i) {
			dogma_simple_energy_entity_t* ent = entities + i;

			/* Perform simulation…
			 *
			 * NOTE: the following algorithm will not work for values
			 * of DOGMA_CAPACITOR_STEP so large that a module can
			 * enter reload multiple times per step. */

			ent->timer -= DOGMA_CAPACITOR_STEP;

			if(ent->reloading && ent->timer <= 0) {
				/* Module is done reloading */
				ent->remaining_cycles = ent->num_cycles_per_reload;
				ent->reloading = false;
			}

			if(!ent->reloading) {
				while(ent->timer <= 0 && ent->remaining_cycles > 0) {
					if(ent->nooverflow) {
						double newcap = capacitor + increment + ent->amount_used;
						if(newcap < 0 || newcap > capacity) break;
					}

					--(ent->remaining_cycles);
					ent->timer += ent->cycle_time;
					increment -= ent->amount_used;
				}

				if(ent->remaining_cycles == 0) {
					/* Module starts reloading */
					ent->timer += ent->reload_time;
					ent->reloading = true;
				}
			}
		}

		/* Use RK4 method, which in this case is very similar to the
		 * Simpson's rule */
		k1 = runge_kutta_step(capacitor                                  , capacity, tau);
		k2 = runge_kutta_step(capacitor + 0.5 * DOGMA_CAPACITOR_STEP * k1, capacity, tau);
		k3 = runge_kutta_step(capacitor + 0.5 * DOGMA_CAPACITOR_STEP * k2, capacity, tau);
		k4 = runge_kutta_step(capacitor +       DOGMA_CAPACITOR_STEP * k3, capacity, tau);
		increment += DOGMA_CAPACITOR_STEP * (k1 + k2 + k2 + k3 + k3 + k4) / 6;

		capacitor += increment;
		t += DOGMA_CAPACITOR_STEP;
	}

	/* Finally, use simple linear interpolation to refine the zero position:
	 *
	 * C(t - step) = capacitor - increment    ( ≥ 0 )
	 * C(t)        = capacitor                ( < 0 )
	 *
	 * Assume C(x) = m*x+b in the [ t-step; t ] interval. Then m = increment / step.
	 * Using it in the second line, we get:
	 *
	 * capacitor = (increment / step) * t + b  => b = capacitor - t * (increment / step)
	 *
	 * The zero of C(x) happens at x = -b/a, which is:
	 * (t * (increment / step) - capacitor) * step / increment
	 */
	*s = (t * (increment / DOGMA_CAPACITOR_STEP) - capacitor) * DOGMA_CAPACITOR_STEP / increment;
	*stable = false;
	return DOGMA_OK;

}
