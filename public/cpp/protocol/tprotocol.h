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
			int				m_protocol;
			std::string		m_name;
		};
		static std::multimap<size_t, pinfo>		m_keyval;
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
				m_keyval.insert(std::make_pair(lcode, pinfo
					{
						.m_type = atype,
						.m_protocol = ++lcustoms,
						.m_name = tools::type_name<T>()
					}));
			}
		};
	public:
		using customs = template_arg<tcustoms, EPROTOCOL_TYPE>;

		template <typename T>
		static bool init_protobufs()
		{
			static std::string& lname = tools::type_name<T>();
			int32_t lprotocol = xmlprotocol::protocol(lname);
			if (lprotocol == -1)
			{
				return false;
			}
			m_keyval.insert(std::make_pair(typeid(T).hash_code(), pinfo
				{
					.m_type = EPROTOCOL_TYPE_PROTOCOLBUFF,
					.m_protocol = lprotocol,
					.m_name = lname
				}));
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

		template <typename T>
		static int32_t protocol()
		{
			pinfo& linfo = get<T>();
			return linfo.m_protocol;
		}

		template <typename T>
		static EPROTOCOL_TYPE protocol_type()
		{
			pinfo& linfo = get<T>();
			return linfo.m_type;
		}

		template <typename T>
		static const std::string& protocol_name()
		{
			pinfo& linfo = get<T>();
			return linfo.m_name;
		}
	};
}// namespace ngl