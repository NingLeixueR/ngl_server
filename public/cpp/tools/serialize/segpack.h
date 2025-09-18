#pragma once

#include "threadtools.h"
#include "segpack.h"
#include "type.h"
#include "pack.h"

#include <functional>
#include <map>

namespace ngl
{
	struct segpack_rate_interval
	{
		int m_resetutc;		// 上次重置的utc
		int m_ratecount;	// 收到数据包的数量

		segpack_rate_interval();
		void reset();
	};

	class segpack_rate
	{
		std::map<i32_socket, segpack_rate_interval> m_data;
	public:
		bool add(i32_socket aid);
	};

	class segpack_heartbeat
	{
	public:
		static bool is_heartbeat(i32_protocolnum aprotocolnum);
	};

	class segpack
	{
		std::map<i32_socket, std::shared_ptr<pack>> m_data; //残包暂时保管
		bpool			m_pool;
		segpack_rate	m_rate;
		segpack*		m_segpack;

	public:
		enum class edopush
		{
			e_continue,		// 继续循环
			e_break,		// 跳出循环
			e_error,		// 出错
		};

		segpack();

		bool push(i32_socket aid, const char* ap, int alen, bool aislanip);

		void close(i32_socket aid);

	private:
		void erase(i32_socket aid);

		std::shared_ptr<pack> remnant_package(i32_socket aid);

		edopush telnet_cmd(std::shared_ptr<pack>& apack, const char*& ap, int& alen, bool aislanip);

		bool check_recv(std::shared_ptr<pack>& apack, i32_socket aid, int alen, bool aislanip);

		bool is_heartbeat(std::shared_ptr<pack>& apack);

		bool fill_pack(std::shared_ptr<pack>& apack, const char*& ap, int& alen, int& len);

		edopush do_push(i32_socket aid, const char*& ap, int& alen, bool aislanip);
	};
}// namespace ngl