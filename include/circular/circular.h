/******************************************************************************
 * $Id:  $
 * $Name: $
 *
 * Author: Pete Goodliffe
 *
 * ----------------------------------------------------------------------------
 * Copyright 2002 Pete Goodliffe
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:

 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Released under MIT license with permission from the author.
 * ----------------------------------------------------------------------------
 * Purpose: STL-style circular buffer
 *
 * ----------------------------------------------------------------------------
 * History: See source control system log.
 *
 *****************************************************************************/

#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <algorithm>
#include <exception>
#include <iterator>
#include <memory>

/******************************************************************************
 * Iterators
 *****************************************************************************/

/**
 * Iterator type for the circular_buffer class.
 *
 * This one template class provides all variants of forward/reverse
 * const/non const iterators through plentiful template magic.
 *
 * You don't need to instantiate it directly, use the good public functions
 * availble in circular_buffer.
 */
template <typename T,                                  //circular_buffer type
                                                       //(incl const)
          typename T_nonconst,                         //with any consts
          typename elem_type = typename T::value_type> //+ const for const iter
class circular_buffer_iterator
{
    public:

        typedef circular_buffer_iterator<T,T_nonconst,elem_type> self_type;

        typedef T                                   cbuf_type;
        typedef std::random_access_iterator_tag     iterator_category;
        typedef typename cbuf_type::value_type      value_type;
        typedef typename cbuf_type::size_type       size_type;
        typedef typename cbuf_type::pointer         pointer;
        typedef typename cbuf_type::const_pointer   const_pointer;
        typedef typename cbuf_type::reference       reference;
        typedef typename cbuf_type::const_reference const_reference;
        typedef typename cbuf_type::difference_type difference_type;

        circular_buffer_iterator(cbuf_type *b, size_type p)
            : buf_(b), pos_(p) {}

        // Converting a non-const iterator to a const iterator
        circular_buffer_iterator
           (const circular_buffer_iterator<T_nonconst, T_nonconst,
                                           typename T_nonconst::value_type>
            &other)
            : buf_(other.buf_), pos_(other.pos_) {}
        friend class circular_buffer_iterator<const T, T, const elem_type>;

        // Use compiler generated copy ctor, copy assignment operator and dtor

        elem_type &operator*()  { return (*buf_)[pos_]; }
        elem_type *operator->() { return &(operator*()); }

        self_type &operator++()
        {
            pos_ += 1;
            return *this;
        }
        self_type operator++(int)
        {
            self_type tmp(*this);
            ++(*this);
            return tmp;
        }

        self_type &operator--()
        {
            pos_ -= 1;
            return *this;
        }
        self_type operator--(int)
        {
            self_type tmp(*this);
            --(*this);
            return tmp;
        }

        self_type operator+(difference_type n) const
        {
            self_type tmp(*this);
            tmp.pos_ += n;
            return tmp;
        }
        self_type &operator+=(difference_type n)
        {
            pos_ += n;
            return *this;
        }

        self_type operator-(difference_type n) const
        {
            self_type tmp(*this);
            tmp.pos_ -= n;
            return tmp;
        }
        self_type &operator-=(difference_type n)
        {
            pos_ -= n;
            return *this;
        }

        difference_type operator-(const self_type &c) const
        {
            return pos_ - c.pos_;
        }

        bool operator==(const self_type &other) const
        {
            return pos_ == other.pos_ && buf_ == other.buf_;
        }
        bool operator!=(const self_type &other) const
        {
            return pos_ != other.pos_ && buf_ == other.buf_;
        }
        bool operator>(const self_type &other) const
        {
            return pos_ > other.pos_;
        }
        bool operator>=(const self_type &other) const
        {
            return pos_ >= other.pos_;
        }
        bool operator<(const self_type &other) const
        {
            return pos_ < other.pos_;
        }
        bool operator<=(const self_type &other) const
        {
            return pos_ <= other.pos_;
        }

    private:

        cbuf_type *buf_;
        size_type  pos_;
};

template <typename circular_buffer_iterator_t>
circular_buffer_iterator_t operator+
    (const typename circular_buffer_iterator_t::difference_type &a,
     const circular_buffer_iterator_t                           &b)
{
    return circular_buffer_iterator_t(a) + b;
}

