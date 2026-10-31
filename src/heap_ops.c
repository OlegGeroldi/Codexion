/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_ops.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olunichk <olunichk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/30 16:08:15 by olunichk          #+#    #+#             */
/*   Updated: 2026/10/30 16:17:45 by olunichk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "heap.h"

bool	heap_less(const t_heap *heap, size_t a, size_t b)
{
	if (heap->policy == POLICY_EDF)
	{
		if (heap->items[a].deadline != heap->items[b].deadline)
			return (heap->items[a].deadline < heap->items[b].deadline);
	}
	return (heap->items[a].seq < heap->items[b].seq);
}

void	heap_swap(t_heap *heap, size_t a, size_t b)
{
	t_request	tmp;

	tmp = heap->items[a];
	heap->items[a] = heap->items[b];
	heap->items[b] = tmp;
}

void	heap_sift_up(t_heap *heap, size_t index)
{
	size_t	parent;

	while (index > 0)
	{
		parent = (index - 1) / 2;
		if (!heap_less(heap, index, parent))
			return ;
		heap_swap(heap, index, parent);
		index = parent;
	}
}

void	heap_sift_down(t_heap *heap, size_t index)
{
	size_t	child;
	size_t	best;

	best = index;
	while (true)
	{
		child = 2 * index + 1;
		if (child < heap->size && heap_less(heap, child, best))
			best = child;
		if (child + 1 < heap->size && heap_less(heap, child + 1, best))
			best = child + 1;
		if (best == index)
			return ;
		heap_swap(heap, index, best);
		index = best;
	}
}

bool	heap_remove(t_heap *heap, long seq)
{
	size_t		i;
	t_request	*item;
	t_request	*end;

	item = heap->items;
	end = heap->items + heap->size;
	while (item < end)
	{
		if (item->seq == seq)
		{
			i = item - heap->items;
			heap->size--;
			if (i < heap->size)
			{
				*item = heap->items[heap->size];
				heap_sift_up(heap, i);
				heap_sift_down(heap, i);
			}
			return (true);
		}
		item++;
	}
	return (false);
}
