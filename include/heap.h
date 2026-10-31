/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olunichk <olunichk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/30 16:03:37 by olunichk          #+#    #+#             */
/*   Updated: 2026/10/30 16:03:40 by olunichk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEAP_H
# define HEAP_H

# include <stdbool.h>
# include <stddef.h>
# include <stdlib.h>

typedef enum e_policy
{
	POLICY_FIFO,
	POLICY_EDF
}	t_policy;

typedef struct s_request
{
	int		coder;
	long	seq;
	long	deadline;	
}	t_request;

typedef struct s_heap
{
	t_request	*items;
	size_t		size;
	size_t		cap;
	t_policy	policy;
}	t_heap;

/* heap.c */
bool	heap_init(t_heap *heap, size_t cap, t_policy policy);
void	heap_free(t_heap *heap);
bool	heap_push(t_heap *heap, t_request req);
bool	heap_pop(t_heap *heap, t_request *out);
bool	heap_peek(const t_heap *heap, t_request *out);

/* heap_ops.c */
bool	heap_less(const t_heap *heap, size_t a, size_t b);
void	heap_swap(t_heap *heap, size_t a, size_t b);
void	heap_sift_up(t_heap *heap, size_t index);
void	heap_sift_down(t_heap *heap, size_t index);
bool	heap_remove(t_heap *heap, long seq);

#endif
