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
#include <assert.h>

/* Number of milliseconds between each integration step, higher value
 * will mean a faster result, at the cost of accuracy. NB: The game
 * servers internally use ticks of 1 second. */
#define DOGMA_CAPACITOR_STEP 5000



#define EFFECT_TOUCHES_ENERGY(e) (	  \
	(e)->id == EFFECT_PowerBooster \
	|| (e)->id == EFFECT_EnergyTransfer \
	|| (e)->id == EFFECT_EnergyDestabilizationNew \
	|| (e)->id == EFFECT_Leech \
	|| ((e)->dischargeattributeid != 0 && (e)->durationattributeid != 0) \
)

#define EFFECT_AFFECTED_BY_TARGET_CAPACITOR(e) (	  \
	(e)->id == EFFECT_Leech \
)


struct dogma_simple_energy_pool_s {
	dogma_context_t* context;

	double capacity; /* Maximum capacity */
	double tau; /* Tau, defines how fast the pool naturally
	             * recharges */

	double current; /* The current level (0 <= current <= capacity) */
	double delta; /* The amount of energy consumed by modules in one
	               * second */

	double max_affector_cycle_time; /* The longest duration of an
	                                 * entity affecting this pool. */
	double min; /* Minimum level */
	double time_since_min; /* When was min last reached? */

	double starvation_time; /* After how many milliseconds did this
	                         * pool run out of capacitor? (Negative
	                         * value if not applicable) */
};
typedef struct dogma_simple_energy_pool_s dogma_simple_energy_pool_t;



enum dogma_simple_energy_entity_type_e {
	DOGMA_EENT_Simple,
	DOGMA_EENT_Powerboost,
	DOGMA_EENT_Transfer,
	DOGMA_EENT_Neutralization,
	DOGMA_EENT_Leech,
};
typedef enum dogma_simple_energy_entity_type_e dogma_simple_energy_entity_type_t;



struct dogma_simple_energy_entity_s {
	dogma_simple_energy_entity_type_t type;
	dogma_simple_energy_pool_t* location;
	dogma_simple_energy_pool_t* target;
	double cycle_time;
	double amount_used; /* Used energy per cycle, in GJ */
	double other_amount; /* Amount restored, transferred, neutralized
	                      * or leeched depending on type */
	uint32_t num_cycles_per_reload; /* Typically the number of charges
	                                 * (except crystals) */
	double reload_time;
	bool reloading; /* If true, the module is reloading */
	uint32_t remaining_cycles; /* Remaining cycles before a reload
	                            * triggers */
	double timer; /* Time left till end of cycle or reload */
};
typedef struct dogma_simple_energy_entity_s dogma_simple_energy_entity_t;



