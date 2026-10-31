/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_dongles.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olunichk <olunichk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/30 16:06:21 by olunichk          #+#    #+#             */
/*   Updated: 2026/10/30 16:06:23 by olunichk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static bool	take_lonely(t_coder *coder, t_request req)
{
	t_dongle	*only;

	only = &coder->sim->dongles[0];
	if (!dongle_enqueue(only, req))
		return (false);
	if (!dongle_claim(coder->sim, only, req))
	{
		dongle_withdraw(only, req);
		return (false);
	}
	log_event(coder->sim, coder->id, STATE_TAKEN);
	while (!sim_stopped(coder->sim))
		usleep(MONITOR_TICK_US);
	dongle_drop(coder->sim, only);
	return (false);
}

static bool	abandon(t_coder *coder, t_request req, t_dongle *held)
{
	if (held != NULL)
		dongle_drop(coder->sim, held);
	dongle_withdraw(low_dongle(coder), req);
	dongle_withdraw(high_dongle(coder), req);
	return (false);
}

bool	coder_take_both(t_coder *coder)
{
	t_request	req;

	req = request_make(coder->sim, coder);
	if (coder->sim->cfg.coders == 1)
		return (take_lonely(coder, req));
	if (!dongle_enqueue(low_dongle(coder), req))
		return (false);
	if (!dongle_enqueue(high_dongle(coder), req))
		return (abandon(coder, req, NULL));
	if (!dongle_claim(coder->sim, low_dongle(coder), req))
		return (abandon(coder, req, NULL));
	log_event(coder->sim, coder->id, STATE_TAKEN);
	if (!dongle_claim(coder->sim, high_dongle(coder), req))
		return (abandon(coder, req, low_dongle(coder)));
	log_event(coder->sim, coder->id, STATE_TAKEN);
	return (true);
}

void	coder_drop_both(t_coder *coder)
{
	if (coder->sim->cfg.coders == 1)
		return ;
	dongle_drop(coder->sim, high_dongle(coder));
	dongle_drop(coder->sim, low_dongle(coder));
}
