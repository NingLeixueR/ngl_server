#pragma once

#include "handle_pram.h"
#include "nguid.h"
#include "type.h"
#include "impl.h"

#include <map>

namespace ngl
{
	// # (actor_client/actor_server) 用来管理actor地址
	// # 只允许(actor_client/actor_server) 调用
	class naddress
	{
		naddress() = delete;
		naddress(const naddress&) = delete;
		naddress& operator=(const naddress&) = delete;

	public:
		//# [ACTOR_ID] -> SERVER_ID
		//# [ACTOR_TYPE + EARE_ID + DATA_ID] -> SERVER_ID
		using map_guidserver		= std::map<nguid, i32_serverid>;
		//# [ACTOR_TYPE]  -> set<ACTOR_ID>
		//# [ACTOR_TYPE]  -> set<ACTOR_TYPE + EARE_ID + DATA_ID>
		using map_typeguid			= std::map<i16_actortype, std::set<nguid>>;
		//# [SERVER_ID] -> [SESSION_ID]
		using map_servernode		= std::map<i32_serverid, actor_node_session>;
		//# [ACTOR_ID] -> [GATEWAY_SERVER_ID]
		using map_rolegateway		= std::map<nguid, i32_serverid>;
		//# [ergodic]回调
		using ergodic_callbackfun	= std::function<bool(const map_guidserver&, const map_servernode&)>;
		//# [foreach]回调
		using foreach_callbackfun	= std::function<bool(const actor_node_session&)>;

		static naddress::map_guidserver		m_actorserver;
		static naddress::map_typeguid		m_actortypeserver;
		static naddress::map_servernode		m_session;
		static naddress::map_rolegateway	m_rolegateway;
		static std::map<i64_actorid, std::map<int64_t, std::function<void()>>> m_nguidfun;
	public:
#pragma region base
		//# debug 打印
		static void print_address();

		//# 设置结点
		static bool set_node(const nactornode& anode);

		//# 添加actor
		static void add_actor_address(i32_serverid aserverid, i64_actorid adataid);

		//# 添加一组actor
		static void add_actor_address(i32_serverid aserverid, const std::vector<i64_actorid>& avec);

		//# 删除actor
		static void del_actor_address(i64_actorid adataid);

		//# 删除一组actor
		static void del_actor_address(const std::vector<i64_actorid>& avec);

		//# 设置session
		static void set_session(i32_serverid aserverid, i32_sessionid asession);

		//# 获取session
		static i32_sessionid get_session(i32_serverid aserverid);

		//# 获取server id
		static i32_serverid get_server(const nguid& aguid);

		// 如果[get_server]获取失败，暂时保存发送列表等收到了发送
		static void server_actor_send(const nguid& aguid, const std::function<void()>& afun);

		//# 获取ENUM_ACTOR 对应的服务器
		static void get_serverlist(ENUM_ACTOR atype, std::set<i32_serverid>& avec);
#pragma endregion

#pragma region gateway
		//# [ACTOR_ID(ACTOR_ROLE的actor)] -> [GATEWAY_SERVER_ID]
		static i32_serverid gatewayid(const nguid& aguid);

		//# 将actor_role.actorid与gatewayid关联
		static void add_gatewayid(const nguid& aguid, i32_serverid aserverid);

		//# 解除actor_role.actorid与gatewayid的关联
		static void remove_gatewayid(const nguid& aguid);

		//# 获取一组actor_role.actorid与gatewayid的关联
		static void gatewayid(
			const std::set<nguid>& aactorset, std::set<i32_serverid>& aserverset
		);
#pragma endregion
		//# 遍历[std::function<bool(const actor_node_session&)>] 
		//# 如果返回值为false
		//# 则终止遍历
		static void foreach(const foreach_callbackfun& afun);

		//# [ergodic_callbackfun:std::function<bool(map_guidserver&, map_servernode&)>]
		static void ergodic(const ergodic_callbackfun& afun);

		//# 获取guid与server id对应关系
		static map_guidserver& get_actorserver_map();

		//# 根据area与dataid获取session
		static i32_sessionid sessionbyrole(i16_area aarea, i32_actordataid aroleid);

		//# 转发
		static bool forward(handle_pram& apram);
	};
}//namespace ngl