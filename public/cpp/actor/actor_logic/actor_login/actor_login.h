#pragma once

#include "actor_manage.h"
#include "db_manage.h"
#include "ndbclient.h"
#include "account.h"
#include "db_data.h"
#include "db_pool.h"
#include "net.pb.h"
#include "db.pb.h"
#include "net.h"
#include "db.h"

#include <format>

namespace ngl
{
	struct pair_account
	{
		i32_serverid	m_gameserverid;
		i32_serverid	m_gatewayserverid;
		std::string		m_session;
	};
	struct server_info
	{
		int32_t	m_id = 0;
		int32_t	m_rolesize = 0;
	};

	class actor_login : public actor
	{
	public:
	private:
		// ----- Data Begin -----
		account								m_account;
		std::map<i32_serverid, server_info> m_game;
		std::map<i32_serverid, server_info> m_gateway;
		i16_area							m_config_area;
		std::map<i64_actorid, pair_account> m_actorbyserver;
		// ----- Data End   -----

		actor_login(const actor_login&) = delete;
		actor_login& operator=(const actor_login&) = delete;

		actor_login();
	public:
		friend class actor_instance<actor_login>;
		static actor_login& getInstance() 
		{ 
			return actor_instance<actor_login>::instance(); 
		}

		virtual ~actor_login() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid();

		virtual void init();

		virtual void loaddb_finish(bool adbishave);

		static void nregister();

		// # �����˺������ȡpbdb::db_account
		data_modified<pbdb::db_account>* get_account(int area, const std::string& account, const std::string& apassworld, bool& aiscreate);

		// # ��ȡamap����Կ��еķ�����
		bool get_freeserver(std::map<i32_serverid, server_info>& amap, std::pair<i32_serverid, int32_t>& apair);

		// # ��ȡgame����������Կ��еķ�����
		bool get_freeserver_game(std::pair<i32_serverid, int32_t>& apair);

		// # ��ȡgateway����������Կ��еķ�����
		bool get_freeserver_gateway(std::pair<int32_t, int32_t>& apair);

		// # ����amap��aserverid��Ӧ�������ĳ�������
		bool dec_freeserver(std::map<i32_serverid, server_info>& amap, i32_serverid aserverid);

		// # ����game��������aserverid��Ӧ�������ĳ�������
		bool dec_freeserver_game(i32_serverid aserverid);

		// # ����gateway��������aserverid��Ӧ�������ĳ�������
		bool dec_freeserver_gateway(i32_serverid aserverid);

		// # ��ӡ���з���������
		void printf_freeserver();

		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<np_arg_null>&);
		bool handle(const message<np_actor_disconnect_close>& adata);
		bool handle(const message<np_actorserver_connect>& adata);
		// [����]��¼�˺�
		bool handle(const message<pbnet::PROBUFF_NET_ACOUNT_LOGIN>& adata);
	};
}//namespace ngl

template <>
struct std::formatter<ngl::server_info>
{
	constexpr auto parse(std::format_parse_context& ctx)
	{
		return ctx.begin();
	}

	auto format(const ngl::server_info& aval, std::format_context& ctx)const
	{
		return std::format_to(ctx.out(), "[server_info:id{}-rolesize{}]", aval.m_id, aval.m_rolesize);
	}
};

