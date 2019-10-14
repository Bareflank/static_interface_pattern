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

#ifndef HELPERS_H
#define HELPERS_H

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

#endif
