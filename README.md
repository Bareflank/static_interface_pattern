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

There are however several disadvantages to the above example:
- **S.O.L.I.D**: The example doesn't adhere to the S.O.L.I.D design principles.
- **Testability**: The example is hard to test. There are different approaches including mocking libraries, and build system tricks that can help to reduce or even remove this issue, but in general, the design doesn't include a simple means to implement unit level testing.

The S.O.L.I.D design principles are a set of 5 principles designed to address different types of common problems found in projects that leverage object oriented programming. These 5 principles are as follows:

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

Our example does not adhere to the open-closed principle. To better understand this, let us look at the definition of B:

```cpp
struct B
{
    void bar();
    A m_a;
};
```

Although it is possible to inherit B and add additional functionality, if A needs to be replaced, B would have to be redefined and likely reimplemented. That is, the definition of B is highly coupled with A. Any changes to A will likely affect B.

There are a couple of ways to solve this problem. We could use an abstract interface, which is usually implemented using pure virtual inheritance as follows:

```cpp
class interface
{
public:
    virtual ~interface() = default;
    virtual void foo() = 0;
};
```

With A being defined as follows:

```cpp
class A :
    public interface
{
public:
    void foo() override;
};
```

We can now implement B as follows:

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

As shown above, instead of `B` depending on `A`, it depends on a template type `T`. This pattern is used extensively in C++. Instead of `A` implementing an interface, `A` must implement a "Concept". After we explain what the SAIP is, we will take this example further with how C++20's Concepts can be used to ensure that `T` implements the agreed upon Concept with easy to read compiler error messages.

### Liskov substitution principle

The Liskov substitution principle (LSP) ensures that if a subtype reimplements an object's API, the API behaves the same. In other words, you should be able to substitue a subtype with its parent without any addition modifications to your program.

Our example above doesn't leverage inheritance and therefore there are no subtypes. Later on, however, we will show how S.O.L.I.D can be implemented using dynamic and static polymorphism, both of which leverage inheritance in a way that requires that the LSP is adhered to.

### Interface segregation principle

### Dependency inversion principle

## Dynamic Abstraction

## Static Abstraction
