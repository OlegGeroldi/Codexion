/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olunichk <olunichk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/30 16:12:38 by olunichk          #+#    #+#             */
/*   Updated: 2026/10/30 16:12:40 by olunichk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

bool	sim_stopped(t_sim *sim)
{
	bool	flag;

	pthread_mutex_lock(&sim->stop_lock);
	flag = sim->stop;
	pthread_mutex_unlock(&sim->stop_lock);
	return (flag);
}

void	sim_stop(t_sim *sim)
{
	pthread_mutex_lock(&sim->stop_lock);
	sim->stop = true;
	pthread_mutex_unlock(&sim->stop_lock);
}

long	sim_next_seq(t_sim *sim)
{
	long	value;

	pthread_mutex_lock(&sim->seq_lock);
	value = sim->seq;
	sim->seq++;
	pthread_mutex_unlock(&sim->seq_lock);
	return (value);
}

long	sim_elapsed(t_sim *sim)
{
	return (now_ms() - sim->start_ms);
}
