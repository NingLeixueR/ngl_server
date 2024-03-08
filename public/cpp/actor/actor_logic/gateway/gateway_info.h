#pragma once

#include <map>

#include "nguid.h"
#include "nprotocol.h"
#include "rebot_test.h"

namespace ngl
{
	class gateway_info
	{
		// key area  value key roleid
		std::map<i16_area, std::map<i32_actordataid, gateway_socket>> m_info;
		// key socket
		std::map<i32_socket, gateway_socket*> m_sockinfo;
	public:
		void updata(gateway_socket& ainfo);

		bool updata_socket(i16_area aarea, i32_actordataid aactordataid, i32_socket asocket);

		std::map<i16_area, std::map<i32_actordataid, gateway_socket>>& info();

		// ### 只是断开连接
		void remove_socket(i32_socket asocket);
		// ### 删除连接信息
		void remove_actorid(i64_actorid aactorid);

		gateway_socket* get(i16_area aarea, i32_actordataid aroleid);

		gateway_socket* get(i32_socket asocket);

		void get_subscribe_to(std::map<i64_actorid, i32_serverid>& m_subscribe_to);

		int64_t get_gatewayid(int64_t aid);

		void foreach(const std::function<void(gateway_socket*)>& afun);
	};
}