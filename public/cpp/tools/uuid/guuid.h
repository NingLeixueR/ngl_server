#pragma once

#include <cstdint>
#include <cstdio>
#include <string>

#if defined(_WIN32)
#include <objbase.h>
#else
#include <uuid/uuid.h>
#endif

#define GUID_LEN 64

namespace ngl
{
    class guuid
    {
    public:
        static bool make(std::string& astr)
        {
#if defined(_WIN32)
            char buf[GUID_LEN] = { 0 };
            GUID guid;

            if (CoCreateGuid(&guid))
            {
                return false;
            }

            sprintf(buf,
                "%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X",
                guid.Data1, guid.Data2, guid.Data3,
                guid.Data4[0], guid.Data4[1], guid.Data4[2],
                guid.Data4[3], guid.Data4[4], guid.Data4[5],
                guid.Data4[6], guid.Data4[7]);

            astr = buf;
            return true;
#else
            uuid_t uuid;
            uuid_generate(uuid);

            char str[37];
            uuid_unparse(uuid, str);

            astr = str;
            return true;
#endif
        }
    };
}// namespace ngl