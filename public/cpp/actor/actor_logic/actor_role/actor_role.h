/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* ��Ŀ���ƣ�ngl_server
* ��Ŀ��ַ��https://github.com/NingLeixueR/ngl_server
* 
* ���ļ��� ngl_server ��Ŀ��һ���֣���ѭ MIT ��ԴЭ�鷢����
* �����԰���Э��涨����ʹ�á��޸ĺͷַ�����Ŀ��������ҵ��;��
* ���豣��ԭʼ��Ȩ�����������
* 
* �������μ���Ŀ��Ŀ¼�µ� LICENSE �ļ���
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#pragma once

#include "actor_create.h"
#include "actor_manage.h"
#include "events_logic.h"
#include "manage_curl.h"
#include "db_manage.h"
#include "ndbclient.h"
#include "nprotocol.h"
#include "attribute.h"
#include "autoitem.h"
#include "roleinfo.h"
#include "remakes.h"
#include "db_pool.h"
#include "db_data.h"
#include "ntimer.h"
#include "rolekv.h"
#include "drop.h"
#include "task.h"
#include "cmd.h"
#include "bag.h"
#include "net.h"
#include "db.h"

#include "example.pb.h"
#include "net.pb.h"

namespace ngl
{
	enum ecross
	{
		ecross_ordinary = 1,			// ����ת��
		ecross_cross_ordinary = 2,		// ���ת��
		ecross_cross_example = 3,		// ����ת��
		ecross_none = 4,				// ����ת��
	};

	struct prorechange
	{
		std::string m_orderid;
		int32_t m_rechargeid = 0;
		int64_t m_roleid = 0;

		dprotocol(prorechange, m_orderid, m_rechargeid, m_roleid)
	};

