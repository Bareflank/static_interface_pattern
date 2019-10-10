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

## Performance Comparisons

## C++20 Concepts
