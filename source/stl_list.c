/* The MIT License (MIT)
 *
 * Copyright (c) 2019 LUOYUN <sysu.zqlong@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <string.h>
#include "include/stl_list.h"

struct stl_list_node {
    void *data;
    struct stl_list_node *prev;
    struct stl_list_node *next;
};

struct stl_list {
    struct stl_list_node *end;
    void (*free_cb)(void *data);
    size_t elem_count;
};

static struct stl_list_node *stl_list_node_create(void *data)
{
    struct stl_list_node *node = malloc(sizeof(struct stl_list_node));
    if (node != NULL)
        node->data = data;
    return node;
}

static void stl_list_node_destroy(struct stl_list_node *node, void (*free_cb)(void *data))
{
    if (free_cb != NULL && node->data != NULL)
        free_cb(node->data);
    free(node);
}

stl_list_t stl_list_create(void (*free_cb)(void *data))
{
    struct stl_list *list;
    struct stl_list_node *node;

    list = malloc(sizeof(struct stl_list));
    if (list == NULL) {
        fprintf(stderr, "Out of memory\n");
        return NULL;
    }

    node = stl_list_node_create(NULL);
    if (node == NULL) {
        fprintf(stderr, "Out of memory\n");
        free(list);
        return NULL;
    }
    node->next = node;
    node->prev = node;

    list->end = node;
    list->free_cb = free_cb;
    list->elem_count = 0;

    return list;
}

void stl_list_destroy(const stl_list_t list)
{
    stl_list_clear(list);
    stl_list_node_destroy(list->end, NULL);
    free(list);
}

size_t stl_list_size(const stl_list_t list)
{
    return list->elem_count;
}

bool stl_list_empty(const stl_list_t list)
{
    return list->end->next == list->end;
}

iterator stl_list_begin(const stl_list_t list)
{
    return list->end->next;
}

iterator stl_list_end(const stl_list_t list)
{
    return list->end;
}

iterator stl_list_rbegin(const stl_list_t list)
{
    return list->end->prev;
}

iterator stl_list_rend(const stl_list_t list)
{
    return list->end;
}

iterator stl_list_next(iterator iter)
{
    return ((struct stl_list_node *)iter)->next;
}

iterator stl_list_prev(iterator iter)
{
    return ((struct stl_list_node *)iter)->prev;
}

int stl_list_pushfront(const stl_list_t list, void *data)
{
    iterator iter = stl_list_insert(list, stl_list_begin(list), data);
    if (iter == NULL)
        return -1;
    return 0;
}

void stl_list_popfront(const stl_list_t list)
{
    stl_list_erase(list, stl_list_begin(list));
}

int stl_list_pushback(const stl_list_t list, void *data)
{
    iterator iter = stl_list_insert(list, stl_list_end(list), data);
    if (iter == NULL)
        return -1;
    return 0;
}

void stl_list_popback(const stl_list_t list)
{
    stl_list_erase(list, stl_list_rbegin(list));
}

void *stl_list_front(const stl_list_t list)
{
    return ((struct stl_list_node *)stl_list_begin(list))->data;
}

void *stl_list_back(const stl_list_t list)
{
    return ((struct stl_list_node *)stl_list_rbegin(list))->data;
}

void *stl_list_at(iterator iter)
{
    return ((struct stl_list_node *)iter)->data;
}

iterator stl_list_insert(const stl_list_t list, iterator iter, void *data)
{
    struct stl_list_node *position = (struct stl_list_node *)iter;
    struct stl_list_node *new_node = stl_list_node_create(data);

    if (new_node == NULL) {
        fprintf(stderr, "Out of memory\n");
        return NULL;
    }

    new_node->next = position;
    new_node->prev = position->prev;
    position->prev->next = new_node;
    position->prev = new_node;

    list->elem_count++;
    return (iterator)new_node;
}

iterator stl_list_erase(const stl_list_t list, iterator iter)
{
    struct stl_list_node *position = (struct stl_list_node *)iter;
    struct stl_list_node *next_node = position->next;
    struct stl_list_node *prev_node = position->prev;

    if (iter == stl_list_end(list)) {
        fprintf(stderr, "Out of range\n");
        return (iterator)next_node;
    }

    prev_node->next = next_node;
    next_node->prev = prev_node;

    stl_list_node_destroy(position, list->free_cb);
    list->elem_count--;
    return (iterator)next_node;
}

void stl_list_clear(const stl_list_t list)
{
    struct stl_list_node *current = (struct stl_list_node *)stl_list_begin(list);

    while (current != stl_list_end(list)) {
        struct stl_list_node *temp = current;
        current = current->next;
        stl_list_node_destroy(temp, list->free_cb);
    }

    list->end->next = list->end;
    list->end->prev = list->end;
    list->elem_count = 0;
}

iterator stl_list_find(const stl_list_t list, bool (*match_cb)(void *data))
{
    struct stl_list_node *current = (struct stl_list_node *)stl_list_begin(list);

    if (match_cb == NULL)
        return NULL;

    while (current != stl_list_end(list)) {
        if (match_cb(current->data))
            return (iterator)current;
        current = current->next;
    }
    return NULL;
}

void stl_list_remove_if(const stl_list_t list, bool (*match_cb)(void *data))
{
    struct stl_list_node *current = (struct stl_list_node *)stl_list_begin(list);

    if (match_cb == NULL)
        return;

    while (current != stl_list_end(list)) {
        struct stl_list_node *temp = current;
        current = current->next;
        if (match_cb(temp->data))
            stl_list_erase(list, temp);
    }
}
