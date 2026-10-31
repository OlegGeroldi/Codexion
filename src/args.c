/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   args.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olunichk <olunichk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/30 16:06:08 by olunichk          #+#    #+#             */
/*   Updated: 2026/10/30 16:06:11 by olunichk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static bool	digits_only(const char *raw)
{
	if (raw == NULL || *raw == '\0')
		return (false);
	while (*raw != '\0')
	{
		if (*raw < '0' || *raw > '9')
			return (false);
		raw++;
	}
	return (true);
}

long	str_to_long(const char *raw, bool *ok)
{
	long	value;

	value = 0;
	if (*ok == false)
		return (0);
	if (!digits_only(raw))
	{
		*ok = false;
		return (0);
	}
	while (*raw != '\0')
	{
		value = value * 10 + (*raw - '0');
		if (value > ARG_MAX_VALUE)
		{
			*ok = false;
			return (0);
		}
		raw++;
	}
	return (value);
}

static bool	fill_number(t_config *cfg, char **argv)
{
	bool	ok;

	ok = true;
	cfg->coders = (int)str_to_long(argv[1], &ok);
	cfg->burnout = str_to_long(argv[2], &ok);
	cfg->compile = str_to_long(argv[3], &ok);
	cfg->debug = str_to_long(argv[4], &ok);
	cfg->refactor = str_to_long(argv[5], &ok);
	cfg->required = (int)str_to_long(argv[6], &ok);
	cfg->cooldown = str_to_long(argv[7], &ok);
	return (ok);
}

static bool	parse_policy(t_config *cfg, const char *raw)
{
	if (strcmp(raw, "fifo") == 0)
	{
		cfg->policy = POLICY_FIFO;
		return (true);
	}
	if (strcmp(raw, "edf") == 0)
	{
		cfg->policy = POLICY_EDF;
		return (true);
	}
	return (false);
}

bool	args_parse(t_config *cfg, int argc, char **argv)
{
	if (argc != ARG_COUNT)
		return (arg_error("exactly 8 arguments are required"));
	memset(cfg, 0, sizeof(t_config));
	if (!fill_number(cfg, argv))
		return (arg_error("numbers must be integer from 0 to 2147483647"));
	if (!parse_policy(cfg, argv[8]))
		return (arg_error("sheduler must be 'fifo' or 'edf'"));
	return (cfg_valid(cfg));
}