template <typename circular_buffer_iterator_t>
circular_buffer_iterator_t operator-
    (const typename circular_buffer_iterator_t::difference_type &a,
     const circular_buffer_iterator_t                           &b)
{
    return circular_buffer_iterator_t(a) - b;
}


/******************************************************************************
 * circular_buffer
 *****************************************************************************/

/**
 * This class provides a circular buffer in the STL style.
 *
 * You can add data to the end using the @ref push_back function, read data
 * using @ref front() and remove data using @ref pop_front().
 *
 * The class also provides random access through the @ref operator[]()
 * function and its random access iterator. Subscripting the array with
 * an invalid (out of range) index number leads to undefined results, both
 * for reading and writing.
 *
 * This class template accepts three template parameters:
 *   <li> T                            The type of object contained
 *   <li> always_accept_data_when_full Determines the behaviour of
 *                                     @ref push_back when the buffer is full.
 *                                     Set to true new data is always added, the
 *                                     old "end" data is thrown away.
 *                                     Set to false, the new data is not added.
 *                                     No error is returned neither is an
 *                                     exception raised.
 *   <li> Alloc                        Allocator type to use (in line with other
 *                                     STL containers).
 *
 * @short   STL style circule buffer
 * @author  Pete Goodliffe
 * @version 1.00
 */
template <typename T,
          bool     always_accept_data_when_full = true,
          typename Alloc                        = std::allocator<T> >
class circular_buffer
{
    public:

        enum
        {
            version_major = 1,
            version_minor = 0
        };

        // Typedefs
        typedef circular_buffer<T, always_accept_data_when_full, Alloc>
                self_type;

        typedef Alloc                             allocator_type;

        typedef typename Alloc::value_type        value_type;
        typedef typename Alloc::pointer           pointer;
        typedef typename Alloc::const_pointer     const_pointer;
        typedef typename Alloc::reference         reference;
        typedef typename Alloc::const_reference   const_reference;

        typedef typename Alloc::size_type         size_type;
        typedef typename Alloc::difference_type   difference_type;

        typedef circular_buffer_iterator
                <self_type, self_type>
                iterator;
        typedef circular_buffer_iterator
                <const self_type, self_type, const value_type>
                const_iterator;
        typedef std::reverse_iterator<iterator>       reverse_iterator;
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

        // Lifetime
        enum { default_capacity = 100 };
        explicit circular_buffer(size_type capacity = default_capacity)
        : array_(alloc_.allocate(capacity)), array_size_(capacity),
          head_(1), tail_(0), contents_size_(0)
        {
        }
        circular_buffer(const circular_buffer &other)
        : array_(alloc_.allocate(other.array_size_)),
          array_size_(other.array_size_),
          head_(other.head_), tail_(other.tail_),
          contents_size_(other.contents_size_)
        {
            try
            {
                assign_into(other.begin(), other.end());
            }
            catch (...)
            {
                destroy_all_elements();
                alloc_.deallocate(array_, array_size_);
                throw;
            }
        }
        template <class InputIterator>
        circular_buffer(InputIterator from, InputIterator to)
        : array_(alloc_.allocate(1)), array_size_(1),
          head_(1), tail_(0), contents_size_(0)
        {
            circular_buffer tmp;
            tmp.assign_into_reserving(from, to);
            swap(tmp);
        }
        ~circular_buffer()
        {
            destroy_all_elements();
            alloc_.deallocate(array_, array_size_);
        }
        circular_buffer &operator=(const self_type &other)
        {
            circular_buffer tmp(other);
            swap(tmp);
            return *this;
        }
        void swap(circular_buffer &other)
        {
            std::swap(array_,         other.array_);
            std::swap(array_size_,    other.array_size_);
            std::swap(head_,          other.head_);
            std::swap(tail_,          other.tail_);
            std::swap(contents_size_, other.contents_size_);
        }
        allocator_type get_allocator() const { return alloc_; }

        // Iterators
        iterator         begin()       { return iterator(this, 0); }
        iterator         end()         { return iterator(this, size()); }

        const_iterator   begin() const { return const_iterator(this, 0); }
        const_iterator   end() const   { return const_iterator(this, size()); }

        reverse_iterator rbegin()      { return reverse_iterator(end()); }
        reverse_iterator rend()        { return reverse_iterator(begin()); }

