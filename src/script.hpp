#ifndef LASKIN_SCRIPT_HPP_GUARD
#define LASKIN_SCRIPT_HPP_GUARD

#include "error.hpp"
#include "hashmap.hpp"
#include "token.hpp"
#include <vector>

namespace laskin
{
    class script
    {
    public:
        typedef std::vector<token>::iterator iterator;
        typedef std::vector<token>::const_iterator const_iterator;

        explicit script(const std::vector<token>& tokens = std::vector<token>());

        script(const_iterator& first, const_iterator& last);

        script(const script& that);

        static script scan(std::istream& is)
            throw(error);

        /**
         * Returns true if the script has no tokens.
         */
        inline bool empty() const
        {
            return m_tokens.empty();
        }

        inline const_iterator begin() const
        {
            return m_tokens.begin();
        }

        inline const_iterator end() const
        {
            return m_tokens.end();
        }

        void execute(class interpreter& interpreter,
                     stack<value>& data,
                     hashmap<value>& locals,
                     std::istream& in,
                     std::ostream& out) const
            throw(error);

        script& assign(const script& that);
        script& assign(const_iterator& first, const_iterator& last);

        /**
         * Assignment operator.
         */
        inline script& operator=(const script& that)
        {
            return assign(that);
        }

    private:
        std::vector<token> m_tokens;
    };
}

#endif /* !LASKIN_SCRIPT_HPP_GUARD */
