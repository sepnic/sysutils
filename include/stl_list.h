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

#ifndef __STL_LIST_H__
#define __STL_LIST_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct stl_list *stl_list_t;
typedef void *iterator;

stl_list_t stl_list_create(void (*free_cb)(void *data));

void stl_list_destroy(const stl_list_t list);

// Returns the number of elements in the list
size_t stl_list_size(const stl_list_t list);

// Returns whether the list is empty
bool stl_list_empty(const stl_list_t list);

// Returns a iterator to the first element in the list
iterator stl_list_begin(const stl_list_t list);

// Returns a iterator to the past-the-end element in the list, that would follow
// the last element. It does not point to any element, and thus shall not be
// dereferenced
iterator stl_list_end(const stl_list_t list);

// Returns a reverse iterator pointing to the last element
iterator stl_list_rbegin(const stl_list_t list);

// Returns a reverse iterator pointing to the theoretical element preceding the
// first element
iterator stl_list_rend(const stl_list_t list);

// Returns a iterator that followed the specified position
iterator stl_list_next(iterator iter);

// Returns a iterator that before the specified position
iterator stl_list_prev(iterator iter);

// Adds a new element at the beginning of the list, before current first element
int stl_list_pushfront(const stl_list_t list, void *data);

// Removes the first elemen in the list
void stl_list_popfront(const stl_list_t list);

// Adds a new element at the end of the list, after current last element
int stl_list_pushback(const stl_list_t list, void *data);

// Removes the last elemen in the list
void stl_list_popback(const stl_list_t list);

// Returns a pointer to the first element in the list
void *stl_list_front(const stl_list_t list);

// Returns a pointer to the last element in the list
void *stl_list_back(const stl_list_t list);

// Returns a pointer to the element at specified position
void *stl_list_at(iterator iter);

// Extended by inserting new element before the element at the position
// Parameters:
//   @iter: positon in the list where the new element is inserted
//   @data: pointer to the inserted element
// Return:
//   iterator that points to the newly inserted element
iterator stl_list_insert(const stl_list_t list, iterator iter, void *data);

// Removes a single element from the list
// Parameters:
//   @iter: iterator to the element to be removed from the list
// Return:
//   iterator to the new location of the element that followd the erased
//   element
iterator stl_list_erase(const stl_list_t list, iterator iter);

// Removes all elements from the list
void stl_list_clear(const stl_list_t list);

// Find the element that first meet the condition
iterator stl_list_find(const stl_list_t list, bool (*match_cb)(void *data));

// Removes the elements that meet the condition
void stl_list_remove_if(const stl_list_t list, bool (*match_cb)(void *data));

#ifdef __cplusplus
}
#endif

#endif // __STL_LIST_H__
