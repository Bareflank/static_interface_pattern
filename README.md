# Static Abstract Interface Pattern

The static abstract interface pattern (SAIP) provides the ability to implement the S.O.L.I.D. design principles, including the use of abstract interfaces without the need for virtual inheritance, improving the run-time performance of abstraction.

## General Problem

Suppose we have the following class:

```cpp
class A
{
public:
    void foo();
};
```

With the following implementation:

```cpp
void
A::foo()
{ }
```

Now suppose we wish to use this class in another class as a private member variable as follows:

```cpp
struct B
{
    void bar();
    A m_a;
};
```

With the following implementation:

```cpp
void
B::bar()
{
    m_a.foo();
}
```

Finally, let us instantiate `B` in our application as follows:

```cpp
int main()
{
    B b;
    b.bar();

    return 0;
}
```

## S.O.L.I.D

The above example is a simple demonstration of a class hierarchy that most C++ programmers have implemented at one point in time or another. Simply put, this example shows a class depending on another class.

There are some advantages to the above example:
- **Performance**: This example performs at run-time really well. In fact, it performs so well, we will use this example as our "baseline" throughout this explanation as our goal with the static abstract interface pattern is to perform at least as good or better when compared to this example.
- **Readability**": This example is simple to understand and read, which is why this example is always used when teaching C++.

The biggest issue with the above example is it doesn't adhere to the S.O.L.I.D design principles. The S.O.L.I.D design principles are a set of 5 principles designed to address different types of common problems found in projects that leverage object oriented programming. These 5 principles are as follows:

