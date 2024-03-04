#pragma once

#include <cctype>
#include <string>
#include <utility>

namespace ngl
{
    class base64
    {
    public:
        std::string base64_encode(std::uint8_t const* data, std::size_t len);

        std::string base64_encode(std::string const& s);

        std::string base64_decode(char const* data, std::size_t len);

        std::string base64_decode(std::string const& data);
    };
}