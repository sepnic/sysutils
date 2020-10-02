/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string>

#include "utils/Vector.h"
#include "utils/SortedVector.h"
#include "utils/KeyedVector.h"
#include "utils/Namespace.h"

#define LOG_TAG "VectorTest"

MSGUTILS_NAMESPACE_USING

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

