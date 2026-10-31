/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olunichk <olunichk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/30 16:07:33 by olunichk          #+#    #+#             */
/*   Updated: 2026/10/30 16:24:46 by olunichk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	coder_stagger(t_coder *coder)
{
	if (coder->id % 2 == 0)
		sim_sleep(coder->sim, coder->sim->cfg.compile / 2 + 1);
}

static bool	coder_compile(t_coder *coder)
{
	if (!coder_take_both(coder))
		return (false);
	coder_mark_start(coder);
	log_event(coder->sim, coder->id, STATE_COMPILING);
	sim_sleep(coder->sim, coder->sim->cfg.compile);
	coder_mark_done(coder);
	coder_drop_both(coder);
	return (!sim_stopped(coder->sim));
}

static bool	coder_rest(t_coder *coder)
{
	log_event(coder->sim, coder->id, STATE_DEBUGGING);
	sim_sleep(coder->sim, coder->sim->cfg.debug);
	if (sim_stopped(coder->sim))
		return (false);
	log_event(coder->sim, coder->id, STATE_REFACTORING);
	sim_sleep(coder->sim, coder->sim->cfg.refactor);
	return (!sim_stopped(coder->sim));
}

void	*coder_routine(void *raw)
{
	t_coder	*coder;

	coder = (t_coder *)raw;
	coder_stagger(coder);
	while (!sim_stopped(coder->sim))
	{
		if (!coder_compile(coder))
			break ;
		if (!coder_rest(coder))
			break ;
	}
	return (NULL);
}
