#include "interpreter.hpp"
#include "numbers.hpp"
#include "token.hpp"
#include "value.hpp"
#include <sstream>

namespace laskin
{
    namespace internal
    {
        void initialize_bool(interpreter*);
        void initialize_io(interpreter*);
        void initialize_list(interpreter*);
        void initialize_numbers(interpreter*);
        void initialize_script(interpreter*);
        void initialize_stack(interpreter*);
        void initialize_string(interpreter*);
        void initialize_trigonometry(interpreter*);
    }

    interpreter::interpreter() {}

    interpreter::interpreter(const interpreter& that)
        : m_functions(that.m_functions) {}

    void interpreter::initialize()
    {
        using namespace internal;

        initialize_bool(this);
        initialize_io(this);
        initialize_list(this);
        initialize_numbers(this);
        initialize_script(this);
        initialize_stack(this);
        initialize_string(this);
        initialize_trigonometry(this);
    }

    void interpreter::execute(const std::string& word,
                              stack<value>& data,
                              hashmap<value>& locals,
                              std::istream& in,
                              std::ostream& out)
        throw(error)
    {
        const function_map::entry* e = m_functions.find(word);

        if (e)
        {
            for (auto& function : e->value)
            {
                if (function.signature().test(data))
                {
                    function.invoke(*this, data, locals, in, out);
                    return;
                }
            }

            throw error(
                    error::type_type,
                    "signature of function `"
                    + word
                    + "' does not match with current stack"
            );
        }

        throw error(
                error::type_reference,
                "undefined word: `"
                + word
                + "'"
        );
    }

    void interpreter::register_function(const std::string& name,
                                        const class signature& signature,
                                        function::callback callback)
    {
        function_map::entry* e = m_functions.find(name);

        if (e)
        {
            for (auto& f : e->value)
            {
                if (signature == f.signature())
                {
                    f.assign(signature, callback);
                    return;
                }
            }
            e->value.push_back(function(signature, callback));
        } else {
            std::vector<function> vector;

            vector.push_back(function(signature, callback));
            m_functions.insert(name, vector);
        }
    }

    void interpreter::register_function(const std::string& name,
                                        const std::string& sig,
                                        function::callback callback)
    {
        register_function(name, signature::parse(sig), callback);
    }

    void interpreter::register_function(const std::string& name,
                                        const class signature& signature,
                                        const class script& script)
    {
        function_map::entry* e = m_functions.find(name);

        if (e)
        {
            for (auto& f : e->value)
            {
                if (signature == f.signature())
                {
                    f.assign(signature, script);
                    return;
                }
            }
            e->value.push_back(function(signature, script));
        } else {
            std::vector<function> vector;

            vector.push_back(function(signature, script));
            m_functions.insert(name, vector);
        }
    }

    interpreter& interpreter::assign(const interpreter& that)
    {
        m_functions = that.m_functions;

        return *this;
    }
}