static inline int dogma_fill_entity(dogma_context_t* ctx, dogma_env_t* source,
                                    const dogma_effect_t* e, bool reload,
                                    dogma_simple_energy_entity_t* ent) {
	int num_cycles, ret;

	DOGMA_ASSUME_OK(dogma_get_module_attribute(
		ctx, source->index, e->durationattributeid, &(ent->cycle_time)
	));

	if(!reload) {
		ent->num_cycles_per_reload = (uint32_t)-1;
		ent->reload_time = .0;
	} else {
		DOGMA_ASSUME_OK(dogma_get_number_of_module_cycles_before_reload(ctx, source->index, &num_cycles));

		if(num_cycles == -1) {
			ent->num_cycles_per_reload = (uint32_t)-1;
			ent->reload_time = .0;
		} else {
			ent->num_cycles_per_reload = num_cycles;

			DOGMA_ASSUME_OK(dogma_get_module_attribute(
				ctx, source->index, ATT_ReloadTime, &(ent->reload_time)
			));
		}
	}

	switch(e->id) {

	case EFFECT_PowerBooster:
		ret = dogma_get_charge_attribute(ctx, source->index, ATT_CapacitorBonus, &(ent->other_amount));
		if(ret == DOGMA_NOT_FOUND) {
			ent->other_amount = 0;
		} else if(ret != DOGMA_OK) {
			return ret;
		}

		ent->type = DOGMA_EENT_Powerboost;
		ent->amount_used = 0.0;
		ent->location->delta -= ent->other_amount /
			(ent->cycle_time + ent->reload_time / ent->num_cycles_per_reload);
		break;

	case EFFECT_EnergyTransfer:
		DOGMA_ASSUME_OK(dogma_get_module_attribute(
			ctx, source->index, e->dischargeattributeid, &(ent->amount_used)
		));

		DOGMA_ASSUME_OK(dogma_get_module_attribute(
			ctx, source->index, ATT_PowerTransferAmount, &(ent->other_amount)
		));

		ent->type = DOGMA_EENT_Transfer;
		ent->location->delta += ent->amount_used /
			(ent->cycle_time + ent->reload_time / ent->num_cycles_per_reload);
		if(ent->target != NULL) {
			ent->target->delta -= ent->other_amount /
			(ent->cycle_time + ent->reload_time / ent->num_cycles_per_reload);
		}
		break;

	case EFFECT_EnergyDestabilizationNew: /* TODO: reflect thingy */
		DOGMA_ASSUME_OK(dogma_get_module_attribute(
			ctx, source->index, e->dischargeattributeid, &(ent->amount_used)
		));

		DOGMA_ASSUME_OK(dogma_get_module_attribute(
			ctx, source->index, ATT_EnergyDestabilizationAmount, &(ent->other_amount)
		));

		ent->type = DOGMA_EENT_Neutralization;
		ent->location->delta += ent->amount_used /
			(ent->cycle_time + ent->reload_time / ent->num_cycles_per_reload);
		if(ent->target != NULL) {
			ent->target->delta += ent->other_amount /
			(ent->cycle_time + ent->reload_time / ent->num_cycles_per_reload);
		}
		break;

	case EFFECT_Leech: /* TODO: reflect thingy */
		DOGMA_ASSUME_OK(dogma_get_module_attribute(
			ctx, source->index, ATT_PowerTransferAmount, &(ent->other_amount)
		));

		ent->type = DOGMA_EENT_Leech;
		ent->amount_used = 0.0;
		ent->location->delta += ent->other_amount /
			(ent->cycle_time + ent->reload_time / ent->num_cycles_per_reload);
		if(ent->target != NULL) {
			ent->target->delta -= ent->other_amount /
			(ent->cycle_time + ent->reload_time / ent->num_cycles_per_reload);
		}
		break;

	default:
		DOGMA_ASSUME_OK(dogma_get_module_attribute(
			ctx, source->index, e->dischargeattributeid, &(ent->amount_used)
		));

		ent->type = DOGMA_EENT_Simple;
		ent->other_amount = 0.0;
		ent->location->delta += ent->amount_used /
			(ent->cycle_time + ent->reload_time / ent->num_cycles_per_reload);
		break;

	}

	return DOGMA_OK;
}



/* Count the number of entities and fills target maps.
 *
 * @param pool_map initialise to NULL, dogma_context_t* -> 0, later dogma_simple_energy_pool_t*
 */
static inline int dogma_capacitor_count_entities_and_pools(
	dogma_context_t* ctx,
	size_t* n_entities,
	array_t* pool_map) {

	key_t index = 0, sub;
	dogma_env_t** m;
	array_t effects;
	const dogma_type_effect_t** te;
	const dogma_effect_t* e;
	void** v;
	dogma_context_t** targeter;

	JLG(v, *pool_map, (intptr_t)ctx);
	if(v != NULL) {
		/* Already went there */
		return DOGMA_OK;
	}

	JLI(v, *pool_map, (intptr_t)ctx);
	*v = NULL;

	JLF(m, ctx->ship->children, index);
	while(m != NULL) {
		/* Check active effects of module with non-zero dischargeattributeid */
		dogma_get_type_effects((*m)->id, &effects);

		sub = 0;
		JLF(te, effects, sub);
		while(te != NULL) {
			DOGMA_ASSUME_OK(dogma_get_effect((*te)->effectid, &e));
			if((((*m)->state >> e->category) & 1) && EFFECT_TOUCHES_ENERGY(e)) {
				++(*n_entities);

				if(EFFECT_AFFECTED_BY_TARGET_CAPACITOR(e) && (*m)->target.context != NULL) {
					DOGMA_ASSUME_OK(dogma_capacitor_count_entities_and_pools(
						(*m)->target.context, n_entities, pool_map
					));
				}
			}

			JLN(te, effects, sub);
		}

		JLN(m, ctx->ship->children, index);
	}

	index = 0;
	JLF(targeter, ctx->ship->targeted_by, index);
	while(targeter != NULL) {
		DOGMA_ASSUME_OK(dogma_capacitor_count_entities_and_pools(
			*targeter, n_entities, pool_map
		));

		JLN(targeter, ctx->ship->targeted_by, index);
	}

	return DOGMA_OK;
}

