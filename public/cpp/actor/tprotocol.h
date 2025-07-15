#pragma once
#include "nprotocol_template.h"
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
	public:
		struct pinfo
		{
			EPROTOCOL_TYPE	m_type;
			i32_protocolnum	m_protocol;
			std::string		m_name;
			bool m_forward = false;
			// # 为了给脚本提供根据结构名字发送数据给客户端:参数pb的json串
			std::function<bool(int64_t, const char*)> m_toclient;
			// # 为了给脚本提供根据结构名字发送数据给其他actor:参数pb的json串
			std::function<void(int64_t, const char*)> m_toactor;
		};
	private:
		static std::map<size_t, pinfo>				m_keyval;
		static std::map<i32_protocolnum, pinfo*>	m_protocol;
		static std::map<std::string, pinfo*>		m_nameprotocol;
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
		template <EPROTOCOL_TYPE TYPE, bool REGISTER_JSON>
		struct tcustoms
		{
			template <typename TX>
			static void send_actor(int64_t aactorid, const char* adata);

			template <typename T>
			static pinfo* funcx(int32_t aprotocolnum = -1)
			{
				size_t lcode = hash_code<T>();
				if (m_keyval.contains(lcode))
				{
					return nullptr;
				}
				pinfo& linfo = m_keyval[lcode];
				linfo.m_name = tools::type_name<T>();
				linfo.m_type = TYPE;

				linfo.m_protocol = (aprotocolnum == -1) ? ++m_customs : aprotocolnum;

				m_protocol[linfo.m_protocol] = &linfo;

				m_nameprotocol[linfo.m_name] = &linfo;
				std::cout << linfo.m_protocol << "-" << linfo.m_name << std::endl;
				return &linfo;
			}

			template <typename T>
			static pinfo* func(int32_t aprotocolnum = -1)
			{
				if constexpr (TYPE == EPROTOCOL_TYPE_CUSTOM)
				{
					pinfo* linfo = funcx<np_mass_actor<T>>(aprotocolnum);
					if (linfo != nullptr)
					{
						if constexpr (REGISTER_JSON)
						{
							linfo->m_toactor = std::bind(
								&tcustoms<TYPE, REGISTER_JSON>::send_actor<np_mass_actor<T>>
								, std::placeholders::_1
								, std::placeholders::_2
							);
						}						
					}
					linfo = funcx<T>(aprotocolnum);
					if (linfo != nullptr)
					{
						if constexpr (REGISTER_JSON)
						{
							linfo->m_toactor = std::bind(
								&tcustoms<TYPE, REGISTER_JSON>::send_actor<T>
								, std::placeholders::_1
								, std::placeholders::_2
							);
						}
					}
					return linfo;
				}
				else
				{
					return funcx<T>(aprotocolnum);
				}
			}
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
		using tp_customsjson = template_arg<tcustoms<EPROTOCOL_TYPE_CUSTOM, true>>;
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

		static pinfo* getbyname(const char* aname)
		{
			auto itor = m_nameprotocol.find(aname);
			if (itor == m_nameprotocol.end())
			{
				return nullptr;
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

		template <typename T>
		static bool isforward()
		{
			pinfo& linfo = get<T>();
			return linfo.m_forward;
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
			{
				return "none";
			}
			return linfo->m_name;
		}

		// # 根据协议号获取协议类型
		static EPROTOCOL_TYPE protocol(i32_protocolnum aprotocolnum)
		{
			pinfo* linfo = get(aprotocolnum);
			if (linfo == nullptr)
			{
				return EPROTOCOL_TYPE_ERROR;
			}
			return linfo->m_type;
		}

		// # 获取当前进程已注册的所有协议
		static void get_allprotocol(
			std::map<i32_protocolnum
			, std::string>& apromap
			, std::map<i32_protocolnum, std::string>& acustommap
		)
		{
			for (const auto& apair : m_keyval)
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
			}
		}
	};
}// namespace ngl