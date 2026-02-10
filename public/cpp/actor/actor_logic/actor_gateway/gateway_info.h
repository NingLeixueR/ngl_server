/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
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

		std::map<i16_area, std::map<i32_actordataid, gateway_socket>>	m_info;		// key:area  value.key:roleid
		std::map<i32_socket, gateway_socket*>							m_sockinfo;	// key:socket
	public:
		gateway_info() = default;

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

		// # 根据actorid获取gatewayid
		int64_t gatewayid(i64_actorid aid);

		// # 遍历所有socket
		void foreach(const std::function<void(gateway_socket*)>& afun);

		// # 数据更新
		void updata(const np_actor_gatewayinfo_updata& adata);
	};
}//namespace ngl