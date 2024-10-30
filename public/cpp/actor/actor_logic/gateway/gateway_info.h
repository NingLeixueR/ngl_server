#pragma once

#include "nprotocol.h"
#include "nguid.h"

#include <map>

namespace ngl
{
	class gateway_info
	{
		gateway_info(const gateway_info&) = delete;
		gateway_info& operator=(const gateway_info&) = delete;

		// key area  value key roleid
		std::map<i16_area, std::map<i32_actordataid, gateway_socket>> m_info;
		// key socket
		std::map<i32_socket, gateway_socket*> m_sockinfo;
	public:
		gateway_info() {}

		void updata(const gateway_socket& ainfo);

		bool updata_socket(i16_area aarea, i32_actordataid aactordataid, i32_socket asocket);

		std::map<i16_area, std::map<i32_actordataid, gateway_socket>>& info();

		// # 只是断开连接
		void remove_socket(i32_socket asocket);

		// # 删除连接信息
		void remove_actorid(i64_actorid aactorid);

		// # 根据area和dataid获取gateway_socket
		gateway_socket* get(i16_area aarea, i32_actordataid aroleid);

		// # 根据socket获取gateway_socket
		gateway_socket* get(i32_socket asocket);

		void get_subscribe_to(std::map<i64_actorid, i32_serverid>& m_subscribe_to);

		// # 根据actorid获取gatewayid
		int64_t get_gatewayid(i64_actorid aid);

		void foreach(const std::function<void(gateway_socket*)>& afun);

		void gatewayinfo_updata(const np_actor_gatewayinfo_updata& adata);
	};
}//namespace ngl