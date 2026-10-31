/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olunichk <olunichk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/30 16:08:03 by olunichk          #+#    #+#             */
/*   Updated: 2026/10/31 20:04:19 by olunichk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

bool	dongle_init(t_dongle *dongle, int id, const t_config *cfg)
{
	dongle->id = id;
	dongle->held = false;
	dongle->free_at = 0;
	if (!heap_init(&dongle->waiters, (size_t)cfg->coders, cfg->policy))
		return (false);
	if (pthread_mutex_init(&dongle->lock, NULL) != 0)
	{
		heap_free(&dongle->waiters);
		return (false);
	}
	if (pthread_cond_init(&dongle->cond, NULL) != 0)
	{
		pthread_mutex_destroy(&dongle->lock);
		heap_free(&dongle->waiters);
		return (false);
	}
	return (true);
}

void	dongle_destroy(t_dongle *dongle)
{
	pthread_cond_destroy(&dongle->cond);
	pthread_mutex_destroy(&dongle->lock);
	heap_free(&dongle->waiters);
}

bool	dongle_enqueue(t_dongle *dongle, t_request req)
{
	bool	pushed;

	pthread_mutex_lock(&dongle->lock);
	pushed = heap_push(&dongle->waiters, req);
	pthread_mutex_unlock(&dongle->lock);
	return (pushed);
}

void	dongle_withdraw(t_dongle *dongle, t_request req)
{
	pthread_mutex_lock(&dongle->lock);
	heap_remove(&dongle->waiters, req.seq);
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->lock);
}

void	dongle_drop(t_sim *sim, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->lock);
	dongle->held = false;
	dongle->free_at = now_ms() + sim->cfg.cooldown;
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->lock);
}
