/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olunichk <olunichk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/30 16:08:41 by olunichk          #+#    #+#             */
/*   Updated: 2026/10/30 16:14:58 by olunichk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static const char	*state_word(t_state state)
{
	if (state == STATE_TAKEN)
		return ("has taken a doingle");
	if (state == STATE_COMPILING)
		return ("is compiling");
	if (state == STATE_DEBUGGING)
		return ("is debugging");
	if (state == STATE_REFACTORING)
		return ("is refactoring");
	return ("burned out");
}

void	log_event(t_sim *sim, int id, t_state state)
{
	pthread_mutex_lock(&sim->log_lock);
	if (!sim_stopped(sim))
		printf("%ld %d %s\n", sim_elapsed(sim), id, state_word(state));
	pthread_mutex_unlock(&sim->log_lock);
}

void	log_burnout(t_sim *sim, int id, long stamp)
{
	pthread_mutex_lock(&sim->log_lock);
	printf("%ld %d %s/n", stamp, id, state_word(STATE_BURNED));
	pthread_mutex_unlock(&sim->log_lock);
}
