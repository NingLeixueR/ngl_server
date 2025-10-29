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
	enum enscript
	{
		enscript_none = -1,		// ����
		enscript_lua = 0,		// lua
		enscript_count,			// ֧�ֵĽű�����
	};

	class tprotocol
	{
		tprotocol() = delete;
		tprotocol(const tprotocol&) = delete;
		tprotocol& operator=(const tprotocol&) = delete;
	public:
		struct info
		{
			i32_protocolnum	m_protocol;
			std::string		m_name;

			int8_t			m_highvalue = 0; // ��Ȩ��ֵ(0-127)

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
		template <bool SCRIPT>
		struct tcustoms
		{
			template <typename T>
			static info* funcx_s(int32_t aprotocolnum = -1, int8_t ahigh = 0)
			{
				size_t lcode = hash_code<T>();
				if (m_keyval.contains(lcode))
				{
					return nullptr;
				}
				info& linfo = m_keyval[lcode];
				linfo.m_name = tools::type_name<T>();
				linfo.m_protocol = (aprotocolnum == -1) ? ++m_customs : aprotocolnum;
				linfo.m_highvalue = ahigh;

				m_protocol[linfo.m_protocol] = &linfo;
				m_nameprotocol[linfo.m_name] = &linfo;
				return &linfo;
			}

			template <typename T>
			static info* funcx(int32_t aprotocolnum = -1, int8_t ahigh = 0)
			{
				info* lpinfo = funcx_s<T>(aprotocolnum, ahigh);
				funcx_s<np_mass_actor<T>>(-1);
				log_error()->print("{}-{}", linfo.m_protocol, linfo.m_name);
				return lpinfo;
			}

			template <typename T>
			static info* func(int32_t aprotocolnum = -1, int8_t ahigh = 0);
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

		using tp_customs = template_arg<tcustoms<false>, int32_t, int8_t>;
		using tp_customs_script = template_arg<tcustoms<true>>;

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

		static info* get(i32_protocolnum aprotocolnum)
		{
			info** linfo = tools::findmap(m_protocol, aprotocolnum);
			if (linfo == nullptr)
			{
				//tools::no_core_dump();
				return nullptr;
			}
			return *linfo;
		}

		// # ����Э��Ż�ȡЭ������
		static const char* protocol_name(i32_protocolnum aprotocolnum)
		{
			info* linfo = get(aprotocolnum);
			if (linfo == nullptr)
			{
				return "none";
			}
			return linfo->m_name.c_str();
		}

		// # ��ȡ��ǰ������ע�������Э��
		static void get_allprotocol(std::map<i32_protocolnum, std::string>& amap)
		{
			for (const auto& apair : m_keyval)
			{
				const info& litem = apair.second;
				amap[litem.m_protocol] = litem.m_name;
			}
		}
	};
}// namespace ngl