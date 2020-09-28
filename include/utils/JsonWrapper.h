/* The MIT License (MIT)
 *
 * Copyright (c) 2019-2020 luoyun <sysu.zqlong@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef __MSGUTILS_JSON_WRAPPER_H__
#define __MSGUTILS_JSON_WRAPPER_H__

#include <stdio.h>
#include <stdbool.h>
#include <string>
#include "Namespace.h"

MSGUTILS_NAMESPACE_BEGIN

struct cJSON;

class JsonWrapper {
public:
    JsonWrapper();
    JsonWrapper(const char* input);
    JsonWrapper(cJSON* node, bool owner=false);
    JsonWrapper(const JsonWrapper&);
    JsonWrapper& operator=(const JsonWrapper&);
    JsonWrapper(JsonWrapper&& rhs);
    JsonWrapper& operator=(JsonWrapper&& rhs);

    ~JsonWrapper();

    bool isValid() const;
    void parse(const char* input);
    void assign(cJSON* node);
    void setObjectType();
    void setArrayType();
    JsonWrapper getObject(const char* name) const;
    JsonWrapper getArray(const char* name) const;
    JsonWrapper getArray(int index) const;
    JsonWrapper getChild();
    JsonWrapper getNext();
    JsonWrapper getPrev();
    JsonWrapper* take();
    cJSON* release();

    bool fromFile(const std::string& filePath);
    bool toFile(const std::string& filePath) const;
    std::string toString(bool formatted=false) const;

    const cJSON* getRoot() const { return m_root; }
    cJSON* getRoot() { return m_root; }
    cJSON* duplicate() const;
    const char* getName() const;
    int getArraySize() const;

    int getIntValue(const char* name, int defValue) const;
    int getIntValue(int index, int defValue) const;
    unsigned int getUIntValue(const char* name, unsigned int defValue) const;
    unsigned int getUIntValue(int index, unsigned int defValue) const;
    bool tryGetIntValue(const char* name, int& value) const;
    bool tryGetUIntValue(const char* name, unsigned int& value) const;
    bool tryGetStringValue(const char* name, std::string& value) const;
    double getDoubleValue(const char* name, double defValue) const;
    double getDoubleValue(int index, double defValue) const;
    const char* getStringValue(const char* name, const char* defValue) const;
    const char* getStringValue(int index, const char* defValue) const;
    const char* getStringValue(const char* defValue) const;
    bool getBooleanValue(bool defValue) const;
    int getIntValue(int defValue) const;
    unsigned int getUIntValue(unsigned int defValue) const;
    double getDoubleValue(double defValue) const;
    bool getBooleanValue(const char* name, bool defValue) const;
    bool getBooleanValue(int index, bool defValue) const;
    bool tryGetBooleanValue(const char* name, bool& value) const;
    void setIntValue(int value);
    void setIntValue(const char* name, int value);
    void setUIntValue(unsigned int value);
    void setUIntValue(const char* name, unsigned int value);
    void setDoubleValue(double value);
    void setDoubleValue(const char* name, double value);
    void setBooleanValue(bool value);
    void setBooleanValue(const char* name, bool value);
    void setStringValue(const char* value);
    void setStringValue(const char* name, const char* value);
    void setStringValue(const char* name, const std::string& value) { return setStringValue(name, value.c_str()); }
    void setObjectValue(const char* name, const JsonWrapper& value);
    void setObjectValue(const char* name, const cJSON* value);
    void setArrayValue(const char* name, const JsonWrapper& value);

    bool isNull(const char* name) const;
    bool isNull(int index) const;
    bool isString(const char* name) const;
    bool isString(int index) const;
    bool isNumber(const char* name) const;
    bool isNumber(int index) const;
    bool isBoolean() const;
    bool isBoolean(const char* name) const;
    bool isBoolean(int index) const;
    bool isObject(const char* name) const;
    bool isObject(int index) const;
    bool isArray(const char* name) const;
    bool isArray(int index) const;
    bool isArray() const;
    bool isObject() const;
    bool isString() const;
    bool isInt() const;
    bool isUInt() const;
    bool isDouble() const;

    bool hasNode(const char* name) const;
    cJSON* findNode(const char* name) const;
    bool eraseNode(const char* name);

    bool addStringValueToArray(const char* value);
    bool addIntValueToArray(int value);
    bool addUIntValueToArray(unsigned int value);
    bool addBoolValueToArray(bool value);
    bool addItemToArray(cJSON* item);
    bool addItemToObject(const char* name, cJSON* item);

    void merge(const char* profile);
    void merge(JsonWrapper& profile);
    void reverseMerge(const char* profile);
    void reverseMerge(JsonWrapper& profile);

private:
    cJSON* findNode(int index) const;
    cJSON* findNode(const char* name, int type) const;
    void doMerge(cJSON* root1, cJSON* root2);

private:
    cJSON* m_root;
    bool m_owner;
};

MSGUTILS_NAMESPACE_END

#endif // __MSGUTILS_JSON_WRAPPER_H__
