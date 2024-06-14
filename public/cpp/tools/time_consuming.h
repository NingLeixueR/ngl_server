#pragma once

#include "time_wheel.h"
#include "nlog.h"

namespace ngl
{
	struct tconsuming
	{
		int32_t			m_id;
		std::string		m_name;
		int64_t			m_beg;
		int64_t			m_end;
		std::function<bool(int64_t, int64_t)> m_fun;
		static std::atomic<int32_t> m_indexid;

		tconsuming(
			const std::string& aname, 
			const std::function<bool(int64_t, int64_t)>& afun
		);
	};

	class tconsuming_thread
	{
		tconsuming_thread(const tconsuming_thread&) = delete;
		tconsuming_thread& operator=(const tconsuming_thread&) = delete;

		using ptr_tconsuming = std::shared_ptr<tconsuming>;

		ngl::thread					m_thread;
		std::shared_mutex			m_mutex;
		std::list<ptr_tconsuming>	m_list;
		std::map<int32_t,int64_t>	m_remove;

		tconsuming_thread();
	public:
		static tconsuming_thread& getInstance()
		{
			static tconsuming_thread ltemp;
			return ltemp;
		}

		int32_t add(
			const std::string& aname, 
			const std::function<bool(int64_t, int64_t)>& afun
		);

		void remove(int32_t aid);

		void printf_log(bool awarn, const char* aname, tconsuming* aitem);

		void run();
	};

	class time_consuming
	{
		time_consuming() = delete;
		time_consuming(const time_consuming&) = delete;
		time_consuming& operator=(const time_consuming&) = delete;

		int32_t m_id;
	public:
		time_consuming(
			const std::string& aname, 
			const std::function<bool(int64_t, int64_t)>& afun
		);
		~time_consuming();
	};
}