        const_reverse_iterator rbegin() const
        {
            return const_reverse_iterator(end());
        }
        const_reverse_iterator rend() const
        {
            return const_reverse_iterator(begin());
        }

        // Size
        size_type size() const        { return contents_size_; }
        size_type capacity() const    { return array_size_; }
        bool      empty() const       { return !contents_size_; }
        size_type max_size() const
        {
            return alloc_.max_size();
        }
        void reserve(size_type new_size)
        {
            if (capacity() < new_size)
            {
                circular_buffer tmp(new_size);
                tmp.assign_into(begin(), end());
                swap(tmp);
            }
        }

        // Accessing
        reference       front()       {return array_[head_];}
        reference       back()        {return array_[tail_];}
        const_reference front() const {return array_[head_];}
        const_reference back() const  {return array_[tail_];}

        void push_back(const value_type &item)
        {
            size_type next = next_tail();
            if (contents_size_ == array_size_)
            {
                if (always_accept_data_when_full)
                {
                    array_[next] = item;
                    increment_head();
                }
            }
            else
            {
                alloc_.construct(array_ + next, item);
            }
            increment_tail();
        }
        void pop_front()
        {
            size_type destroy_pos = head_;
            increment_head();
            alloc_.destroy(array_ + destroy_pos);
        }
        void clear()
        {
            for (size_type n = 0; n < contents_size_; ++n)
            {
                alloc_.destroy(array_ + index_to_subscript(n));
            }
            head_ = 1;
            tail_ = contents_size_ = 0;
        }

        reference       operator[](size_type n)       {return at_unchecked(n);}
        const_reference operator[](size_type n) const {return at_unchecked(n);}

        reference       at(size_type n)               {return at_checked(n);}
        const_reference at(size_type n) const         {return at_checked(n);}

    private:

        reference at_unchecked(size_type index) const
        {
            return array_[index_to_subscript(index)];
        }

        reference at_checked(size_type index) const
        {
            if (index >= contents_size_)
            {
                throw std::out_of_range( "index out of range" );
            }
            return at_unchecked(index);
        }

        // Rounds an unbounded to an index into array_
        size_type normalise(size_type n) const { return n % array_size_; }

        // Converts external index to an array subscript
        size_type index_to_subscript(size_type index) const
        {
            return normalise(index + head_);
        }

        void increment_tail()
        {
            ++contents_size_;
            tail_ = next_tail();
        }

        size_type next_tail()
        {
            return (tail_+1 == array_size_) ? 0 : tail_+1;
        }

        void increment_head()
        {
            // precondition: !empty()
            ++head_;
            --contents_size_;
            if (head_ == array_size_) head_ = 0;
        }

        template <typename f_iter>
        void assign_into(f_iter from, f_iter to)
        {
            if (contents_size_) clear();
            while (from != to)
            {
                push_back(*from);
                ++from;
            }
        }

        template <typename f_iter>
        void assign_into_reserving(f_iter from, f_iter to)
        {
            if (contents_size_) clear();
            while (from != to)
            {
                if (contents_size_ == array_size_)
                {
                    reserve(static_cast<size_type>(array_size_ * 1.5));
                }
                push_back(*from);
                ++from;
            }
        }

        void destroy_all_elements()
        {
            for (size_type n = 0; n < contents_size_; ++n)
            {
                alloc_.destroy(array_ + index_to_subscript(n));
            }
        }

        allocator_type  alloc_;
        value_type     *array_;
        size_type       array_size_;
        size_type       head_;
        size_type       tail_;
        size_type       contents_size_;
};

template <typename T,
          bool consume_policy,
          typename Alloc>
bool operator==(const circular_buffer<T, consume_policy, Alloc> &a,
                const circular_buffer<T, consume_policy, Alloc> &b)
{
    return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin());
}

template <typename T,
          bool consume_policy,
          typename Alloc>
bool operator!=(const circular_buffer<T, consume_policy, Alloc> &a,
                const circular_buffer<T, consume_policy, Alloc> &b)
{
    return a.size() != b.size() || !std::equal(a.begin(), a.end(), b.begin());
}

template <typename T,
          bool consume_policy,
          typename Alloc>
bool operator<(const circular_buffer<T, consume_policy, Alloc> &a,
               const circular_buffer<T, consume_policy, Alloc> &b)
{
    return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
}

#endif
