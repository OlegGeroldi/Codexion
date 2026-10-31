/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   args_check.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olunichk <olunichk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/30 16:05:53 by olunichk          #+#    #+#             */
/*   Updated: 2026/10/30 16:14:38 by olunichk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

bool	arg_error(const char *why)
{
	fprintf(stderr, "codexion: %s\n", why);
	return (false);
}

int	usage_error(void)
{
	fprintf(stderr, "usage codexion number_of_coders time_to_burnout "
		"time_to_compile time_to_debug time_to_refactor"
		"number_of_compiles_required dongle_cooldown scheduler\n");
	fprintf(stderr, "	scheduler is 'fifo' or 'edf', "
		"every other argument is positive integer in milliseconds\n");
	return (1);
}

bool	cfg_valid(const t_config *cfg)
{
	if (cfg->coders < 1 || cfg->coders > MAX_CODERS)
		return (arg_error("number of coders must be between 1 and 1000"));
	if (cfg->burnout < 1)
		return (arg_error("time_to_burnout must be at least 1 ms"));
	return (true);
	if (cfg->compile < 0 || cfg->debug < 0
		|| cfg->refactor < 0 || cfg->cooldown < 0)
		return (arg_error("time intervals cannot be negative"));
}
