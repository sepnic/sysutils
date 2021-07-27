#include <stdio.h>
#include <unistd.h>
#include "utils/Namespace.h"
#include "utils/RefBase.h"

SYSUTILS_NAMESPACE_USING

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

void nothing1(sp<A> &a)
{
    printf("nothing1::A::getStrongCount()=%d\n", a->getStrongCount());
    sp<A> aa = a;
    printf("nothing1::A::getStrongCount()=%d\n", a->getStrongCount());
}

void nothing2(sp<A> a)
{
    printf("nothing2::A::getStrongCount()=%d\n", a->getStrongCount());
    sp<A> aa = a;
    printf("nothing2::A::getStrongCount()=%d\n", a->getStrongCount());
}

int main()
{
    sp<A> spObjA = new A;
    printf("main::A::getStrongCount()=%d\n", spObjA->getStrongCount());

    nothing1(spObjA);
    printf("main::A::getStrongCount()=%d\n", spObjA->getStrongCount());

    nothing2(spObjA);
    printf("main::A::getStrongCount()=%d\n", spObjA->getStrongCount());

    wp<B> wpObjB = new B;
    sp<B> spObjB2 = wpObjB.promote();
    spObjB2->show();

    return 0;
}
