/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim_run.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olunichk <olunichk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/30 16:12:11 by olunichk          #+#    #+#             */
/*   Updated: 2026/10/30 16:27:18 by olunichk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	rest_clocks(t_sim *sim)
{
	t_coder	*coder;
	t_coder	*end;

	sim->start_ms = now_ms();
	coder = sim->coders;
	end = sim->coders + sim->cfg.coders;
	while (coder < end)
	{
		coder->last_start = sim->start_ms;
		coder++;
	}
}

static bool	spawn_coders(t_sim *sim)
{
	t_coder	*coder;
	t_coder	*end;

	coder = sim->coders;
	end = sim->coders + sim->cfg.coders;
	while (coder < end)
	{
		if (pthread_create(&coder->thread, NULL, coder_routine, coder) != 0)
			return (false);
		sim->spawned++;
		coder++;
	}
	return (true);
}

static void	join_coders(t_sim *sim)
{
	t_coder	*coder;
	t_coder	*end;

	coder = sim->coders;
	end = sim->coders + sim->spawned;
	while (coder < end)
	{
		pthread_join(coder->thread, NULL);
		coder++;
	}
}

bool	sim_run(t_sim *sim)
{
	rest_clocks(sim);
	if (!spawn_coders(sim))
	{
		sim_stop(sim);
		join_coders(sim);
		return (false);
	}
	if (pthread_create(&sim->monitor, NULL, monitor_routine, sim) != 0)
	{
		sim_stop(sim);
		join_coders(sim);
		return (false);
	}
	pthread_join(sim->monitor, NULL);
	join_coders(sim);
	return (true);
}
