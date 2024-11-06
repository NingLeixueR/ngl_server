#pragma once
#include "template_arg.h"
#include "xmlprotocol.h"
#include "net.pb.h"
#include "tools.h"

#include <map>

namespace ngl
{
	template <typename T, EPROTOCOL_TYPE PROTYPE, bool ISUSING, typename TREAL>
	struct np_actor_forward;

	class tprotocol
	{
		tprotocol() = delete;
		tprotocol(const tprotocol&) = delete;
		tprotocol& operator=(const tprotocol&) = delete;

		struct pinfo
		{
			EPROTOCOL_TYPE	m_type;
			i32_protocolnum	m_protocol;
			std::string		m_name;
		};
		static std::map<size_t, pinfo>						m_keyval;
		static std::map<i32_protocolnum, pinfo*>			m_protocol;
		// net/gm		[1			-  100000000];
		// custom		[200000001	-  300000000];
		static int32_t							lcustoms/* = 200000000*/;

		template <typename T>
		static size_t hash_code()
		{
			static size_t lcode = typeid(T).hash_code();
			return lcode;
		}

		//CUSTOM
		class tcustoms
		{
		public:
			template <typename T>
			static void func(EPROTOCOL_TYPE atype)
			{
				size_t lcode = hash_code<T>();
				if (m_keyval.find(lcode) != m_keyval.end())
				{
					return;
				}

				pinfo& linfo = m_keyval[lcode];
				linfo.m_name = tools::type_name<T>();
				linfo.m_type = atype;
				linfo.m_protocol = ++lcustoms;
				m_protocol[linfo.m_protocol] = &linfo;
			}
		};
	public:
		using customs = template_arg<tcustoms, EPROTOCOL_TYPE>;

		template <typename T>
		static bool init_protobufs()
		{
			static std::string& lname = tools::type_name<T>();
			i32_protocolnum lprotocol = xmlprotocol::protocol(lname);
			if (lprotocol == -1)
			{
				return false;
			}
			pinfo& linfo = m_keyval[hash_code<T>()];
			linfo.m_name = lname;
			linfo.m_type = EPROTOCOL_TYPE_PROTOCOLBUFF;
			linfo.m_protocol = lprotocol;
			m_protocol[linfo.m_protocol] = &linfo;

			return true;
		}

		template <typename T>
		static pinfo& get()
		{
			using TRC = std::remove_const<T>::type;
			size_t lcode = hash_code<T>();
			auto itor = m_keyval.find(lcode);
			if (itor == m_keyval.end())
			{
				if (init_protobufs<TRC>() == false)
					Throw("init_protobufs<TRC>() == false");
				itor = m_keyval.find(lcode);
				if (itor == m_keyval.end())
					Throw("itor == m_keyval.end()");
			}
			return itor->second;
		}

		// # 根据协议获取协议号
		template <typename T>
		static i32_protocolnum protocol()
		{
			pinfo& linfo = get<T>();
			return linfo.m_protocol;
		}

		// # 根据协议获取协议类型
		template <typename T>
		static EPROTOCOL_TYPE protocol_type()
		{
			pinfo& linfo = get<T>();
			return linfo.m_type;
		}

		// # 根据协议获取协议名称
		template <typename T>
		static const std::string& protocol_name()
		{
			pinfo& linfo = get<T>();
			return linfo.m_name;
		}

		static pinfo* get(i32_protocolnum aprotocolnum)
		{
			pinfo** linfo = tools::findmap(m_protocol, aprotocolnum);
			return *linfo;
		}

		// # 根据协议号获取协议名称
		static std::string protocol_name(i32_protocolnum aprotocolnum)
		{
			pinfo* linfo = get(aprotocolnum);
			if (linfo == nullptr)
				return "none";
			return linfo->m_name;
		}

		// # 根据协议号获取协议类型
		static EPROTOCOL_TYPE protocol(i32_protocolnum aprotocolnum)
		{
			pinfo* linfo = get(aprotocolnum);
			if (linfo == nullptr)
				return EPROTOCOL_TYPE_ERROR;
			return linfo->m_type;
		}

		// # 获取当前进程已注册的所有协议
		static void get_allprotocol(std::map<i32_protocolnum, std::string>& apromap, std::map<i32_protocolnum, std::string>& acustommap)
		{
			std::ranges::for_each(m_keyval, [&apromap, &acustommap](const auto& apair)
				{
					const pinfo& litem = apair.second;
					if (litem.m_type == EPROTOCOL_TYPE_CUSTOM)
					{
						acustommap[litem.m_protocol] = litem.m_name;
					}
					else
					{
						apromap[litem.m_protocol] = litem.m_name;
					}
				});
		}
	};
}// namespace ngl