/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim_free.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olunichk <olunichk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/30 16:11:58 by olunichk          #+#    #+#             */
/*   Updated: 2026/10/30 17:18:50 by olunichk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	destroy_locks(t_sim *sim)
{
	pthread_mutex_destroy(&sim->log_lock);
	pthread_mutex_destroy(&sim->seq_lock);
	pthread_mutex_destroy(&sim->stop_lock);
}

void	destroy_dongles(t_sim *sim)
{
	t_dongle	*dongle;
	t_dongle	*end;

	if (sim->dongles == NULL)
		return ;
	dongle = sim->dongles;
	end = sim->dongles + sim->cfg.coders;
	while (dongle < end)
	{
		dongle_destroy(dongle);
		dongle++;
	}
	free(sim->dongles);
	sim->dongles = NULL;
}

void	destroy_coders(t_sim *sim)
{
	t_coder	*coder;
	t_coder	*end;

	if (sim->coders == NULL)
		return ;
	coder = sim->coders;
	end = sim->coders + sim->cfg.coders;
	while (coder < end)
	{
		pthread_mutex_destroy(&coder->guard);
		coder++;
	}
	free(sim->coders);
	sim->coders = NULL;
}

void	sim_destroy(t_sim *sim)
{
	destroy_coders(sim);
	destroy_dongles(sim);
	destroy_locks(sim);
}
