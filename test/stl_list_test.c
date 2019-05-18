#include <stdio.h>
#include "include/stl_list.h"

#define array_size(arr) sizeof(arr)/sizeof((arr)[0])

static void dump(stl_list_t list)
{
    iterator iter;
    int *value;

    printf("/***********************************\n");
    printf(" element count: %d\n", (int)stl_list_size(list));
    for (iter = stl_list_begin(list); iter != stl_list_end(list); iter = stl_list_next(iter)) {
        value = stl_list_at(iter);
        printf(" %d ", *value);
    }
    printf("\n **********************************/\n");
}

static bool match_cb(void *data)
{
    return *((int *)data) % 3 == 0;
}

int main()
{
    int arr[10], i, val = 99;
    iterator it;
    stl_list_t list = stl_list_create(NULL);

    printf("\npush back 1...10 into the list\n");
    for (i = 0; i < array_size(arr); i++) {
        arr[i] = i + 1;
        stl_list_pushback(list, &arr[i]);
    }
    dump(list);

    printf("\npop back all elements from the list\n");
    for (i = 0; i < array_size(arr); i++) {
        stl_list_popback(list);
    }
    dump(list);

    printf("\npush front 1...10 into the list\n");
    for (i = 0; i < array_size(arr); i++) {
        arr[i] = i + 1;
        stl_list_pushfront(list, &arr[i]);
    }
    dump(list);

    it = stl_list_find(list, match_cb);
    if (it) {
        printf("\nfind the element[%d] that divisible by three at the position[%p]\n",
               *((int *)stl_list_at(it)), it);

        printf("\ninsert 99 before the element[%d]\n", *((int *)stl_list_at(it)));
        stl_list_insert(list, it, &val);
    }
    dump(list);

    printf("\nremove all the elements that divisible by three\n");
    stl_list_remove_if(list, match_cb);
    dump(list);

    printf("\nclear all the elements from the list\n");
    stl_list_clear(list);
    dump(list);

    stl_list_destroy(list);
    return 0;
}
