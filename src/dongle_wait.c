/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_wait.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olunichk <olunichk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/30 16:07:46 by olunichk          #+#    #+#             */
/*   Updated: 2026/10/30 17:16:44 by olunichk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

t_request	request_make(t_sim *sim, t_coder *coder)
{
	t_request	req;

	req.coder = coder->id;
	req.seq = sim_next_seq(sim);
	pthread_mutex_lock(&coder->guard);
	req.deadline = coder->last_start + sim->cfg.burnout;
	pthread_mutex_unlock(&coder->guard);
	return (req);
}

static bool	can_grant(t_dongle *dongle, t_request req, long now)
{
	t_request	top;

	if (dongle->held || now < dongle->free_at)
		return (false);
	if (!heap_peek(&dongle->waiters, &top))
		return (false);
	return (top.seq == req.seq);
}

static bool	wait_for_grant(t_sim *sim, t_dongle *dongle, t_request req)
{
	struct timespec	ts;

	while (!sim_stopped(sim))
	{
		if (can_grant(dongle, req, now_ms()))
		{
			heap_pop(&dongle->waiters, NULL);
			dongle->held = true;
			return (true);
		}
		slice_deadline(&ts);
		pthread_cond_timedwait(&dongle->cond, &dongle->lock, &ts);
	}
	return (false);
}

bool	dongle_claim(t_sim *sim, t_dongle *dongle, t_request req)
{
	bool	granted;

	pthread_mutex_lock(&dongle->lock);
	granted = wait_for_grant(sim, dongle, req);
	pthread_mutex_unlock(&dongle->lock);
	return (granted);
}
