/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_state.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olunichk <olunichk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/30 16:07:15 by olunichk          #+#    #+#             */
/*   Updated: 2026/10/30 16:07:18 by olunichk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	coder_mark_start(t_coder *coder)
{
	pthread_mutex_lock(&coder->guard);
	coder->last_start = now_ms();
	pthread_mutex_unlock(&coder->guard);
}

void	coder_mark_done(t_coder *coder)
{
	pthread_mutex_lock(&coder->guard);
	coder->compiles++;
	pthread_mutex_unlock(&coder->guard);
}

void	coder_snapshot(t_coder *coder, long *last_start, int *compiles)
{
	pthread_mutex_lock(&coder->guard);
	*last_start = coder->last_start;
	*compiles = coder->compiles;
	pthread_mutex_unlock(&coder->guard);
}
