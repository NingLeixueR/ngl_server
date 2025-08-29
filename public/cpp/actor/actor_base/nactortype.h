#pragma once

#include "nactortype.h"
#include "csvtable.h"
#include "ndefine.h"
#include "net.pb.h"
#include "db.pb.h"
#include "nlog.h"

#include <cstdint>
#include <string>
#include <map>

namespace ngl
{
	// # actor��Ӧ��ö������
	enum ENUM_ACTOR
	{
		ACTOR_NONE = -1,
		//# �ǵ���
		ACTOR_ROLE = 1,
		ACTOR_ROBOT = 2,											// ������
		ACTOR_LOG = 3,												// ��־
		ACTOR_EXAMPLE_GUESS_NUMBER = 4,								// ����С��Ϸ:������-����ը��

		//# ����(����ȫ�ֵ������͵����̵���) 
		ACTOR_SIGNLE_START = 20,
		ACTOR_SERVER = ACTOR_SIGNLE_START,							// ���� actor address server  ����ַ���ַ������
		ACTOR_CLIENT = 21,											// ���� actor address client 
		ACTOR_LOGIN = 22,
		ACTOR_GATEWAY = 23,											// ����
		ACTOR_GATEWAY_C2G = 24,										// ���� [Client]->[Game]
		ACTOR_GATEWAY_G2C = 25,										// ���� [Game]	->[Client]
		ACTOR_CREATE = 26,											// ��ָ��[Server]�ϴ���[Actor]
		ACTOR_ROLE_MANAGE = 27,										// ���𴴽�actor_role
		ACTOR_KCP = 28,
		ACTOR_ROBOT_MANAGE = 29,
		ACTOR_CSVSERVER = 30,										// �ȸ�csv server
		ACTOR_CSVCLIENT = 31,										// �ȸ�csv client
		ACTOR_NOTICE = 32,											// ����
		ACTOR_GM = 33,												// GM			(world ���� )
		ACTOR_GMCLIENT = 34,										// GM CLIENT	(����world����ÿ�����̶�Ӧ���д˽���)
		ACTOR_MAIL = 35,											// �ʼ�
		ACTOR_CHAT = 36,											// ����
		ACTOR_RANKLIST = 37,										// ���а�
		ACTOR_ACTIVITY_MANAGE = 38,									// �����
		ACTOR_BRIEF = 39,											// brief��Ҫ��Ϣ
		ACTOR_KEYVALUE = 40,										// ����key/value
		ACTOR_FAMILY = 41,											// ���ż���
		ACTOR_FRIENDS = 42,											// ����
		ACTOR_EVENTS = 43,											// �¼�
		ACTOR_EVENTS_MAX_COUNT = ACTOR_EVENTS + 10,					// �¼�Ԥ��10��
		ACTOR_EXAMPLE_MATCH = 54,									// ����С��Ϸ:ƥ��
		ACTOR_EXAMPLE_MANAGE = 55,									// ������������С��Ϸ
		ACTOR_TESTLUA = 56,											// ����lua
		ACTOR_TESTLUA2 = 57,										// ����lua2
		ACTOR_DB = 57,												// ���������һ��
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
				log_error()->print("type_enum<{}, {}>::type() == enum_null()"
					, tools::type_name<T>()
					, tools::type_name<ENUMT>()
				);
				Throw("type_enum<{}, {}>::type() == enum_null()"
					, tools::type_name<T>()
					, tools::type_name<ENUMT>()
				);
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

	// �������͵İ�
	void auto_actor();

	// # ע���Զ���Э��
	void tprotocol_customs();

	// # ע��ת��Э��
	void tprotocol_forward_pb();

	// # actor eventע��
	void event_register();
}//namespace ngl