static inline int dogma_capacitor_fill_entities_and_pools(
	dogma_context_t* ctx, array_t* pool_map, bool reload,
	dogma_simple_energy_pool_t* pools, dogma_simple_energy_entity_t* entities,
	size_t* pool_offset, size_t* entity_offset) {

	key_t index = 0, sub;
	dogma_env_t** m;
	array_t effects;
	const dogma_type_effect_t** te;
	const dogma_effect_t* e;
	dogma_simple_energy_pool_t** v;
	dogma_context_t** targeter;
	dogma_simple_energy_pool_t* loc;

	JLG(v, *pool_map, (intptr_t)ctx);
	assert(v != NULL);

	if(*v != NULL) {
		return DOGMA_OK;
	}

	loc = *v = pools + *pool_offset;

	loc->context = ctx;
	DOGMA_ASSUME_OK(dogma_get_ship_attribute(ctx, ATT_CapacitorCapacity, &(loc->capacity)));
	DOGMA_ASSUME_OK(dogma_get_ship_attribute(ctx, ATT_RechargeRate, &(loc->tau)));
	loc->tau *= .2;
	loc->current = loc->capacity;
	loc->delta = 0.0;
	loc->max_affector_cycle_time = 0.0;
	loc->min = loc->capacity;
	loc->time_since_min = 0.0;
	loc->starvation_time = -1.0;

	++(*pool_offset);

	JLF(targeter, ctx->ship->targeted_by, index);
	while(targeter != NULL) {
		DOGMA_ASSUME_OK(dogma_capacitor_fill_entities_and_pools(
			*targeter, pool_map, reload, pools, entities, pool_offset, entity_offset
		));

		JLN(targeter, ctx->ship->targeted_by, index);
	}

	index = 0;

	JLF(m, ctx->ship->children, index);
	while(m != NULL) {
		dogma_get_type_effects((*m)->id, &effects);

		sub = 0;
		JLF(te, effects, sub);
		while(te != NULL) {
			DOGMA_ASSUME_OK(dogma_get_effect((*te)->effectid, &e));
			if((((*m)->state >> e->category) & 1) && EFFECT_TOUCHES_ENERGY(e)) {
				if(EFFECT_AFFECTED_BY_TARGET_CAPACITOR(e) && (*m)->target.context != NULL) {
					DOGMA_ASSUME_OK(dogma_capacitor_fill_entities_and_pools(
						(*m)->target.context, pool_map, reload, pools, entities, pool_offset, entity_offset
					));
				}

				entities[*entity_offset].location = loc;

				if((*m)->target.context != NULL) {
					dogma_simple_energy_pool_t** targetv;
					JLG(targetv, *pool_map, (intptr_t)((*m)->target.context));
					entities[*entity_offset].target = (targetv != NULL) ? *targetv : NULL;
				} else {
					entities[*entity_offset].target = NULL;
				}

				DOGMA_ASSUME_OK(dogma_fill_entity(ctx, *m, e, reload, entities + *entity_offset));

				entities[*entity_offset].reloading = false;
				entities[*entity_offset].remaining_cycles = entities[*entity_offset].num_cycles_per_reload;
				entities[*entity_offset].timer = 0;

				++(*entity_offset);
			}

			JLN(te, effects, sub);
		}

		JLN(m, ctx->ship->children, index);
	}

	return DOGMA_OK;
}