- [Single responsibility principle](https://en.wikipedia.org/wiki/Single_responsibility_principle)
- [Open–closed principle](https://en.wikipedia.org/wiki/Open%E2%80%93closed_principle)
- [Liskov substitution principle](https://en.wikipedia.org/wiki/Liskov_substitution_principle)
- [Interface segregation principle](https://en.wikipedia.org/wiki/Interface_segregation_principle)
- [Dependency inversion principle](https://en.wikipedia.org/wiki/Dependency_inversion_principle)

Let us dive deeper into each of these principles to see how our example handles each one.

### Single responsibility principle

The single responsibility principle (SRP) states that an object should only be responsible for one job. For example, a car is capable of driving and playing music:

```cpp
struct car
{
    void drive();
    void play_music();
};
```

The above example doesn't adhere to the SRP as the car has two different responsibilities: driving and playing music. Failure to adhere to the SRP produces tight coupling in your source code. Changes to how the car drives could inadvertently affect how the car plays music. There are many ways to address this issue including delegation and inheritance. For example:

```cpp
struct engine
{
    void drive();
};

struct stereo
{
    void play_music();
};

struct car :
    public engine,
    public stereo
{
};
```

In the example above, we are now adhering to the SRP. An `engine` is responsible for driving, while the `stereo` is responsible for playing music. The `car` is responsible for integrating the two into a single object, but doesn't include its own logic (its responsibility is integration, not logic).

In our example problem above, we correctly adhere to the SRP. The `A` object is responsible for executing `foo()` while the `B` object is responsible for executing `bar()`. The issue with the general problem above has more to do with the fact that `B` needs the `foo()` function to implement `bar()`. In other words, the SAIP doesn't exist to address SRP, but SRP itself can lead to the need for SAIP.

### Open–closed principle

The open-closed principle (OCP) states that you should be able to extend the interface of a object without actually modifying the object's interface or implementation of that interface.

To better explain how this principle works, let us look at the example above as it does not adhere to the open-closed principle. Specifically, let us look at the definition of `B`:

```cpp
struct B
{
    void bar();
    A m_a;
};
```

Although it is possible to inherit `B` and add additional functionality, if `A` needs to be replaced, `B` would have to be redefined and likely reimplemented. That is, the definition of `B` is highly coupled with `A`. Any changes to `A` will likely affect `B`.

There are a couple of ways to solve this problem. We could use an abstract interface, which is usually implemented using pure virtual inheritance as follows:

```cpp
class interface
{
public:
    virtual ~interface() = default;
    virtual void foo() = 0;
};
```

With `A` being defined as follows:

```cpp
class A :
    public interface
{
public:
    void foo() override;
};
```

We can now implement `B` as follows:

```cpp
struct B
{
    void bar();
    interface *m_a;
};
```

Instead of `B` relying on `A`, it relies on `interface`. So long as `A` continues to implement the abstract interface, it can change as needed without affecting `B`. Another way to solve this problem is through the use of template classes as follows:

```cpp
template<typename T>
struct B
{
    void bar();
    T m_a;
};
```

As shown above, instead of `B` depending on `A`, it depends on a template type `T`. This pattern is used extensively in C++. Instead of `A` implementing an interface, `A` must implement a "Concept". Both of the examples above, however, do not address the binary compatibility of `B`. Although `B` doesn't rely on `A` (it either relies on an interface or a Concept), the binary layout of `B` still changes if the interface or concept changes.

To adhere to the true intent of the OCP, we can leverage a design pattern called a private implementation, or "PIMPL". For example:

```cpp
struct B
{
    B();
    ~B();

    void bar();
    void *d;
};
```

In the definition of `B` above, we no longer store a pointer to an interface of `A`. Instead, we store a `void *` pointer. The implementation of `B` is as follows:

```cpp
struct pimpl
{
    pimpl() :
        m_a{new A}
    { }

    ~pimpl()
    {
        delete m_a;
    }

    void bar()
    {
        m_a->foo();
    }

    interface m_a;
};

B::B() :
    d{new pimpl}
{ }

~B::B()
{
    delete pimpl;
}

void
B::bar()
{
    static_cast<pimpl *>(d)->bar();
}
```

As shown above, we start with the definition and implementation of a private `pimpl` class. When `B` is created, it allocates a pointer to the `pimpl` class and stores it in `B` as a `void *`, meaning the definition of `B` does not include any information about the `pimpl`, and instead just stores a raw pointer. Whenever, `B` needs to call `bar()`, it does so by forwarding the call to the `pimpl`.

Now, if `A` changes, the binary layout of `B` doesn't change, only the binary layout of the `pimpl` changes. The downside to how this works is the call to `foo()` now must go through several layers of abstraction including the `pimpl`, and the vTable introduced by the use of pure virtual inheritance. It should also be noted that this same approach can also be implemented using the template class approach, which at least removes the vTable overhead. NBoth approaches however still also require the addition of heap memory (hence the use of `new`).

In other words, unless you are writing a library that is intended to maintain binary compatibility, the most important part of the OCP

### Liskov substitution principle

The Liskov substitution principle (LSP) ensures that if a subtype reimplements an object's API, the API behaves the same. In other words, you should be able to substitue a subtype with its parent without any addition modifications to your program.

Our example above doesn't leverage inheritance and therefore there are no subtypes. Later on, however, we will show how S.O.L.I.D can be implemented using dynamic and static polymorphism, both of which leverage inheritance in a way that requires that the LSP is adhered to.

### Interface segregation principle

The interface segregation principle (ISP), states that client of an API should only depend (or even be made aware of) the APIs they depend on. In some sense, this principle is similar to the SRP. Instead of one large object with hundreds of APIs, a good design would be broken up into many smaller objects, each with their own, single set of responsibilities.

What distinguishes this principle from the SRP is clients should only be made aware of APIs they require. In C++, this translates to header files. In our example above, the ISP is not adhered to as `B` includes `A` in its own include file. This means that clients of `B` must also be made aware of `A` to work. In other words, clients of `B`, who only care about the APIs that `B` provides must also include the header files that define `A`, and as a result, become dependent on `A` even though they don't need to.

Although this seems like a trival problem, this type of issue creates a chain of dependencies that quickly turns into a nightmare in larger projects, even at times generating issues with circular references. Furthermore, this problem often times increases the expense of refactoring code as changing `A` could also change `B`.

The solution to this problem involves the use of abstract interfaces (as is with most of the principles defined by S.O.L.I.D).

### Dependency inversion principle

Finally, we have the dependency inversion principle (DIP), which states that clients should never depend on their API's concrete defintions, but rather an abstract interface. That is to say, an API adheres to and implements an abstract interface ensuring clients only every need to include the abstract interface. The DIP is the logical solution to most of the problems identified by S.O.L.I.D, and in most texts on the issue, pure virtual inheritance is the preferred approach.

## Dynamic Abstraction

Dynamic abstraction is simply another way of saying pure virtual abstract interfaces, or run-time polymorphism. Most of the literature on the subject leverages this approach to implementing the S.O.L.I.D principles. To better understand how this works, let us refactor our original example using dynamic abstraction.

```cpp
class interface
{
public:
    virtual ~interface() = default;
    virtual void foo() = 0;
};
```

We start with a defintion of our abstract interface using pure virtual abstraction as shown above. All of the functions in our interface a marked as pure virtual, and we have marked the interface's destructor as virtual to ensure deletion if handled properly. From here, `A` can be defined as follows:

```cpp
#include "interface.h"

class A :
    public interface
{
public:
    void foo() override;
};
```

As shown above, `A` now inherits our interface, and overrides the foo() function in the interface. An important note here is that the defintion of `A` only includes the interface. The implementation of `A` doesn't change as before. The defintion of `B` however, now looks like this:

```cpp
#include "interface.h"

struct B
{
    B(interface *a);

    void bar();
    interface *m_a;
};
```

As shown above, `B`, now stores a pointer to the interface of `A` and not an instance of `A` directly. Since the defintion of `B` no longer creates an instance of `A`, it too only depends on the interface and not `A` itself, meaning `B` only includes the interface. For this system to work, we must store a pointer to an interface and not the interface itself as a complete representation of `B` must exist somewhere. As a result, this abstraction introduces an ownership issue that we must resolve. Some implementations could allocate `A` in the constructor of `B`, which is fine so longer as this code is located in its own source file, hiding the details of `A` from `B`'s clients. In our example, however, we will pass a pointer to `A` to `B`'s constructor, meaning something else will own the lifetime of `A`.

The implementation of `B` now looks like this:

```cpp
#include "b.h"

B::B(interface *a) :
    m_a{a}
{ }

void
B::bar()
{
    m_a->foo();
}
```

As shown above, `B` only ever relies on the interface. Details about how `A` is implemented are irrelevant to `B`. From here, our main logic looks like this:

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

If we take a quick look at the S.O.L.I.D principles, we can see that we still adhere to the SRP as `A` and `B` still have single responsibilities. We also adhere to the OCP as we are still free to extend `A` and `B`, and modifications to `A` do not effect `B`. In this implementation, we do not implement the `pimpl` portion of this concept as we are not concerned about binary compatibility (as we are not creating a library), but instead on coupling and cohesion. We adhere to the LSP as we are inheriting a pure virtual interface and as such, the interface's behavior has not yet been defined (meaning our implementation of the interface can take on anything and still adhere to the LSP). We adhere to the ISP as we adhere to the SRP, our interfaces are as small as we can make them and the defintions of `A` and `B` do not expose clients to implementation specific details but rather these defintions only ever include the interface. Finally we adhere to the DIP as `A` and `B` are interconnected via the interface and never directly include each other (meaning `B`'s dependency on `A` is inverted).

With the example above, we have adhered to the S.O.L.I.D principles. The problem with this example is we have introduced a layer of abstraction that comes with a run-time cost. Whenever `B` executes `A`, it must first perform a vTable lookup and function call redirection. Furthermore, the compiler is not able to inline the call to `A` as it is not sure if it is calling `A` or some other object `C` that also adheres to same interface.

The fundamental problem with dynamic abstraction is it uses run-time polymorphism to perform abstraction on code that was originally static in nature. Run-time polymorphism is designed to allow the programmer, at run-time, to swap between a type and its subtype as needed. Not to ensure object oriented code is properly designed and implemented to reduce coupling and increase cohesion. By using dynamic abstraction to solve this problem, we are introducing run-time performance penalties.

This design approach does have some advantages. For example, it is easy to read and understand. Run-time polymorphism has been around forever and as such, the C++ language has first-class facilities to ensure it is easy to use and understand. This approach also provides a clean way to unit test `B` by providing a mock of `A` as follows:

```cpp
#include "interface.h"
#include "b.h"

#include <iostream>

struct A_mock :
    public interface
{
    void foo() override
    {
        std::cout << "mocked foo\n";
    }
};

int main()
{
    A_mock a;
    B b(&a);
    b.bar();

    return 0;
}
```

As shown above, instead of including `A` in our `main()` function, we include the interface and create our own version of `A` that mocks the interface, allowing us to test `B` as needed.

## Static Abstraction

The goals of static abstraction are to leverage the design advantages of dynamic abstraction without the performance hits it imposes. To accomplish this, we will start by implementing our interface as follows:

```cpp
template<typename T>
struct interface
{
    constexpr void foo()
    {
        static_cast<T *>(this)->d.foo();
    }
};
```

To better understand how this will work, let us also show how `A` is defined:

```cpp
#include "interface.h"

class A :
    public interface<A>
{
    struct details {
        void foo();
    };

    details d;
    friend class interface<A>;
};
```

The combination of `A` and the interface is a pattern called static polymorphism (also referred to as curiously recurring template pattern). `A` inherits the interface, which intern provides a defintion of itself to the interface. The interface itself makes call back to `A` as needed which provides the interface at compile-time with an implementation of itself. Finally, we wrap the implementation of `A` into a `details` structure to ensure that anything using the interface can only ever access functions from the interface (ensuring the user of the interface doesn't accidentially introduce coupling)

Although static polymorphism is critical to the design of the static abstract interface pattern, it is not enough. `B` itself must only ever depend on the interface. The problem with static polymorphism is that you cannot simply store a pointer to the interface to access the functions the interface contains. Instead, if you attempt to access the interface's functions, you must also know information about the type provided to the interface. Meaning, the interface and its implementation are always coupled at compile-time, a problem that dynamic polymorphism does not share. To solve this problem, we complete our abstract interface pattern with the following defintion of `B`:

```cpp
#include "interface.h"
#include <type_traits>

template<typename T>
struct B
{
    static_assert(std::is_base_of_v<interface<T>, T>);

    void bar();
    T m_a;
};
```

As shown, `B` is also defined as a template. We leverage std::is_base_of to ensure that `T` adheres to the interface and we store our instance of T as a member of `B`, just like our original example. Note that if binary compatibility (as defined by OCP) is required, you could and a pimpl to `B` as well to store `T`.

One issue with the use of a template definition of `B` is where you place the source code. Typically, template classes are defined and implemented in a header file. For a large project, with a lot of objects, this is a terrible idea as compile times would be terrible. It also reintroduces issues with the ISP as your implementation will almost certainly have its own dependencies and includes that you do not want in your definition of `B` (once again, we only want `B`'s header to contain the interface, but if the same header file implements `B` as well, you might have other headers that you would have to include). To address this, we will place the implementation of `B` in its own source file as follows:

```cpp
#include "b.h"

template<typename T>
void
B<T>::bar()
{
    m_a.foo();
}
```

As shown above, `B` is able to call the `foo()` function as needed, and it cannot call any functions not in the interface as `B` itself is actually implemented in the `details` nested class as private, preventing access or even visibility to any of these functions.

Of course the question is, how do we compile this code as it is a template, and not actual source code. In our previous example, the implementation of `B` statically stated that `A` was the "thing" that `B` depended on. The goal of the static abstract interface pattern is to adhere to S.O.L.I.D, and not to simply use generic programming everywhere. Meaning, nothing about the original problem has changed. `B` still relies on `A`, and therefore adding the following to `B`'s implementation is an option:

```cpp
#include "a.h"
template class B<A>;
```

The above code is called "explicit instantiation", and results in a complete definition of `B` given `A`, meaning we can now use `B<A>` anywhere we include the definition of `B`. Note that since the definition of `B` doesn't include the defintion of `A`, our code still adheres to S.O.L.I.D. Another option is to include the implementation of `B` when `B` is actually instantiated as follows:

```cpp
#include "a.h"
#include "b.h"
#include "b.cpp"

int main()
{
    B<A> b;
    b.bar();

    return 0;
}
```

As shown above, we include "b.cpp" which ensures that we get a complete implementation of `B` using the "inclusion" model. Note that both the explicit instantiation model, or the inclusion model work, and it just depends on how your project is organized. As will be shown, the inclusion model is the preferred approach as the compiler is given a complete defintion of the template class, allowing for further optimizations that even our general problem above cannot realize.

With respect to testing, like dynamic abstraction, static abstraction also supports testing as follows:

```cpp
#include "interface.h"
#include "b.h"
#include "b.cpp"

#include <iostream>

struct A_mock :
    public interface<A_mock>
{
    void foo()
    {
        std::cout << "mocked foo\n";
    }
};

int main()
{
    B<A_mock> b;
    b.bar();

    return 0;
}
```

As shown above, instead of including `A`, we include the interface and create a mocked version of `A` so that we can test `B`. The difference is this type substitution occurs without the need for virtual inheritance improving performance.

## Performance Comparisons

To prove that the static abstract interface pattern performs as good or better than the approach taken in the general problem, let us create a simple set of performance tests.

First, we will need a control as follows:

```cpp
#include "b.h"

int main()
{
    B b;

    for (auto i = 0ULL; i < 1000000000; i++) {
        b.bar();
    }

    return 0;
}
```

The above performance test uses our "bad" implementation defined in the original general problem. If we execute this code using "time" we get the following output (on an Intel(R) Core(TM) i7-7500U):

```bash
> time ./bad/bad_perf

real    0m1.466s
user    0m1.461s
sys     0m0.002s
```

If we run the same test using dynamic abstraction, we get the following:

```bash
> time ./dynamic_abstraction/dynamic_abstraction_perf

real    0m1.758s
user    0m1.748s
sys     0m0.006s
```

Clearly, dynamic abstraction runs slower (20% slower). Now let us run the same performance test using the explicit instantiation version of our static abstract interface:

```bash
> time ./static_abstraction/static_abstraction_perf

real    0m1.461s
user    0m1.456s
sys     0m0.003s
```

As shown, our static abstract interface performs as good as the general solution does that doesn't adhere to S.O.L.I.D, meaning static abstraction doesn't impose a run-time performance cost, but still provides the needed abstraction to remain compliant. It should also be noted that if you do a binary diff between the explicit instantiation version of our pattern with the general problem, you get a byte for byte exact copy. Meaning, even though static abstraction adds all of the additional template source code overhead, the resulting code is byte for byte identical to the source code without the abstraction once compiled... something dynamic abstraction cannot claim. This does not mean that static abstraction doesn't come at a cost as it is clearly more difficult to read and understand. All we claim is that it does not impose a run-time performance hit, and should outperform the general implementation with respect to build times as the reduction of include files not only decouples the code, it also helps with build times.

If we use the inclusion model, we see the following:

```bash
> time ./static_abstraction/static_abstraction_perf

real    0m1.171s
user    0m1.167s
sys     0m0.003s
```

As shown above, the inclusion model outperforms the general problem. This is because (if you look at a diff between the two resulting binaries), the inclusion model provides enough information to the compiler allowing it to remove the `bar()` and directly inline the `foo()` into the performance test. We could do the same thing if we wrote `B` as follows in the general problem:

```cpp
#include "a.h"

struct B
{
    inline void bar()
    {
        m_a.foo();
    }

    A m_a;
};
```

Once again, however, this would result in implementation details showing up in our definition, likely resulting in more dependencies and higher coupling. Using the template approach for `B` allows us to keep the definition and implementation separate, while still allowing the compiler to inline as needed (i.e. best of both worlds).

## C++20 Concepts

C++20 offers to remove the human costs of this pattern through the use of C++20 Concepts (at the expense of the C++ language and compilers being more complex). To better understand how this will work once C++20 is available, let us look at how the interface would be written:

```cpp
template<typename T>
concept interface = requires(T t) {
    { t.foo() };
};
```

As shown here, using C++20 concepts allows us to remove the need for static polymorphism from the pattern (the need for `B` to be a template is still required), allowing `A` to be defined as the following:

```cpp
class A
{
public:
    void foo();
};
```

As shown, `A` now has the same defintion as our general problem, meaning `A` is now a simple class with now additional decorations. The definition of `B` is as follows:

```cpp
#include "interface.h"

template<interface T>
struct B
{
    void bar();
    T m_a;
};
```

As shown above, `B` must still be a template, but `T` is now defined using the `interface` and not `typename`. This removes the need for the type traits static assert, and instead tells the compiler to use C++20 Concepts to ensure that `T` adheres to the Concept called `interface`. Everything else about this pattern remains the same. For more information, please see the following:

https://www.cppfiddler.com/2019/06/09/concept-based-interfaces/
