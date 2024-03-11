#include "varint.h"
#include <vector>
#include "serialize.h"

namespace ngl
{
    struct varint_impl
    {
        static bool m_isopen;
        static const int32_t m_64maxpos/* = 10*/;
        static const int32_t m_32maxpos/* = 5*/;

        static uint64_t zigzag64encode(int64_t n)
        {
            return (uint64_t)((n >> 63) ^ (n << 1));
        }

        static int64_t zigzag64decode(uint64_t n)
        {
            return (uint64_t)((n >> 1) ^ -(n & 1));
        }

        static uint32_t zigzag32encode(int32_t n)
        {
            return (uint32_t)((n >> 31) ^ (n << 1));
        }

        static int32_t zigzag32decode(uint32_t n)
        {
            return (int32_t)((n >> 1) ^ -(n & 1));
        }
    };

    bool varint_impl::m_isopen;
    const int32_t varint_impl::m_64maxpos = 10;
    const int32_t varint_impl::m_32maxpos = 5;

    void varint::set(bool aisopen)
    {
        varint_impl::m_isopen = aisopen;
    }
    
    int	varint::length(parm_length<int64_t>& avaluearrays)
	{
        if (varint_impl::m_isopen)
        {
            uint64_t ln = varint_impl::zigzag64encode(avaluearrays.m_value);
            if ((ln & (0xffffffffffff << 7)) == 0)
                return 1;
            else if ((ln & (0xffffffffffff << 14)) == 0)
                return 2;
            else if ((ln & (0xffffffffffff << 21)) == 0)
                return 3;
            else if ((ln & (0xffffffffffff << 28)) == 0)
                return 4;
            else if ((ln & (0xffffffffffff << 35)) == 0)
                return 5;
            else if ((ln & (0xffffffffffff << 42)) == 0)
                return 6;
            else if ((ln & (0xffffffffffff << 49)) == 0)
                return 7;
            else if ((ln & (0xffffffffffff << 56)) == 0)
                return 8;
            else if ((ln & (0xffffffffffff << 63)) == 0)
                return 9;
            else
                return 10;
        }
        else
            return sizeof(int64_t);
	}

    int varint::length(parm_length<int32_t>& avaluearrays)
    {
        if (varint_impl::m_isopen)
        {
            uint64_t ln = varint_impl::zigzag64encode(avaluearrays.m_value);
            if ((ln & (0xffffffffffff << 7)) == 0)
                return 1;
            else if ((ln & (0xffffffffffff << 14)) == 0)
                return 2;
            else if ((ln & (0xffffffffffff << 21)) == 0)
                return 3;
            else if ((ln & (0xffffffffffff << 28)) == 0)
                return 4;
            else
                return 5;
        }
        else
            return sizeof(int32_t);
    }

    template <typename T>
    bool encodebasetype(char* buf, int len, const T* adata, int32_t adatalen, int32_t* bytes)
    {
        if (sizeof(T) > len)
            return false;
        memcpy(buf, adata, sizeof(T)* adatalen);
        *bytes += sizeof(T) * adatalen;
        return true;
    }

    bool varint::encode(parm<int64_t>& aparm)
    {
        if (varint_impl::m_isopen == false)
        {
            if (encodebasetype(aparm.m_buf, aparm.m_len, &aparm.m_value, 1, aparm.m_bytes) == false)
                return false;
            return true;
        }
        else
        {
            uint64_t ln = varint_impl::zigzag64encode(aparm.m_value);
            int32_t index = 0;
            while (true)
            {
                if (index > aparm.m_len || index > varint_impl::m_64maxpos)
                    return false;
                //0x7F 即　0111 1111
                //~ 表示取反  
                if ((ln & ~0x7F) == 0)
                {
                    aparm.m_buf[index] = (uint8_t)(ln & 0x7F);
                    break;
                }
                else
                {
                    //取出7位并在第8位加上标记1
                    aparm.m_buf[index] = (uint8_t)((ln & 0x7F) | 0x80);
                    index++;
                    //已经处理的舍去
                    ln = ln >> 7;
                }
            }
            *aparm.m_bytes += index + 1;
            return true;
        }
    }

    bool varint::encode(parm<int32_t>& aparm)
    {
        if (varint_impl::m_isopen == false)
        {
            if (encodebasetype(aparm.m_buf, aparm.m_len, &aparm.m_value, 1, aparm.m_bytes) == false)
                return false;
            return true;
        }
        else
        {
            uint32_t ln = varint_impl::zigzag32encode(aparm.m_value);
            int index = 0;
            while (true)
            {
                if (index > aparm.m_len || index > varint_impl::m_32maxpos)
                    return false;
                //0x7F 即　0111 1111
                //~ 表示取反  
                if ((ln & ~0x7F) == 0)
                {
                    aparm.m_buf[index] = (uint8_t)(ln & 0x7F);
                    break;
                }
                else
                {
                    //取出7位并在第8位加上标记1
                    aparm.m_buf[index] = (uint8_t)((ln & 0x7F) | 0x80);
                    index++;
                    //已经处理的舍去
                    ln = ln >> 7;
                }
            }
            *aparm.m_bytes += index + 1;
            return true;
        }
    }

    template <typename T>
    bool decodebasetype(const char* buf, int len, int32_t* bytes, T* adata, int32_t adatalen)
    {
        int llen = sizeof(T) * adatalen;
        if (llen > len)
            return false;
        memcpy(adata, buf, llen);
        *bytes += llen;
        return true;
    }

    bool varint::decode(parm<int64_t>& aparm)
    {
        if (varint_impl::m_isopen == false)
        {
            if (decodebasetype(aparm.m_buf, aparm.m_len, aparm.m_bytes, &aparm.m_value, 1) == false)
                return false;
            return true;
        }
        else
        {
            aparm.m_value = 0;
            int i = 0;
            for (; i < aparm.m_len; i++)
            {
                uint8_t b = aparm.m_buf[i];
                //0x7F 即　0111 1111
                //取出7位然后右移，因为是小端存储
                uint8_t c = (b & 0x7F);
                aparm.m_value |= (int64_t)c << (i * 7);
                // 0x80 即 1000 0000
                //第8位是0 说明后面没有字节了
                if ((uint8_t)(b & 0x80) == 0)
                {
                    break;
                }
                if (i > varint_impl::m_64maxpos)
                    break;
            }
            *aparm.m_bytes += i + 1;
            aparm.m_value = varint_impl::zigzag64decode(aparm.m_value);
            return true;
        }
    }

    bool varint::decode(parm<int32_t>& aparm)
    {
        if (varint_impl::m_isopen == false)
        {
            int32_t lbytes = 0;
            if (decodebasetype(aparm.m_buf, aparm.m_len, &lbytes, &aparm.m_value, 1) == false)
                return false;
            *aparm.m_bytes += lbytes;
            return true;
        }
        else
        {
            aparm.m_value = 0;
            int i = 0;
            for (; i < aparm.m_len; i++)
            {
                uint8_t b = aparm.m_buf[i];
                //0x7F 即　0111 1111
                //取出7位然后右移，因为是小端存储
                aparm.m_value |= (b & 0x7F) << (i * 7);
                // 0x80 即 1000 0000
                //第8位是0 说明后面没有字节了
                if ((uint8_t)(b & 0x80) == 0)
                {
                    break;
                }
                if (i > varint_impl::m_32maxpos)
                    break;
            }
            *aparm.m_bytes += i + 1;
            aparm.m_value = varint_impl::zigzag32decode(aparm.m_value);
            return true;
        }
    }
}