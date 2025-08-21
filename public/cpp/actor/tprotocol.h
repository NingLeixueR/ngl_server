#pragma once
#include "nprotocol_template.h"
#include "template_arg.h"
#include "xmlprotocol.h"
#include "net.pb.h"
#include "lua.hpp"
#include "tools.h"

#include <map>

namespace ngl
{
	template <typename T, EPROTOCOL_TYPE PROTYPE, bool ISUSING, typename TREAL>
	struct np_actor_forward;


	enum enscript
	{
		enscript_none = -1,
		enscript_lua = 0,
		enscript_count,
	};

	class tprotocol
	{
		tprotocol() = delete;
		tprotocol(const tprotocol&) = delete;
		tprotocol& operator=(const tprotocol&) = delete;
	public:
		struct info
		{
			EPROTOCOL_TYPE	m_type;
			i32_protocolnum	m_protocol;
			std::string		m_name;
			bool m_forward = false;

			using func = std::function<bool(int64_t, void*)>;
			// # Ϊ�˸��ű��ṩ���ݽṹ���ַ������ݸ��ͻ���
			std::array<func, enscript_count> m_toclient;
			// # Ϊ�˸��ű��ṩ���ݽṹ���ַ������ݸ�����actor
			std::array<func, enscript_count> m_toactor;


			template <enscript SCRIPT>
			const func& toactor()
			{
				if constexpr (SCRIPT < enscript_count && SCRIPT > 0)
				{
					return m_toactor[SCRIPT];
				}
				static const func lnullfun = nullptr;
				return lnullfun;
			}

			template <enscript SCRIPT>
			const func& toclient()
			{
				if constexpr (SCRIPT < enscript_count && SCRIPT > 0)
				{
					return m_toclient[SCRIPT];
				}
				static const func lnullfun = nullptr;
				return lnullfun;
			}
		};
	private:
		static std::map<size_t, info>				m_keyval;
		static std::map<i32_protocolnum, info*>		m_protocol;
		static std::map<std::string, info*>			m_nameprotocol;
		// pbnet/pbexample		[1			-  100000000];
		// custom				[200000001	-  300000000];
		static int32_t								m_customs/* = 200000000*/;

		template <typename T>
		static size_t hash_code()
		{
			static size_t lcode = typeid(T).hash_code();
			return lcode;
		}

		//CUSTOM
		template <EPROTOCOL_TYPE TYPE, bool SCRIPT>
		struct tcustoms
		{
			template <typename T>
			static info* funcx(int32_t aprotocolnum = -1)
			{
				size_t lcode = hash_code<T>();
				if (m_keyval.contains(lcode))
				{
					return nullptr;
				}
				info& linfo = m_keyval[lcode];
				linfo.m_name = tools::type_name<T>();
				linfo.m_type = TYPE;

				linfo.m_protocol = (aprotocolnum == -1) ? ++m_customs : aprotocolnum;

				m_protocol[linfo.m_protocol] = &linfo;

				m_nameprotocol[linfo.m_name] = &linfo;
				std::cout << linfo.m_protocol << "-" << linfo.m_name << std::endl;
				return &linfo;
			}

			template <typename T>
			static info* func(int32_t aprotocolnum = -1);
		};

		struct tforward
		{
			template <typename T>
			static void func(int32_t aprotocolnum);
		};
	public:
		static bool set_customs_index(int32_t acustoms)
		{
			m_customs = acustoms;
			return true;
		}

		using tp_customs = template_arg<tcustoms<EPROTOCOL_TYPE_CUSTOM, false>>;
		using tp_customs_script = template_arg<tcustoms<EPROTOCOL_TYPE_CUSTOM, true>>;
		using tp_forward = template_arg<tforward, int32_t>;

		template <typename T>
		static bool init_protobufs()
		{
			static std::string& lname = tools::type_name<T>();
			i32_protocolnum lprotocol = xmlprotocol::protocol(lname);
			if (lprotocol == -1)
			{
				return false;
			}
			info& linfo = m_keyval[hash_code<T>()];
			linfo.m_name = lname;
			linfo.m_type = EPROTOCOL_TYPE_PROTOCOLBUFF;
			linfo.m_protocol = lprotocol;
			m_protocol[linfo.m_protocol] = &linfo;
			return true;
		}

		template <typename T>
		static info& get()
		{
			using TRC = std::remove_const<T>::type;
			size_t lcode = hash_code<T>();
			auto itor = m_keyval.find(lcode);
			if (itor == m_keyval.end())
			{
				if (init_protobufs<TRC>() == false)
				{
					std::cout << tools::type_name<T>() << std::endl;
					Throw("init_protobufs<TRC>() == false");
				}
				itor = m_keyval.find(lcode);
				if (itor == m_keyval.end())
				{
					Throw("itor == m_keyval.end()");
				}
			}
			return itor->second;
		}

		static info* getbyname(const char* aname)
		{
			auto itor = m_nameprotocol.find(aname);
			if (itor == m_nameprotocol.end())
			{
				return nullptr;
			}
			return itor->second;
		}

		// # ����Э���ȡЭ���
		template <typename T>
		static i32_protocolnum protocol()
		{
			info& linfo = get<T>();
			return linfo.m_protocol;
		}

		// # ����Э���ȡЭ������
		template <typename T>
		static EPROTOCOL_TYPE protocol_type()
		{
			info& linfo = get<T>();
			return linfo.m_type;
		}

		// # ����Э���ȡЭ������
		template <typename T>
		static const std::string& protocol_name()
		{
			info& linfo = get<T>();
			return linfo.m_name;
		}

		template <typename T>
		static bool isforward()
		{
			info& linfo = get<T>();
			return linfo.m_forward;
		}

		static info* get(i32_protocolnum aprotocolnum)
		{
			info** linfo = tools::findmap(m_protocol, aprotocolnum);
			return *linfo;
		}

		// # ����Э��Ż�ȡЭ������
		static std::string protocol_name(i32_protocolnum aprotocolnum)
		{
			info* linfo = get(aprotocolnum);
			if (linfo == nullptr)
			{
				return "none";
			}
			return linfo->m_name;
		}

		// # ����Э��Ż�ȡЭ������
		static EPROTOCOL_TYPE protocol(i32_protocolnum aprotocolnum)
		{
			info* linfo = get(aprotocolnum);
			if (linfo == nullptr)
			{
				return EPROTOCOL_TYPE_ERROR;
			}
			return linfo->m_type;
		}

		// # ��ȡ��ǰ������ע�������Э��
		static void get_allprotocol(
			std::map<i32_protocolnum
			, std::string>& apromap
			, std::map<i32_protocolnum, std::string>& acustommap
		)
		{
			for (const auto& apair : m_keyval)
			{
				const info& litem = apair.second;
				if (litem.m_type == EPROTOCOL_TYPE_CUSTOM)
				{
					acustommap[litem.m_protocol] = litem.m_name;
				}
				else
				{
					apromap[litem.m_protocol] = litem.m_name;
				}
			}
		}
	};
}// namespace ngl