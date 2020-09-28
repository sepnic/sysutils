#include <stdio.h>
#include <unistd.h>
#include "utils/Namespace.h"
#include "utils/RefBase.h"

MSGUTILS_NAMESPACE_USING

class A : public RefBase
{
public:
    A()
    {
        printf("A::A()\n");
    }
 
    ~A()
    {
        printf("A::~A()\n");
    } 
};

class B : public RefBase
{
public:
    B()
    {
        printf("B::B()\n");
    }
 
    ~B()
    {
        printf("B::~B()\n");
    }

    void show()
    {
        printf("B::show()\n");
    } 
};

int main()
{
    sp<A> spObj = new A;

    wp<B> wpObj = new B;
    sp<B> spObj2 = wpObj.promote();
    spObj2->show();

    return 0;
}
