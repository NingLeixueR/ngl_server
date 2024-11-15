#pragma once

#include "actor_manage.h"
#include "actor_create.h"
#include "ndb_modular.h"
#include "nsp_server.h"
#include "nsp_client.h"
#include "ndbclient.h"
#include "nprotocol.h"
#include "db_manage.h"
#include "db_pool.h"
#include "db_data.h"
#include "ntimer.h"
#include "family.h"
#include "type.h"
#include "drop.h"
#include "nlog.h"
#include "net.h"
#include "db.h"

namespace ngl
{
	class actor_family : public actor
	{
		actor_family(const actor_family&) = delete;
		actor_family& operator=(const actor_family&) = delete;

		friend class familyer;
		friend class family;
		familyer m_familyer;
		family m_family;

		actor_family();
	public:
		friend class actor_instance<actor_family>;
		static actor_family& getInstance()
		{
			return actor_instance<actor_family>::instance();
		}

		virtual void init()
		{
			m_familyer.set(this);
			m_family.set(this);
		}

		static void nregister();

		virtual ~actor_family() {}

		virtual void loaddb_finish(bool adbishave) 
		{
			
		}

		static ENUM_ACTOR actor_type()
		{
			return ACTOR_FAMILY;
		}

		static i64_actorid actorid(i16_area area)
		{
			return nguid::make(ACTOR_FAMILY, area, nguid::none_actordataid());
		}

		// # ��������
		bool handle(const message<mforward<pbnet::PROBUFF_NET_CREATE_FAMIL>>& adata);

		// # ����/ȡ���������
		bool handle(const message<mforward<pbnet::PROBUFF_NET_JOIN_FAMIL>>& adata);

		// # ���ų�ͬ��/��ͬ����Ҽ������
		bool handle(const message<mforward<pbnet::PROBUFF_NET_RATIFY_JOIN_FAMIL>>& adata);

		// # ���ų�ת�þ���
		bool handle(const message<mforward<pbnet::PROBUFF_NET_CEDE_FAMIL>>& adata);

		// # �뿪����
		bool handle(const message<mforward<pbnet::PROBUFF_NET_LEAVE_FAMIL>>& adata);

		// # ��ȡ�����б�
		bool handle(const message<mforward<pbnet::PROBUFF_NET_FAMIL_LIST>>& adata);

		// # �޸ľ�������
		bool handle(const message<mforward<pbnet::PROBUFF_NET_CHANGE_FAMILNAME>>& adata);

		// # ����ǩ��
		bool handle(const message<mforward<pbnet::PROBUFF_NET_FAMILSIGN>>& adata);

		using handle_cmd = cmd<actor_mail, std::string, int, const ngl::json_read&>;
		// # GM����
		bool handle(const message<mforward<np_gm>>& adata);
	};
}// namespace ngl