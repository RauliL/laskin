#ifndef LASKIN_HASHMAP_HPP_GUARD
#define LASKIN_HASHMAP_HPP_GUARD

#include <cstdlib>
#include <new>
#include <string>

namespace laskin
{
    inline std::size_t hash_string(const std::string& s)
    {
        std::size_t result = 0;

        for (auto c : s)
        {
            result += c;
            result += (result << 10);
            result ^= (result >> 6);
        }
        result += (result << 3);
        result ^= (result >> 11);
        result += (result << 15);

        return result;
    }

    /**
     * Simple hash map implementation which stores values identified by
     * strings.
     */
    template< class T >
    class hashmap
    {
    public:
        typedef std::size_t size_type;
        typedef std::string key_type;
        typedef T mapped_value;

        struct entry
        {
            key_type key;
            mapped_value value;
            size_type hash;
            entry* next;
            entry* prev;
            entry* child;
        };

        hashmap(size_type bucket_size = 8)
            : m_bucket_size(bucket_size)
            , m_bucket(new entry*[m_bucket_size])
            , m_front(NULL)
            , m_back(NULL) {}

        hashmap(const hashmap<T>& that)
            : m_bucket_size(that.m_bucket_size)
            , m_bucket(new entry*[m_bucket_size])
            , m_front(NULL)
            , m_back(NULL)
        {
            for (const entry* e1 = that.m_front; e1; e1 = e1->next)
            {
                entry* e2 = static_cast<entry*>(std::malloc(sizeof(entry)));
                const size_type index = static_cast<size_type>(e1->hash % m_bucket_size);

                new (static_cast<void*>(&e2->key)) key_type(e1->key);
                new (static_cast<void*>(&e2->value)) mapped_value(e1->value);
                e2->hash = e1->hash;
                e2->next = NULL;
                if ((e2->prev = m_back))
                {
                    m_back->next = e2;
                } else {
                    m_front = e2;
                }
                m_back = e2;
                e2->child = m_bucket[index];
                m_bucket[index] = e2;
            }
        }

        /**
         * Destructor.
         */
        virtual ~hashmap()
        {
            entry* current = m_front;
            entry* next;

            while (current)
            {
                next = current->next;
                current->key.~key_type();
                current->value.~mapped_value();
                std::free(current);
                current = next;
            }
            delete[] m_bucket;
        }

        /**
         * Returns <code>true</code> if the hash map is empty.
         */
        inline bool empty() const
        {
            return !m_front;
        }

        /**
         * Removes all entries from the hash map.
         */
        void clear()
        {
            entry* current = m_front;
            entry* next;

            while (current)
            {
                next = current->next;
                current->key.~key_type();
                current->value.~mapped_value();
                std::free(current);
                current = next;
            }
            for (size_type i = 0; i < m_bucket_size; ++i)
            {
                m_bucket[i] = NULL;
            }
            m_front = m_back = NULL;
        }

        entry* find(const key_type& key)
        {
            const size_type hash = hash_string(key);

            for (entry* e = m_bucket[static_cast<size_type>(hash % m_bucket_size)]; e; e = e->child)
            {
                if (e->hash == hash)
                {
                    return e;
                }
            }

            return NULL;
        }

        const entry* find(const key_type& key) const
        {
            const size_type hash = hash_string(key);

            for (entry* e = m_bucket[static_cast<size_type>(hash % m_bucket_size)]; e; e = e->child)
            {
                if (e->hash == hash)
                {
                    return e;
                }
            }

            return NULL;
        }

        void insert(const key_type& key, const mapped_value& value)
        {
            const size_type hash = hash_string(key);
            const size_type index = static_cast<size_type>(hash % m_bucket_size);
            entry* e;

            for (e = m_bucket[index]; e; e = e->child)
            {
                if (e->hash == hash)
                {
                    e->key = key;
                    e->value = value;
                    return;
                }
            }
            e = static_cast<entry*>(std::malloc(sizeof(entry)));
            new (static_cast<void*>(&e->key)) key_type(key);
            new (static_cast<void*>(&e->value)) mapped_value(value);
            e->hash = hash;
            e->next = NULL;
            if ((e->prev = m_back))
            {
                m_back->next = e;
            } else {
                m_front = e;
            }
            m_back = e;
            e->child = m_bucket[index];
            m_bucket[index] = e;
        }

        hashmap& assign(const hashmap<T>& that)
        {
            clear();
            for (const entry* e1 = that.m_front; e1; e1 = e1->next)
            {
                entry* e2 = static_cast<entry*>(std::malloc(sizeof(entry)));
                const size_type index = static_cast<size_type>(e1->hash % m_bucket_size);

                new (static_cast<void*>(&e2->key)) key_type(e1->key);
                new (static_cast<void*>(&e2->value)) mapped_value(e1->value);
                e2->hash = e1->hash;
                e2->next = NULL;
                if ((e2->prev = m_back))
                {
                    m_back->next = e2;
                } else {
                    m_front = e2;
                }
                m_back = e2;
                e2->child = m_bucket[index];
                m_bucket[index] = e2;
            }

            return *this;
        }

        /**
         * Assignment operator.
         */
        inline hashmap& operator=(const hashmap<T>& that)
        {
            return assign(that);
        }

    private:
        /** Size of the bucket. */
        const size_type m_bucket_size;
        /** The actual hash table. */
        entry** m_bucket;
        /** Pointer to the first entry in the map. */
        entry* m_front;
        /** Pointer to the last entry in the map. */
        entry* m_back;
    };
}

#endif /* !LASKIN_HASHMAP_HPP_GUARD */
