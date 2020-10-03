/*
 * Copyright (C) 2018-2020 luoyun <sysu.zqlong@gmail.com>
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

#ifndef __MSGUTILS_NAMESPACE_H__
#define __MSGUTILS_NAMESPACE_H__

#define NAMESPACE_BEGIN(name) namespace name {
#define NAMESPACE_END()       }
#define NAMESPACE_USING(name) using namespace name;

#define MSGUTILS_NAMESPACE_BEGIN NAMESPACE_BEGIN(msgutils)
#define MSGUTILS_NAMESPACE_END   NAMESPACE_END()
#define MSGUTILS_NAMESPACE_USING NAMESPACE_USING(msgutils)

#endif /* __MSGUTILS_NAMESPACE_H__ */
