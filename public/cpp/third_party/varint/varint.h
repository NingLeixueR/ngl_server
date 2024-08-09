#pragma once


namespace ngl
{
	struct varint_impl
	{
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

	const int32_t varint_impl::m_64maxpos = 10;
	const int32_t varint_impl::m_32maxpos = 5;
}