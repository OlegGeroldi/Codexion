/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olunichk <olunichk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/30 16:08:26 by olunichk          #+#    #+#             */
/*   Updated: 2026/10/30 16:18:14 by olunichk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "heap.h"
#include <stdlib.h>

bool	heap_init(t_heap *heap, size_t cap, t_policy policy)
{
	heap->items = malloc(sizeof(t_request) * cap);
	if (heap->items == NULL)
		return (false);
	heap->size = 0;
	heap->cap = cap;
	heap->policy = policy;
	return (true);
}

void	heap_free(t_heap *heap)
{
	free(heap->items);
	heap->items = NULL;
	heap->size = 0;
	heap->cap = 0;
}

bool	heap_push(t_heap *heap, t_request req)
{
	if (heap->size == heap->cap)
		return (false);
	heap->items[heap->size] = req;
	heap->size++;
	heap_sift_up(heap, heap->size - 1);
	return (true);
}

bool	heap_pop(t_heap *heap, t_request *out)
{
	if (heap->size == 0)
		return (false);
	if (out != NULL)
		*out = heap->items[0];
	heap->size--;
	if (heap->size > 0)
	{
		heap->items[0] = heap->items[heap->size];
		heap_sift_down(heap, 0);
	}
	return (true);
}

bool	heap_peek(const t_heap *heap, t_request *out)
{
	if (heap->size == 0)
		return (false);
	if (out != NULL)
		*out = heap->items[0];
	return (true);
}
