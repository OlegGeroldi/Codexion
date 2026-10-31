/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olunichk <olunichk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/30 16:04:51 by olunichk          #+#    #+#             */
/*   Updated: 2026/10/30 17:15:41 by olunichk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdbool.h>
# include <stddef.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <time.h>
# include <unistd.h>

# include "heap.h"

# define MAX_CODERS 1000
# define MONITOR_TICK_US 200
# define WAIT_SLICE_NS 300000L
# define ARG_COUNT 9
# define ARG_MAX_VALUE 21474883647L

typedef enum e_state
{
	STATE_TAKEN,
	STATE_COMPILING,
	STATE_DEBUGGING,
	STATE_REFACTORING,
	STATE_BURNED
}	t_state;

typedef struct s_config
{
	int			coders;
	long		burnout;
	long		compile;
	long		debug;
	long		refactor;
	int			required;
	long		cooldown;
	t_policy	policy;
}	t_config;

typedef struct s_dongle
{
	int				id;
	bool			held;
	long			free_at;
	t_heap			waiters;
	pthread_mutex_t	lock;
	pthread_cond_t	cond;
}	t_dongle;

typedef struct s_sim	t_sim;

typedef struct s_coder
{
	int				id;
	long			last_start;
	int				compiles;
	pthread_t		thread;
	pthread_mutex_t	guard;
	t_sim			*sim;
}	t_coder;

struct	s_sim
{
	t_config		cfg;
	t_coder			*coders;
	t_dongle		*dongles;
	long			start_ms;
	bool			stop;
	long			seq;
	int				spawned;
	pthread_mutex_t	stop_lock;
	pthread_mutex_t	seq_lock;
	pthread_mutex_t	log_lock;
	pthread_t		monitor;
};

/* arg.c */
long		str_to_long(const char *raw, bool *ok);
bool		args_parse(t_config *cfg, int argc, char **argv);

/* args_check.c */
bool		cfg_valid(const t_config *cfg);
int			usage_error(void);
bool		arg_error(const char *why);

/* time_utils.c */
long		now_ms(void);
void		sim_sleep(t_sim *sim, long ms);
void		slice_deadline(struct timespec *ts);

/* log.c */
void		log_event(t_sim *sim, int id, t_state state);
void		log_burnout(t_sim *sim, int id, long stamp);

/* dongle.c */
bool		dongle_init(t_dongle *dongle, int id, const t_config *cfg);
void		dongle_destroy(t_dongle *dongle);
bool		dongle_enqueue(t_dongle *dongle, t_request req);
void		dongle_withdraw(t_dongle *dongle, t_request req);
void		dongle_drop(t_sim *sim, t_dongle *dongle);

/* dongle_wait.c */
t_request	request_make(t_sim *sim, t_coder *coder);
bool		dongle_claim(t_sim *sim, t_dongle *dongle, t_request req);

/* coder.c */
void		*coder_routine(void *raw);

/* coder_state.c */
void		coder_mark_start(t_coder *coder);
void		coder_mark_done(t_coder *coder);
void		coder_snapshot(t_coder *coder, long *last_start, int *compiles);

/* coder_pair.c */
t_dongle	*low_dongle(t_coder *coder);
t_dongle	*high_dongle(t_coder *coder);

/* coder_dongles */
bool		coder_take_both(t_coder *coder);
void		coder_drop_both(t_coder *coder);

/* monitor.c */
void		*monitor_routine(void *raw);

/* sim.c */
bool		sim_stopped(t_sim *sim);
void		sim_stop(t_sim *sim);
long		sim_next_seq(t_sim *sim);
long		sim_elapsed(t_sim *sim);

/* sim_run */
bool		sim_run(t_sim *sim);

/* sim_setup.c */
bool		sim_init(t_sim *sim);

/* sim_free.c */
void		destroy_locks(t_sim *sim);
void		destroy_dongles(t_sim *sim);
void		destroy_coders(t_sim *sim);
void		sim_destroy(t_sim *sim);

#endif
