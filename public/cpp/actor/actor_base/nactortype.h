#pragma once

#include "csvtable.h"
#include "ndefine.h"
#include "net.pb.h"
#include "db.pb.h"

#include <cstdint>
#include <string>
#include <map>

namespace ngl
{
	// # actor��Ӧ��ö������
	enum ENUM_ACTOR
	{
		ACTOR_NONE = -1,
		//#######################  �ǵ���  #####################
		ACTOR_ROLE = 1,
		ACTOR_ROBOT,												// ������
		ACTOR_GUILD,												// ����	
		ACTOR_PLAYS,												// �淨
		ACTOR_MANAGE_ROLE,											// ���𴴽�actor_role
		ACTOR_PLAYS_GO_UNDERGROUNDPALACE = ACTOR_MANAGE_ROLE+1,		// �淨[�´��ع�]

		ACTOR_PLAYS_END,
		//#######################  ����  #######################
		ACTOR_SIGNLE_START,
		ACTOR_SERVER = ACTOR_SIGNLE_START,						// ���� actor address server  ����ַ���ַ������
		ACTOR_CLIENT,											// ���� actor address client 
		ACTOR_LOGIN,
		ACTOR_GATEWAY,											// ����
		ACTOR_GATEWAY_C2G,										// ���� [Client]->[Game]
		ACTOR_GATEWAY_G2C,										// ���� [Game]	->[Client]
		ACTOR_CREATE,											// ��ָ��[Server]�ϴ���[Actor]
		ACTOR_LOG,
		ACTOR_KCP,
		ACTOR_MANAGE_ROBOT,
		ACTOR_MANAGE_GUILD,										// ���������
		ACTOR_CSVCLIENT,										// �ȸ�csv client
		ACTOR_CSVSERVER,										// �ȸ�csv server
		ACTOR_NOTICE,											// ����
		ACTOR_GM,												// GM			(world ���� )
		ACTOR_GMCLIENT,											// GM CLIENT	(����world����ÿ�����̶�Ӧ���д˽���)
		ACTOR_MAIL,												// �ʼ�
		ACTOR_CHAT,												// ����
		ACTOR_RANKLIST,											// ���а�
		ACTOR_ACTIVITY_MANAGE,									// �����
		ACTOR_BRIEF,											// brief��Ҫ��Ϣ
		ACTOR_CROSS,											// ���
		ACTOR_MANAGE_PLAYS,										// �����淨
		ACTOR_MATCHING,											// ƥ��
		ACTOR_CALENDAR,											// �ű�����
		ACTOR_CHANNEL,											// ����/���� 
		ACTOR_KEYVALUE,											// ����key/value
		ACTOR_FAMILY,											// ���ż���
		ACTOR_FRIENDS,											// ����
		ACTOR_EVENTS,											// �¼�
		ACTOR_EVENTS_MAX_COUNT = ACTOR_EVENTS+10,				// �¼�Ԥ��10��
		ACTOR_DB,												// ���������һ��
		ACTOR_SIGNLE_FINISH = ACTOR_DB + pbdb::ENUM_DB_COUNT,
		ACTOR_COUNT = ACTOR_SIGNLE_FINISH,
	};

	// # �ж�ENUM_ACTORö���Ƿ�Ϊ����
	class enum_actor
	{
	public:
		static bool is_signle(ENUM_ACTOR aenum)
		{
			return aenum >= ACTOR_SIGNLE_START;
		}
	};

	template <typename T,typename ENUMT>
	class type_enum
	{
		type_enum() = delete;
		type_enum(const type_enum&) = delete;
		type_enum& operator=(const type_enum&) = delete;

		static ENUMT m_type;
	public:
		static void inits(ENUMT atype)
		{
			m_type = atype;
		}

		static ENUMT enum_null()
		{
			return (ENUMT)-1;
		}

		static ENUMT type()
		{
			if (m_type == enum_null())
			{
				log_error()->print("type_enum<{}, {}>::type() == enum_null()", tools::type_name<T>(), tools::type_name<ENUMT>());
				Throw("type_enum<{}, {}>::type() == enum_null()", tools::type_name<T>(), tools::type_name<ENUMT>());
			}
			return m_type;
		}
	};

	template <typename T, typename ENUMT>
	ENUMT type_enum<T, ENUMT>::m_type = type_enum<T, ENUMT>::enum_null();

	template <typename TACTOR>
	using nactor_type = type_enum<TACTOR, ENUM_ACTOR>;

	// # ����pbdb::ENUM_DB��ȡENUM_ACTOR
	ENUM_ACTOR db_enum(pbdb::ENUM_DB TDBTAB_TYPE);

	// # [aregister == true] ��Ҫ��ע��Э��,�������͵İ�
	// # [aregister == false] ʵ����db_actor,db server��Ҫ

	// # ��ʼ��"ע��Э��,�������͵İ�"
	void init_nactor_type();

	// # ʵ����db_actor,db server��Ҫ
	void init_db_actor(bool aisinstantiation);

	void init_core_db_actor(bool aisinstantiation);
}//namespace ngl