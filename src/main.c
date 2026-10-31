/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olunichk <olunichk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/30 16:08:53 by olunichk          #+#    #+#             */
/*   Updated: 2026/10/30 16:14:06 by olunichk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int ac, char **av)
{
	t_sim	sim;

	if (!args_parse(&sim.cfg, ac, av))
		return (usage_error());
	if (!sim_init(&sim))
	{
		fprintf(stderr, "codexion could not set up the simulation\n");
		return (1);
	}
	if (!sim_run(&sim))
	{
		fprintf(stderr, "codexion: could not start thread\n");
		sim_destroy(&sim);
		return (1);
	}
	sim_destroy(&sim);
	return (0);
}