	class actor_role : 
		public actor
	{
	public:
		remakes		 m_remakes;
		roleinfo	 m_info;
		bag			 m_bag;
		task		 m_task;
		rolekv		 m_rolekv;
		attribute	 m_attribute;
		i32_serverid m_gatewayid;
		std::pair<pbexample::EPLAY_TYPE, i64_actorid> m_example;
		drop<actor_role> m_drop;
	public:
		actor_role(const actor_role&) = delete;
		actor_role& operator=(const actor_role&) = delete;

		actor_role(i16_area aarea, i32_actordataid aroleid, void* adata);

		virtual ~actor_role();

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid(int32_t adata);

		virtual void init(); 

		virtual void loaddb_finish(bool adbishave);

		static void nregister();

		void login_finish();

		virtual i32_serverid get_getwayserverid();

		//# ִ��handle֮�����
		virtual void handle_after(handle_pram&);

		virtual void erase_actor_before();

		i64_actorid roleid();

		drop<actor_role>& get_drop()
		{
			return m_drop;
		}

		//# ���ø��½�ɫ����
		void update_attribute(EnumModule amodule, attribute_value& avalue);

		void sync_data_client();

#pragma region forward //ת�����
		
		//# ����forward_type��ָ��ת������
		template <typename T>
		ecross forward_type(const T& adata)
		{
			return ecross_ordinary;
		}

		//# �����ת������
		ecross forward_type(const pbnet::PROBUFF_NET_CHAT& adata)
		{
			if (m_info.bantalk())
			{
				return ecross_none;
			}
			switch (adata.mchannelid())
			{
			case pbnet::ENUM_CHAT_CHANNEL::ENUM_CHAT_CHANNEL_ORDINARY:
				return ecross_ordinary;
			case pbnet::ENUM_CHAT_CHANNEL::ENUM_CHAT_CHANNEL_CROSS_ORDINARY:
				return ecross_cross_ordinary;
			case pbnet::ENUM_CHAT_CHANNEL::ENUM_CHAT_CHANNEL_NONE:
				return ecross_none;
			}
			return ecross_none;
		}

		//# ranklist��ת������
		ecross forward_type(const pbnet::PROBUFF_NET_RANKLIST& adata)
		{
			return adata.miscross() ? ecross_cross_ordinary : ecross_ordinary;
		}

		//# ��������С��Ϸƥ���ת������
		ecross forward_type(const pbexample::PROBUFF_EXAMPLE_PLAY_JOIN& adata)
		{
			if (m_example.second != 0)
			{
				return ecross_none;
			}
			return adata.mcross() == pbexample::ECROSS_CROSS_ORDINARY ? ecross_cross_ordinary : ecross_ordinary;
		}

		ecross forward_type(const pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE& adata)
		{
			return adata.mcross() == pbexample::ECROSS_CROSS_ORDINARY ? ecross_cross_ordinary : ecross_ordinary;
		}

		//# ����С��Ϸƥ���ת������
		ecross example_type()
		{
			if (m_example.second == nguid::make())
			{
				return ecross_none;
			}
			else
			{
				return ecross_cross_example;
			}
		}

		ecross forward_type(const pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER& adata)
		{
			return example_type();
		}

		//# ����forward_before��ָ��ת��ǰ�¼�
		template <typename T>
		bool forward_before(const T& adata)
		{
			return true;
		}

		//# ת��"��������"ǰ
		bool forward_before(const pbnet::PROBUFF_NET_FAMIL_CREATE& adata);

		//# ����dataid��ָ��ת��ģ���dataid
		template <typename T>
		int32_t forward_dataid(const T& adata)
		{
			return nguid::none_actordataid();
		}

		template <ENUM_ACTOR ACTOR, typename T>
		bool handle_forward(const message<T>& adata)
		{
			const T& lparm = adata.get_shared_data() == nullptr ? *adata.get_data() : *adata.get_shared_data();
			if (forward_before(lparm) == false)
			{
				return false;
			}
			std::shared_ptr<mforward<T>> pro(nullptr);
			pro = std::make_shared<mforward<T>>(id_guid(), lparm);
			i64_actorid lguid;
			switch (forward_type(lparm))
			{
			case ecross_ordinary:
				lguid = nguid::make(ACTOR, tab_self_area, forward_dataid(lparm));
				break;
			case ecross_cross_ordinary:
				lguid = nguid::make(ACTOR, tab_self_cros_area, forward_dataid(lparm));
				break; 
			case ecross_cross_example:
				lguid = m_example.second;
				break;
			default:
				return true;
			}
			actor::send_actor(lguid, id_guid(), pro);
			return true;
		}
#pragma endregion

		//# ���õ�½ʱ��
		void reset_logintime();

		//# ���Ը��ͻ���һ���ַ���
		void echo_msg(const char* amsg);

		int32_t rechange(std::string& aorderid, int32_t arechargeid, bool agm, bool areporting);

		//# ��¼����δ������ֵ
		void loginpay();

		void requestgm(const char* aurl, const std::string& aparm, const std::function<void(int32_t, http_parm&)>& acall);

		//# ���󴴽�����
		void createorder(std::string& aorder, int32_t arechargeid);

		//# �Ƿ��״γ�ֵ
		bool is_first_recharge(int32_t arechargeid);

		//# CMDЭ��
		using handle_cmd = cmd<actor_role, std::string, const std::shared_ptr<pack>&, actor_role*, const char*>;
		//# GMЭ��
		using handle_gm = cmd<actor_role, std::string, int, ngl::njson_read&>;

		//# ��ʱ��
		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<np_arg_null>&);
		bool handle(const message<mforward<np_gm>>& adata);
		bool handle(const message<mforward<np_operator_task>>& adata);
		bool handle(const message<np_actor_disconnect_close>& adata);
		bool handle(const message<np_actor_senditem>& adata);
		bool handle(const message<np_eevents_logic_rolelogin>& adata);
		bool handle(const message<np_example_actorid>& adata);
		// [����]��ȡ������Ϣ
		bool handle(const message<pbnet::PROBUFF_NET_BAG_SYNC>& adata);
		// �޸ķ���,�ٶ�
		bool handle(const message<pbnet::PROBUFF_NET_CHANGE_ANGLE>& adata);
		// [����]cmdЭ��
		bool handle(const message<pbnet::PROBUFF_NET_CMD>& adata);
		// [����]������ʱ��
		bool handle(const message<pbnet::PROBUFF_NET_GET_TIME>& adata);
		// [����]��ֵ
		bool handle(const message<pbnet::PROBUFF_NET_RECHARGE>& adata);
		// [����]��ȡ��ɫ��Ϣ
		bool handle(const message<pbnet::PROBUFF_NET_ROLE_SYNC>& adata);
		// [����]�л���·
		bool handle(const message<pbnet::PROBUFF_NET_SWITCH_LINE>& adata);
		// [����]��ȡ������
		bool handle(const message<pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD>& adata);
	};
}//namespace ngl

