#pragma once

#include <list>

namespace ngl
{
	class malloc_buff
	{
		malloc_buff(const malloc_buff&) = delete;
		malloc_buff& operator=(const malloc_buff&) = delete;

		int m_buffcout;
		int m_buffsize;
	public:
		malloc_buff(int abuffcount, int abuffsize) :
			m_buffcout(abuffcount),
			m_buffsize(abuffsize)
		{}

		class ptr
		{
			malloc_buff* m_mb;
		public:
			char* m_buff;
			int32_t m_bufflen;

			ptr(malloc_buff* amb) :
				m_buff(nullptr),
				m_bufflen(0),
				m_mb(amb)
			{}

			ptr(malloc_buff* amb, char* abuff, int32_t abufflen) :
				m_buff(abuff),
				m_bufflen(abufflen),
				m_mb(amb)
			{}

			void free()
			{
				if (m_mb != nullptr && m_buff != nullptr && m_bufflen > 0)
				{
					m_mb->free(*this);
					m_buff = nullptr;
					m_bufflen = 0;
				}
			}

			~ptr()
			{
				if (m_mb != nullptr && m_buff != nullptr && m_bufflen > 0)
				{
					m_mb->free(*this);
				}
			}
		};

		template <typename T>
		int serialize(ptr& aptr, T& adata)
		{
			int lbyte = 0;
			if (malloc_function(aptr, [&adata,&lbyte](ptr& aptr)->bool
				{
					ngl::serialize lserialize(aptr.m_buff, aptr.m_bufflen);
					if (lserialize.push(adata))
					{
						lbyte = lserialize.byte();
						return true;
					}
					return false;
				}))
			{
				return lbyte;
			}
			Throw("malloc_buff.serialize fail");
		}

		bool malloc_function(ptr& aptr, const std::function<bool(ptr&)>& afun);

	private:
		bool malloc(ptr& aptr, int32_t apos);
		
		void free(ptr& aptr);
		
		std::list<std::pair<char*, int32_t>> m_bufflist;
	};
}// namespace ngl