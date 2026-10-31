/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olunichk <olunichk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/30 16:11:48 by olunichk          #+#    #+#             */
/*   Updated: 2026/10/30 16:26:18 by olunichk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static bool	check_burnout(t_sim *sim)
{
	t_coder	*coder;
	t_coder	*end;
	long	last;
	int		done;
	long	now;

	coder = sim->coders;
	end = sim->coders + sim->cfg.coders;
	while (coder < end)
	{
		coder_snapshot(coder, &last, &done);
		now = now_ms();
		if (now - last > sim->cfg.burnout)
		{
			sim_stop(sim);
			log_burnout(sim, coder->id, now - sim->start_ms);
			return (true);
		}
		coder++;
	}
	return (false);
}

static bool	check_done(t_sim *sim)
{
	t_coder	*coder;
	t_coder	*end;
	long	last;
	int		done;

	coder = sim->coders;
	end = sim->coders + sim->cfg.coders;
	while (coder < end)
	{
		coder_snapshot(coder, &last, &done);
		if (done < sim->cfg.required)
			return (false);
		coder++;
	}
	sim_stop(sim);
	return (true);
}

void	*monitor_routine(void *raw)
{
	t_sim	*sim;

	sim = (t_sim *)raw;
	while (!sim_stopped(sim))
	{
		if (check_burnout(sim))
			return (NULL);
		if (check_done(sim))
			return (NULL);
		usleep(MONITOR_TICK_US);
	}
	return (NULL);
}
