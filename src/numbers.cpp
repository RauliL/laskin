#include "numbers.hpp"
#include <cmath>

namespace laskin
{
    static const char digitmap[] = "0123456789abcdefghijklmnopqrstuvwxyz";

    std::string int_to_string(integer number, int radix)
    {
        if (radix < 2 || radix > 36)
        {
            radix = 10;
        }
        if (number != 0)
        {
            std::string result;
            const bool negative = number < 0;
            uinteger mag = static_cast<uinteger>(negative ? -number : number);

            result.reserve(21);
            do
            {
                result.insert(0, 1, digitmap[mag % radix]);
                mag /= radix;
            }
            while (mag != 0);
            if (negative)
            {
                result.insert(0, 1, '-');
            }

            return result;
        }

        return "0";
    }

    std::string real_to_string(real number)
    {
        if (std::isinf(number))
        {
            return number < 0.0 ? "-inf" : "inf";
        }
        else if (std::isnan(number))
        {
            return "nan";
        } else {
            char buffer[20];

            std::snprintf(buffer, sizeof(buffer), "%g", number);

            return buffer;
        }
    }

    integer string_to_int(const std::string& s, int radix)
        throw(std::length_error)
    {
        std::string::const_iterator current = s.begin();
        const std::string::const_iterator end = s.end();
        uinteger number = 0;
        bool sign = true;

        // Eat whitespace.
        while (current < end && std::isspace(*current))
        {
            ++current;
        }

        // Get the sign.
        if (current < end)
        {
            if (*current == '-')
            {
                sign = false;
                ++current;
            }
            else if (*current == '+')
            {
                ++current;
            }
        }

        // Try to decipher the prefix.
        if (radix <= 0 && current + 1< end && *current == '0')
        {
            ++current;
            switch (*current)
            {
                // Hexadecimal
                case 'x': case 'X':
                    radix = 16;
                    ++current;
                    break;

                // Binary
                case 'b': case 'B':
                    radix = 2;
                    ++current;
                    break;

                // Decimal
                case 'd': case 'D':
                    radix = 10;
                    ++current;
                    break;

                // Octal
                case 'o': case 'O':
                    ++current;

                // Default to octal:
                default:
                    radix = 8;
            }
        }

        if (radix < 2 || radix > 36)
        {
            radix = 10;
        }

        const uinteger div = LASKIN_INTEGER_MAX / radix;
        const uinteger rem = LASKIN_INTEGER_MAX % radix;

        while (current < end)
        {
            unsigned int digit;
            char c = *current++;

            if (c >= '0' && c < '0' + radix)
            {
                digit = c - '0';
            }
            else if (radix > 10)
            {
                if (c >= 'a' && c < 'a' + radix - 10)
                {
                    digit = c - 'a' + 10;
                }
                else if (c >= 'A' && c < 'A' + radix - 10)
                {
                    digit = c - 'A' + 10;
                } else {
                    continue;
                }
            } else {
                continue;
            }
            if (number > div || (number == div && digit > rem))
            {
                throw std::length_error("integer overflow");
            }
            number = (number * radix) + digit;
        }
        
        return sign ? number : -number;
    }

    real string_to_real(const std::string& s)
        throw(std::length_error)
    {
        std::string::const_iterator current = s.begin();
        const std::string::const_iterator end = s.end();
        double number = 0.0;
        bool sign;
        integer exponent = 0;
        bool has_digits = false;
        bool has_dot = false;

        // Eat whitespace.
        while (current < end && std::isspace(*current))
        {
            ++current;
        }

        // Get the sign.
        sign = current < end && *current == '-' ? false : true;
        if (!sign || (current < end && *current == '+'))
        {
            ++current;
        }

        for (; current < end; ++current)
        {
            if (std::isdigit(*current))
            {
                has_digits = true;
                if (number > LASKIN_REAL_MAX * 0.1)
                {
                    ++exponent;
                } else {
                    number = (number * 10.0) + (*current - '0');
                }
                if (has_dot)
                {
                    --exponent;
                }
            }
            else if (!has_dot && *current == '.')
            {
                has_dot = true;
            } else {
                break;
            }
        }

        if (!has_digits)
        {
            return 0.0;
        }

        // Parse exponent. (This is kinda shitty way to do it.)
        if (current < end && (*current == 'e' || *current == 'E'))
        {
            exponent = string_to_int(std::string(++current, end), 10);
        }
        if (number == 0.0)
        {
            return 0.0;
        }
        if (exponent < 0)
        {
            if (number < LASKIN_REAL_MIN * std::pow(10.0, static_cast<double>(-exponent)))
            {
                throw std::length_error("real underflow");
            }
        }
        else if (exponent > 0)
        {
            if (number > LASKIN_REAL_MAX * std::pow(10.0, static_cast<double>(exponent)))
            {
                throw std::length_error("real overflow");
            }
        }
        number *= std::pow(10.0, static_cast<double>(exponent));
        
        return sign ? number : -number;
    }
}
