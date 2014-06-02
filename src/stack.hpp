#ifndef LASKIN_STACK_HPP_GUARD
#define LASKIN_STACK_HPP_GUARD

#include <memory>

namespace laskin
{
    template <
        class T,
        class Allocator = std::allocator<T>
    >
    class stack
    {
    public:
        typedef T value_type;
        typedef Allocator allocator_type;
        typedef std::size_t size_type;
        typedef typename Allocator::reference reference;
        typedef typename Allocator::const_reference const_reference;
        typedef typename Allocator::pointer pointer;
        typedef typename Allocator::const_pointer const_pointer;

        /**
         * Constructs empty stack.
         */
        stack(const allocator_type& allocator = allocator_type())
            : m_allocator(allocator)
            , m_capacity(0)
            , m_size(0)
            , m_data(0) {}

        /**
         * Constructs copy of existing stack.
         */
        stack(const stack<T>& that)
            : m_allocator(that.m_allocator)
            , m_capacity(that.m_size)
            , m_size(m_capacity)
            , m_data(m_size ? m_allocator.allocate(m_size) : 0)
        {
            for (size_type i = 0; i < m_size; ++i)
            {
                m_allocator.construct(m_data + i, that.m_data[i]);
            }
        }

        /**
         * Destructor.
         */
        virtual ~stack()
        {
            for (size_type i = 0; i < m_size; ++i)
            {
                m_allocator.destroy(m_data + i);
            }
            if (m_data)
            {
                m_allocator.deallocate(m_data, m_capacity);
            }
        }

        /**
         * Returns true if the stack is empty.
         */
        inline bool empty() const
        {
            return !m_size;
        }

        /**
         * Returns the number of elements stored in the stack.
         */
        inline size_type size() const
        {
            return m_size;
        }

        inline reference front()
        {
            return m_data[0];
        }

        inline const_reference front() const
        {
            return m_data[0];
        }

        inline reference back()
        {
            return m_data[m_size - 1];
        }

        inline const_reference back() const
        {
            return m_data[m_size - 1];
        }

        inline reference at(size_type index)
        {
            return m_data[index];
        }

        inline const_reference at(size_type index) const
        {
            return m_data[index];
        }

        inline reference operator[](size_type index)
        {
            return m_data[index];
        }

        inline const_reference operator[](size_type index) const
        {
            return m_data[index];
        }

        void reserve(size_type n)
        {
            if (!n || m_capacity >= n)
            {
                return;
            }
            if (m_capacity + 1 == n)
            {
                n += 32;
            }
            if (m_size)
            {
                pointer old = m_data;

                m_data = m_allocator.allocate(n);
                for (size_type i = 0; i < m_size; ++i)
                {
                    m_allocator.construct(m_data + i, old[i]);
                    m_allocator.destroy(old + i);
                }
                m_allocator.deallocate(old, m_capacity);
            } else {
                if (m_data)
                {
                    m_allocator.deallocate(m_data, m_capacity);
                }
                m_data = m_allocator.allocate(n);
            }
            m_capacity = n;
        }

        /**
         * Inserts given value to the end of the stack.
         */
        void push(const_reference value)
        {
            reserve(m_size + 1);
            m_allocator.construct(m_data + m_size++, value);
        }

        inline stack& operator<<(const_reference value)
        {
            push(value);

            return *this;
        }

        /**
         * Removes element from end of the stack.
         */
        void pop()
        {
            m_allocator.destroy(m_data + --m_size);
        }

        inline stack& operator>>(reference value)
        {
            value = m_data[--m_size];
            m_allocator.destroy(m_data + m_size);

            return *this;
        }

        /**
         * Removes all elements from the stack.
         */
        void clear()
        {
            for (size_type i = 0; i < m_size; ++i)
            {
                m_allocator.destroy(m_data + i);
            }
            m_size = 0;
        }

        stack& assign(const stack<T>& that)
        {
            if (m_data != that.m_data)
            {
                for (size_type i = 0; i < m_size; ++i)
                {
                    m_allocator.destroy(m_data + i);
                }
                if (m_capacity < that.m_size)
                {
                    if (m_data)
                    {
                        m_allocator.deallocate(m_data, m_capacity);
                    }
                    m_data = m_allocator.allocate(m_capacity = that.m_size);
                }
                m_size = that.m_size;
                for (size_type i = 0; i < m_size; ++i)
                {
                    m_allocator.construct(m_data + i, that.m_data[i]);
                }
            }

            return *this;
        }

        /**
         * Assignment operator.
         */
        inline stack& operator=(const stack<T>& that)
        {
            return assign(that);
        }

    private:
        allocator_type m_allocator;
        size_type m_capacity;
        size_type m_size;
        pointer m_data;
    };
}

#endif /* !LASKIN_STACK_HPP_GUARD */
