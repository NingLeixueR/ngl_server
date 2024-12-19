#pragma once

#include "actor_create.h"
#include "actor_manage.h"
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
#include "net.pb.h"
#include "rolekv.h"
#include "task.h"
#include "cmd.h"
#include "bag.h"
#include "net.h"
#include "db.h"

namespace ngl
{
	class actor_manage_role;
	class actor_role : public actor
	{
	public:
		remakes		 m_remakes;
		roleinfo	 m_info;
		bag			 m_bag;
		task		 m_task;
		rolekv		 m_rolekv;
		attribute	 m_attribute;
		i32_serverid m_gatewayid;
		i64_actorid  m_playactorid;
	public:
		actor_role(const actor_role&) = delete;
		actor_role& operator=(const actor_role&) = delete;

		actor_role(i16_area aarea, i32_actordataid aroleid, void* adata);

		virtual i32_serverid get_getwayserverid();

		virtual void init();

		static void nregister();

		virtual ~actor_role();

		virtual void loaddb_finish(bool adbishave);

		//# ִ��handle֮�����
		virtual void handle_after();

		virtual void erase_actor_before();

		i64_actorid roleid();

		//# ���ø��½�ɫ����
		void update_attribute(EnumModule amodule, attribute_value& avalue);

		void sync_data_client();

#pragma region forward //ת�����
		enum ecross
		{
			ecross_ordinary			= 1,			// ����ת��
			ecross_cross_ordinary	= 2,			// ���ת��
			ecross_none				= 3,			// ����ת��
		};

		//# ����forward_type��ָ��ת������
		template <typename T>
		ecross forward_type(const T& adata)
		{
			return ecross_ordinary;
		}

		//# �����ת������
		ecross forward_type(const pbnet::PROBUFF_NET_CHAT& adata)
		{
			int32_t lnow = (int32_t)localtime::gettime();
			if (lnow < m_info.notalkutc())
			{
				auto pro = std::make_shared<pbnet::PROBUFF_NET_ERROR>();
				pro->set_m_errmessage(std::format("ban talk [{}]", tools::time2str(m_info.notalkutc())));
				send_client(id_guid(), pro);
				return ecross_none;
			}
			switch (adata.m_channelid())
			{
			case pbnet::enum_chat_channel::enum_chat_ordinary:
				return ecross_ordinary;
			case pbnet::enum_chat_channel::enum_chat_cross_ordinary:
				return ecross_cross_ordinary;
			case pbnet::enum_chat_channel::enum_chat_none:
				return ecross_none;
			}
			return ecross_none;
		}

		// ranklist��ת������
		ecross forward_type(const pbnet::PROBUFF_NET_RANKLIST& adata)
		{
			return adata.m_iscross() ? ecross_cross_ordinary : ecross_ordinary;
		}

		//# ����forward_before��ָ��ת��ǰ�¼�
		template <typename T>
		bool forward_before(const T& adata)
		{
			return true;
		}

		//# ת��"��������"ǰ
		bool forward_before(const pbnet::PROBUFF_NET_CREATE_FAMIL& adata);

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
				return false;
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
			default:
				return true;
			}
			send_actor(lguid, pro);
			return true;
		}
#pragma endregion

		//# CMD Э��
		using handle_cmd = cmd<actor_role, std::string, actor_role*, const char*>;
		bool handle(const message<pbnet::PROBUFF_NET_CMD>& adata);

		//# ��ȡrole����
		bool handle(const message<pbnet::PROBUFF_NET_ROLE_SYNC>& adata);

		//# ��ȡ������ʱ��
		bool handle(const message<pbnet::PROBUFF_NET_GET_TIME>& adata);

		//# �л���·������
		bool handle(const message<pbnet::PROBUFF_NET_SWITCH_LINE>& adata);

		//# ������Ʒ
		bool handle(const message<np_actor_senditem>& adata);

		//# �Ͽ�����
		bool handle(const message<np_actor_disconnect_close>& adata);

		//# �淨�����ɹ�  ��¼�淨actorid
		bool handle(const message<pbnet::PROBUFF_NET_MATCHING_SUCCESS_RESPONSE>& adata);

		//# ��ȡ������
		bool handle(const message<pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD>& adata);

		int32_t rechange(std::string& aorderid, int32_t arechargeid, bool agm, bool areporting);

		//# gm����
		using handle_gm = cmd<actor_role, std::string, int, const ngl::json_read&>;
		bool handle(const message<mforward<np_gm>>& adata);

		//# ��¼����δ������ֵ
		void loginpay();

		//# ���󴴽�����
		void createorder(std::string& aorder, int32_t arechargeid);

		//# �Ƿ��״γ�ֵ
		bool is_first_recharge(int32_t arechargeid);

		//# ��ʱ��
		bool timer_handle(const message<timerparm>& adata);

		//# ���Ը��ͻ���һ���ַ���
		void echo_msg(const char* amsg);
	};
}//namespace ngl

