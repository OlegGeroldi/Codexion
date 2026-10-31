/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_pair.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olunichk <olunichk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/30 16:06:39 by olunichk          #+#    #+#             */
/*   Updated: 2026/10/30 16:07:02 by olunichk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

t_dongle	*low_dongle(t_coder *coder)
{
	int	left;
	int	right;

	left = coder->id - 1;
	right = coder->id % coder->sim->cfg.coders;
	if (left < right)
		return (&coder->sim->dongles[left]);
	return (&coder->sim->dongles[right]);
}

t_dongle	*high_dongle(t_coder *coder)
{
	int	left;
	int	right;

	left = coder->id - 1;
	right = coder->id % coder->sim->cfg.coders;
	if (left > right)
		return (&coder->sim->dongles[left]);
	return (&coder->sim->dongles[right]);
}
