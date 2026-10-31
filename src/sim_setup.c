/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim_setup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olunichk <olunichk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/30 16:12:25 by olunichk          #+#    #+#             */
/*   Updated: 2026/10/30 17:18:04 by olunichk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static bool	init_locks(t_sim *sim)
{
	if (pthread_mutex_init(&sim->stop_lock, NULL) != 0)
		return (false);
	if (pthread_mutex_init(&sim->seq_lock, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->stop_lock);
		return (false);
	}
	if (pthread_mutex_init(&sim->log_lock, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->seq_lock);
		pthread_mutex_destroy(&sim->stop_lock);
		return (false);
	}
	return (true);
}

static bool	init_dongles(t_sim *sim)
{
	t_dongle	*d;
	t_dongle	*end;

	sim->dongles = malloc(sizeof(t_dongle) * (size_t)sim->cfg.coders);
	if (!sim->dongles)
		return (false);
	d = sim->dongles;
	end = sim->dongles + sim->cfg.coders;
	while (d < end)
	{
		if (!dongle_init(d, d - sim->dongles, &sim->cfg))
		{
			while (d > sim->dongles)
				dongle_destroy(--d);
			free(sim->dongles);
			sim->dongles = NULL;
			return (false);
		}
		d++;
	}
	return (true);
}

static bool	init_coders(t_sim *sim)
{
	t_coder	*c;
	t_coder	*end;

	sim->coders = malloc(sizeof(t_coder) * (size_t)sim->cfg.coders);
	if (!sim->coders)
		return (false);
	c = sim->coders;
	end = sim->coders + sim->cfg.coders;
	while (c < end)
	{
		c->id = (c - sim->coders) + 1;
		c->last_start = 0;
		c->compiles = 0;
		c->sim = sim;
		if (pthread_mutex_init(&c->guard, NULL) != 0)
		{
			while (c > sim->coders)
				pthread_mutex_destroy(&(--c)->guard);
			free(sim->coders);
			return (!(sim->coders = NULL));
		}
		c++;
	}
	return (true);
}

bool	sim_init(t_sim *sim)
{
	sim->coders = NULL;
	sim->dongles = NULL;
	sim->stop = false;
	sim->seq = 0;
	sim->spawned = 0;
	sim->start_ms = now_ms();
	if (!init_locks(sim))
		return (false);
	if (!init_dongles(sim))
	{
		destroy_locks(sim);
		return (false);
	}
	if (!init_coders(sim))
	{
		destroy_dongles(sim);
		destroy_locks(sim);
		return (false);
	}
	return (true);
}