static inline double runge_kutta_step(double capacitor, double capacity, double tau) {
	double c = (capacitor > 0) ? (capacity > 0 ? (capacitor / capacity) : 0) : 0;
	return (sqrt(c) - (c)) * 2 * capacity / tau;
}


int dogma_get_capacitor(dogma_context_t* ctx, bool reload, double* delta, bool* stable, double* s) {
	dogma_simple_capacitor_t* list;
	size_t len;
	bool found = false;

	DOGMA_ASSUME_OK(dogma_get_capacitor_all(ctx, reload, &list, &len));

	for(size_t i = 0; i < len; ++i) {
		if(list[i].context == ctx) {
			*delta = list[i].delta;
			*stable = list[i].stable;

			if(list[i].stable) {
				*s = 100.0 * list[i].stable_fraction;
			} else {
				*s = list[i].depletion_time;
			}

			found = true;
			break;
		}
	}

	dogma_free_capacitor_list(list);

	assert(found);
	return DOGMA_OK;
}

int dogma_get_capacitor_all(dogma_context_t* ctx, bool reload, dogma_simple_capacitor_t** list, size_t* len) {
	size_t n_entities = 0, n_pools = 0, off_entity = 0, off_pool = 0, n;
	array_t pool_map = NULL;
	unsigned int changed;
	int ret;
	double elapsed = 0;
	double k1, k2, k3, k4;

	/* Pass 1: count number of entities */
	DOGMA_ASSUME_OK(
		dogma_capacitor_count_entities_and_pools(ctx, &n_entities, &pool_map)
	);
	JLC(n_pools, pool_map, 0, -1);
	*len = n_pools;

	/* Pass 2: allocate stuff */
	dogma_simple_energy_pool_t pools[n_pools];
	dogma_simple_energy_entity_t entities[n_entities];

	/* Pass 3: fill them! */
	DOGMA_ASSUME_OK(dogma_capacitor_fill_entities_and_pools(
		ctx, &pool_map, reload, pools, entities, &off_pool, &off_entity
	));

	JLFA(ret, pool_map);
	assert(off_pool == n_pools);
	assert(off_entity == n_entities);

	/* Add peak capacitor regen to deltas */
	for(size_t i = 0; i < n_pools; ++i) {
		if(pools[i].tau == 0) pools[i].tau = 1.0;
		pools[i].delta -= (sqrt(.25) - .25) * 2.0 * pools[i].capacity / pools[i].tau;
	}

	/* Compute the maximum durations of pool entities */
	for(size_t i = 0; i < n_entities; ++i) {
		if(entities[i].cycle_time > entities[i].location->max_affector_cycle_time) {
			entities[i].location->max_affector_cycle_time = entities[i].cycle_time;
		}
	}

	do {
		changed = 0;
		for(size_t i = 0; i < n_entities; ++i) {
			if(entities[i].target == NULL) continue;
			if(entities[i].location->max_affector_cycle_time > entities[i].target->max_affector_cycle_time) {
				entities[i].target->max_affector_cycle_time = entities[i].location->max_affector_cycle_time;
				++changed;
			}
		}
	} while(changed > 0);

	/* Now the simulation */

	while(true) {
		for(size_t i = 0; i < n_entities; ++i) {
			dogma_simple_energy_entity_t* ent = entities + i;

			ent->timer -= DOGMA_CAPACITOR_STEP;

			if(ent->reloading && ent->timer <= 0) {
				/* Module is done reloading */
				ent->remaining_cycles = ent->num_cycles_per_reload;
				ent->reloading = false;
			}

			if(!ent->reloading) {
				while(ent->timer <= 0 && ent->remaining_cycles > 0) {
					if(ent->location->current < ent->amount_used) {
						/* Not enough capacitor */
						if(ent->location->starvation_time < 0) {
							ent->location->starvation_time = elapsed;
						}
						break;
					}

					if(ent->type == DOGMA_EENT_Powerboost) {
						if((ent->location->current + ent->other_amount) > ent->location->capacity) {
							/* Not wasting a precious cap charge */
							break;
						}

						ent->location->current += ent->other_amount;
					}

					else if(ent->type == DOGMA_EENT_Transfer && ent->target != NULL) {
						ent->target->current += ent->other_amount;
						if(ent->target->current > ent->target->capacity) {
							ent->target->current = ent->target->capacity;
						}
					}

					else if(ent->type == DOGMA_EENT_Neutralization && ent->target != NULL) {
						ent->target->current -= ent->other_amount;
						if(ent->target->current < 0) {
							ent->target->current = 0;
						}
					}

					else if(ent->type == DOGMA_EENT_Leech && ent->target != NULL) {
						if(ent->location->current < ent->target->current) {
							double leeched = ent->target->current - ent->location->current;
							if(leeched > ent->other_amount) {
								leeched = ent->other_amount;
							}

							ent->location->current += leeched;
							ent->location->current -= leeched;

							if(ent->location->current > ent->location->capacity) {
								ent->location->current = ent->location->capacity;
							}
						}
					}

					--(ent->remaining_cycles);
					ent->timer += ent->cycle_time;
					ent->location->current -= ent->amount_used;
				}

				if(ent->remaining_cycles == 0) {
					/* Module starts reloading */
					ent->timer += ent->reload_time;
					ent->reloading = true;
				}
			}
		}

		n = 0;
		for(size_t i = 0; i < n_pools; ++i) {
			dogma_simple_energy_pool_t* pool = pools + i;

			if(pool->current < pool->min) {
				pool->min = pool->current;
				pool->time_since_min = 0;
			} else {
				pool->time_since_min += DOGMA_CAPACITOR_STEP;
			}

			/* Regenerate capacitor */
			k1 = runge_kutta_step(pool->current, pool->capacity, pool->tau);
			k2 = runge_kutta_step(pool->current + .5 * DOGMA_CAPACITOR_STEP * k1, pool->capacity, pool->tau);
			k3 = runge_kutta_step(pool->current + .5 * DOGMA_CAPACITOR_STEP * k2, pool->capacity, pool->tau);
			k4 = runge_kutta_step(pool->current +      DOGMA_CAPACITOR_STEP * k3, pool->capacity, pool->tau);
			pool->current += DOGMA_CAPACITOR_STEP * (k1 + k2 + k2 + k3 + k3 + k4) / 6.0;
			if(pool->current > pool->capacity) pool->current = pool->capacity;

			if(pool->starvation_time >= 0 || pool->time_since_min > 32 * pool->max_affector_cycle_time) {
				/* This pool doesn't need more computation */
				++n;
			}
		}

		do {
			changed = 0;
			for(size_t j = 0; j < n_entities; ++j) {
				if(entities[j].target == NULL) continue;
				if(entities[j].location->time_since_min == 0 && entities[j].target->time_since_min != 0) {
					entities[j].target->time_since_min = 0;
					++changed;
				}
			}
		} while(changed > 0);

		if(n == n_pools) {
			/* Simulation can be stopped, all pools are either starved
			 * or stable enough */
			break;
		}

		elapsed += DOGMA_CAPACITOR_STEP;
	}

	dogma_simple_capacitor_t* simple_pools = malloc(n_pools * sizeof(dogma_simple_capacitor_t));
	*list = simple_pools;
	for(size_t i = 0; i < n_pools; ++i) {
		dogma_simple_energy_pool_t* pool = pools + i;
		dogma_simple_capacitor_t* simple = simple_pools + i;

		simple->context = pool->context;
		simple->capacity = pool->capacity;
		simple->delta = pool->delta;

		if(pool->starvation_time >= 0) {
			simple->stable = false;
			simple->depletion_time = pool->starvation_time;
		} else if(pool->capacity > 1e-300) {
			simple->stable = true;
			simple->stable_fraction = pool->min / pool->capacity;
		} else if(pool->delta <= 0) {
			simple->stable = true;
			simple->stable_fraction = 1.0;
		} else {
			simple->stable = false;
			simple->depletion_time = 0.0;
		}
	}

	return DOGMA_OK;
}

int dogma_free_capacitor_list(dogma_simple_capacitor_t* list) {
	free(list);
	return DOGMA_OK;
}
