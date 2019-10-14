# Static Interface Pattern (SIP)

SIP provides the ability to implement the S.O.L.I.D. design principles in C++ without the need for virtual inheritance.

## General Problem

Suppose we have the following class:

```cpp
class A
{
public:
    void foo() { }
};
```

Now suppose we wish to use `A` in another class as a private member variable as follows:

```cpp
#include "a.h"

class B
{
public:
    void bar()
    { m_a.foo(); }

private:
    A m_a;
};
```

Finally, let us instantiate `B` in our application as follows:

```cpp
#include "b.h"

int main()
{
    B b;
    b.bar();

    return 0;
}
```

## S.O.L.I.D

The above example is a simple demonstration of a class hierarchy that most C++ programmers have implemented at one point in time or another. Simply put, this example shows a class depending on another class.

The biggest issue with the above example is it doesn't adhere to the S.O.L.I.D design principles. The S.O.L.I.D design principles are a set of 5 principles designed to address different types of common problems found in projects that leverage object oriented programming. These 5 principles are as follows:

- [Single responsibility principle](https://en.wikipedia.org/wiki/Single_responsibility_principle)
- [Open–closed principle](https://en.wikipedia.org/wiki/Open%E2%80%93closed_principle)
- [Liskov substitution principle](https://en.wikipedia.org/wiki/Liskov_substitution_principle)
- [Interface segregation principle](https://en.wikipedia.org/wiki/Interface_segregation_principle)
- [Dependency inversion principle](https://en.wikipedia.org/wiki/Dependency_inversion_principle)

Of these 5, our example has issues with 3 of them. 

### Open–Closed Principle (OCP)

The general problem above does not adhere to the OCP as `B` is not open to extension. `B` directly depends on `A`, which means that any changes to `A` will change `B`. In other words, there is no way to add functionality to `A` without `B` knowing about it. To fix this, we will need to provide `B` with an interface to `A` instead of directly relying on `A` itself.

### Interface Segregation Principle (ISP)

Since we do not define `A`'s and `B`'s responsibilities, we do not know if `A` or `B` provide the needed level of abstraction to prevent clients from depending on interfaces they do not need. We can, however, still state that the general problem above does not adhere to the ISP as clients of `B` are required to include the definition of `A`, since `B`'s definition includes `A`'s definition. The larger a project gets, the more this type of problem will result in hard to debug dependency chains. To solve this problem, both `A` and `B` will need their own interfaces that do not include the "details" of their implementations.

### Dependency Inversion Principle (DIP)

The general problem above does not adhere to the DIP as both `A` and `B` fail to depend on their own interfaces, meaning clients of both `A` and `B` will have to depend on the "details" of `A` and `B` instead of the interfaces of `A` and `B`.

## Dynamic Interfaces

Typically in C++, abstraction is implemented using pure virtual interfaces (i.e., dynamic interfaces) that leverage runtime polymorphism to separate the details of an object from its interface. For example:

```cpp
class AInterface
{
public:
    virtual ~AInterface() = default;
    virtual void foo() = 0;
};
```

The above pure virtual interface defines the interface for `A`. Using this interface, `A` can be defined as the following:

```cpp
#include "a_interface.h"

class A :
    public AInterface
{
public:
    void foo() override { }
};
```

As shown above, `A` now inherits our interface, and overrides the foo() function. To implement `B`, we must first define its interface as follows:

```cpp
class BInterface
{
public:
    virtual ~BInterface() = default;
    virtual void bar() = 0;
};
```

With the above interface defined for `B`, we can now define `B` as the following:

```cpp
#include "b_interface.h"
#include "a_interface.h"

class B :
    public BInterface
{
public:
    B(AInterface *a) :
        m_a{a}
    { }

    void bar() override
    { m_a->foo(); }

private:
    AInterface *m_a;
};
```

As shown above, `B` now stores a pointer to the interface of `A` and not an instance of `A`, which is initialized in the constructor (resulting in an ownership issue as someone else must now instantiate A). Since the defintion of `B` no longer creates an instance of `A`, it too only depends on the interface and not `A` itself, meaning `B` only includes the interface.

To use `A` and `B`, we can do the following:

```cpp
#include "a.h"
#include "b.h"

int main()
{
    A a;
    B b(&a);
    b.bar();

    return 0;
}
```

As shown above, we can see that `A` and `B` now adhere to the S.O.L.I.D principles. Since `B` only depends on a pointer to the interface of `A`, `A` can change without changing `B`. In addition, we can provide `B` with any version of `A` we want meaning that `B` is open to extension while closed to modifications. For example, we can implement a unit test of `B` as follows:

```cpp
#include "a_interface.h"
#include "b.h"

#include <iostream>

class A_mock :
    public AInterface
{
public:
    void foo() override
    { std::cout << "mocked foo\n"; }
};

int main()
{
    A_mock a;
    B b(&a);
    b.bar();

    return 0;
}
```

As shown above, we can use `A`'s interface to mock `A`, without making any modifications to `B`, meaning we now adhere to OCP. We also adhere to ISP as `B` no longer includes the definition of `A`, only the interface, and we also adhere to DSP as both `A` and `B` only depend on interfaces.

There are some issues with dynamic interfaces however. The first issue with dynamic interfaces is they add additional overhead. For example, with some tricks (can be seen in the source code examples) to ensure inlining is controlled, we can see the main function for the general problem looks like this following:

```
0000000000401020 <main>:
  401020:	e8 fb 00 00 00       	callq  401120 <_ZN1A3fooEv.isra.0>
  401025:	31 c0                	xor    %eax,%eax
  401027:	c3                   	retq
```

The resulting code of this same logic using dynamic interfaces results in the following:

```
0000000000401040 <main>:
  401040:	48 83 ec 18          	sub    $0x18,%rsp
  401044:	48 c7 44 24 08 60 20 	movq   $0x402060,0x8(%rsp)
  40104b:	40 00
  40104d:	48 8d 7c 24 08       	lea    0x8(%rsp),%rdi
  401052:	e8 f9 00 00 00       	callq  401150 <_ZN1A3fooEv>
  401057:	31 c0                	xor    %eax,%eax
  401059:	48 83 c4 18          	add    $0x18,%rsp
  40105d:	c3                   	retq
```

The extra logic seen above is needed to initialize the vTable of `A` and get access to the function pointer to `foo()`. Whether or not this overhead actually ends up becoming a problem for your applications depends on your application as modern compilers are amazing at removing the overhead of virtual inheritance.

The other issue with dynamic interfaces is they do not support static functions. Meaning, if `A` defines a static function that `B` needs to use, this entire scheme no longer works as there is no way to define a static function in a pure virtual interface.

## Static Interfaces

The goal of the static interface pattern (SIP) is to address the issues of dynamic interfaces by implementing abstraction without the need for virtual inheritance. To accomplish this, we will use the following class:

```cpp
template<
    template<typename> typename INTERFACE,
    typename DETAILS
    >
class type :
    public INTERFACE<type<INTERFACE, DETAILS>>
{
    using details_type = DETAILS;

    DETAILS d;
    friend class INTERFACE<type<INTERFACE, DETAILS>>;

    constexpr static DETAILS*
    details(INTERFACE<type<INTERFACE, DETAILS>> *i)
    { return &(static_cast<type<INTERFACE, DETAILS> *>(i)->d); }

    constexpr static const DETAILS*
    details(const INTERFACE<type<INTERFACE, DETAILS>> *i)
    { return &(static_cast<const type<INTERFACE, DETAILS> *>(i)->d); }
};
```

The above class implements Static Polymorphism (also called Curiously Recurring Template Pattern). The difference is, the above class provides a means to define an object's interface and implementation separately and then combine different implementations with the same interface as needed, all at compile time.

To better understand how this class works, let us first look at `A`'s interface as follows:

```cpp
namespace interface
{

template<typename T>
struct A
{
    constexpr void foo()
    { T::details(this)->foo(); }
};

}
```

As shown above, the interface is defined using a template. We wrap the interface in an "interface" namespace so that the interface can be called `interface::A`. Each of the functions within the interface call into its own subtype using the `details` function, which returns a pointer to the subtype's private implementation.

With this interface, `A` is defined as follows:

```cpp
#include "a_interface.h"

namespace details
{

class A
{
public:
    void foo() { }
};

}

using A = type<interface::A, details::A>;
```

As shown above, `A` is defined the same as our general problem. The only difference is, we wrap the definition of `A` in a namespace called "details" allowing us to call the implementation `details::A`. From here, we use our `type` class to actually create `A` with `using A = type<interface::A, details::A>`.

The next step is to define the interface for `B` as follows:

```cpp
namespace interface
{

template<typename T>
struct B
{
    constexpr void bar()
    { T::details(this)->bar(); }
};

}
```

As shown above, we use the same pattern as above to create our interface. `B` is define as follows:

```cpp
#include "a_interface.h"
#include "b_interface.h"

namespace details
{

template<typename T>
class B
{
public:
    void bar()
    { m_a.foo(); }

private:
    interface::A<T> m_a;
};

}

template<typename T>
using B = type<interface::B, details::B<T>>;
```

As shown above, the definition of `B` is identical the the general problem with some exceptions. First, `B` is now a template class so that we can provide `B` with different versions of `A` as needed. Instead of directly instantiating `A`, we instantiate `A` using its interface using static polymorphism. Like `A`, `B` is implemented in a "details" namespace, and defined using the `type` class. The difference is that `B` must remain a template type to ensure we can give it whatever `A` we want.

To use this code, we can do the following:

```cpp
#include "a.h"
#include "b.h"

int main()
{
    B<A> b;
    b.bar();

    return 0;
}
```

Compared to dynamic interfaces, static interfaces solve the ownership issues as `A` is instantiated in `B` without `B` having to rely on the definition of `A`. Static interfaces also support static functions as the interface can use the `details_type` type to access a static function, allowing the interface to override static functions (something dynamic interfaces cannot do).

With respect to performance, once again, this depends on your application. If we compare the resulting binary of our static interface with the resulting binary of our general problem (with --strip-all to remove strings), the binaries are identical (byte for byte), meaning the compiler is able to take the above template code and reduce it to the same code as the general problem. This doesn't mean that static interfaces do not come at a cost as the applications string table is much larger with all of the additional decorations (so strip them), and the above code is far more difficult to understand without really knowing how templates work including static polymorphism, so there is a human cost to this abstraction (something C++20 will likely address with C++ Concepts)

For reference, mocking works as follows:

```cpp
#include "a_interface.h"
#include "b.h"

#include <iostream>

namespace details
{

class A_mock
{
public:
    void foo()
    { std::cout << "mocked foo\n"; }
};

}

using A_mock = type<interface::A, details::A_mock>;

int main()
{
    B<A_mock> b;
    b.bar();

    return 0;
}
```

In general, static interfaces address all of the problems that dynamic interfaces introduce when attempting to add abstraction and adhere to S.O.L.I.D at the expense of being more difficult to understand.
