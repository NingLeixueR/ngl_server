#pragma once

#include "handle_pram.h"
#include "nguid.h"
#include "type.h"
#include "impl.h"

#include <map>

namespace ngl
{
	// # (actor_client/actor_server) ��������actor��ַ
	// # ֻ����(actor_client/actor_server) ����
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
		//# [ergodic]�ص�
		using ergodic_callbackfun	= std::function<bool(const map_guidserver&, const map_servernode&)>;
		//# [foreach]�ص�
		using foreach_callbackfun	= std::function<bool(const actor_node_session&)>;

		static naddress::map_guidserver		m_actorserver;
		static naddress::map_typeguid		m_actortypeserver;
		static naddress::map_servernode		m_session;
		static naddress::map_rolegateway	m_rolegateway;
		static std::map<i64_actorid, std::map<int64_t, std::function<void()>>> m_nguidfun;
	public:
#pragma region base
		//# debug ��ӡ
		static void print_address();

		//# ���ý��
		static bool set_node(const nactornode& anode);

		//# ���actor
		static void add_actor_address(i32_serverid aserverid, i64_actorid adataid);

		//# ���һ��actor
		static void add_actor_address(i32_serverid aserverid, const std::vector<i64_actorid>& avec);

		//# ɾ��actor
		static void del_actor_address(i64_actorid adataid);

		//# ɾ��һ��actor
		static void del_actor_address(const std::vector<i64_actorid>& avec);

		//# ����session
		static void set_session(i32_serverid aserverid, i32_sessionid asession);

		//# ��ȡsession
		static i32_sessionid get_session(i32_serverid aserverid);

		//# ��ȡserver id
		static i32_serverid get_server(const nguid& aguid);

		// ���[get_server]��ȡʧ�ܣ���ʱ���淢���б���յ��˷���
		static void server_actor_send(const nguid& aguid, const std::function<void()>& afun);

		//# ��ȡENUM_ACTOR ��Ӧ�ķ�����
		static void get_serverlist(ENUM_ACTOR atype, std::set<i32_serverid>& avec);
#pragma endregion

#pragma region gateway
		//# [ACTOR_ID(ACTOR_ROLE��actor)] -> [GATEWAY_SERVER_ID]
		static i32_serverid gatewayid(const nguid& aguid);

		//# ��actor_role.actorid��gatewayid����
		static void add_gatewayid(const nguid& aguid, i32_serverid aserverid);

		//# ���actor_role.actorid��gatewayid�Ĺ���
		static void remove_gatewayid(const nguid& aguid);

		//# ��ȡһ��actor_role.actorid��gatewayid�Ĺ���
		static void gatewayid(
			const std::set<nguid>& aactorset, std::set<i32_serverid>& aserverset
		);
#pragma endregion
		//# ����[std::function<bool(const actor_node_session&)>] 
		//# �������ֵΪfalse
		//# ����ֹ����
		static void foreach(const foreach_callbackfun& afun);

		//# [ergodic_callbackfun:std::function<bool(map_guidserver&, map_servernode&)>]
		static void ergodic(const ergodic_callbackfun& afun);

		//# ��ȡguid��server id��Ӧ��ϵ
		static map_guidserver& get_actorserver_map();

		//# ����area��dataid��ȡsession
		static i32_sessionid sessionbyrole(i16_area aarea, i32_actordataid aroleid);

		//# ת��
		static bool forward(handle_pram& apram);
	};
}//namespace ngl