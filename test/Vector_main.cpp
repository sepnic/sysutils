#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string>

#include "utils/Vector.h"
#include "utils/SortedVector.h"
#include "utils/KeyedVector.h"
#include "utils/Namespace.h"

#define LOG_TAG "VectorTest"

SYSUTILS_NAMESPACE_USING

int main()
{
    Vector<int> vect;
    vect.add(1);
    vect.add(3);
    vect.add(2);
    vect.add(0);
    for (size_t i = 0; i < vect.size(); i++)
        printf("-->vect[%ld]=%d\n", i, vect[i]);

    SortedVector<int> sorted_vect;
    sorted_vect.add(1);
    sorted_vect.add(3);
    sorted_vect.add(2);
    sorted_vect.add(0);
    for (size_t i = 0; i < sorted_vect.size(); i++)
        printf("-->sorted_vect[%ld]=%d\n", i, sorted_vect[i]);

    KeyedVector<int, std::string> keyed_vect;
    keyed_vect.add(1, "World");
    keyed_vect.add(0, "Hello");
    for (size_t i = 0; i < keyed_vect.size(); i++)
        printf("-->keyed_vect[%ld] key=%d, value=%s\n", i, keyed_vect.keyAt(i), keyed_vect.valueAt(i).c_str());

    return 0;
}

