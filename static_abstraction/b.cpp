//
// Copyright (C) 2019 Assured Information Security, Inc.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "b.h"

template<typename T>
void
B<T>::bar()
{
    m_a.foo();
}

// Notes:
//
// - There are two different ways that we can implement B: inclusion or
//   explicit instantiation.
// - The "bad" code requires that B know what A is. If all we care about is
//   that the definition of B not include the definition of A, but B's
//   implementation can include the definition of A, we can use the explicit
//   instantiation model as defined below. Using this model, we get the exact
//   same implementation as "bad" when doing a diff of the resulting
//   assembly.
// - If we require the "B" knows nothing about A, and only every knows what
//   the interface is (similar to dynamic abstraction), we do not need the
//   code below, but instead, when we define B, we must include this file to
//   ensure we get a complete implementation of B using the inclusion model.
//   Note that this is the preferred model as the compiler can perform
//   additional optimizations, which is shown in our example as the resulting
//   code is smaller than the bad case, which we consider the control (i.e.,
//   we consider the bad case to be our ideal case for performance and
//   binary size, but in fact, static abstraction can do better).
//

// Uncomment the following if you wish to prove that static abstraction
// results in the same executable as the bad case.
//
// #include "a.h"
// template class B<A>;
