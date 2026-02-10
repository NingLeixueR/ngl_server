/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#pragma once
#include "nprotocol_template.h"
#include "template_arg.h"
#include "xmlprotocol.h"
#include "net.pb.h"
#include "lua.hpp"
#include "tools.h"
#include "nhash.h"

#include <map>

namespace ngl
{
	enum enscript
	{
		enscript_none = -1,		// 错误
		enscript_lua = 0,		// lua
		enscript_count,			// 支持的脚本数据
	};

	class tprotocol
	{
		tprotocol() = delete;
		tprotocol(const tprotocol&) = delete;
		tprotocol& operator=(const tprotocol&) = delete;
	public:
		struct info
		{
			using func = std::function<bool(int64_t, void*)>;
			using funclientc = std::function<bool(int64_t, const char*, void*)>;

			i32_protocolnum	m_protocol = 0;
			int8_t			m_highvalue = 0; // 高权限值(0-127)
			std::string		m_name;

			// # 为了给脚本提供根据结构名字发送数据给客户端
			std::array<funclientc, enscript_count> m_toclient;
			// # 为了给脚本提供根据结构名字发送数据给其他actor
			std::array<func, enscript_count> m_toactor;

			template <enscript SCRIPT>
			const func* toactor()
			{
				if constexpr (SCRIPT < enscript_count && SCRIPT > 0)
				{
					return &m_toactor[SCRIPT];
				}
				return nullptr;
			}

			template <enscript SCRIPT>
			const funclientc* toclient()
			{
				if constexpr (SCRIPT < enscript_count && SCRIPT > 0)
				{
					return &m_toclient[SCRIPT];
				}
				return nullptr;
			}
		};
	private:
		static std::map<nhashcode, info>			m_keyval;
		static std::map<i32_protocolnum, info*>		m_protocol;
		static std::map<std::string, info*>			m_nameprotocol;
		static int32_t								m_customs;

		//CUSTOM
		template <bool SCRIPT>
		class tcustoms
		{
			template <typename T>
			static info* func_base(int32_t aprotocolnum = -1, int8_t ahigh = 0)
			{
				nhashcode lcode = nhash::code<T>();
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
				//log_error()->print("{}-{}", linfo.m_protocol, linfo.m_name);
				return &linfo;
			}

			template <typename T>
			static info* funcx(int32_t aprotocolnum = -1, int8_t ahigh = 0)
			{
				info* lpinfo = func_base<T>(aprotocolnum, ahigh);
				func_base<np_mass_actor<T>>(-1);
				return lpinfo;
			}
		public:
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

		template <typename ...ARGS>
		static void tp_customs(int32_t aprotocolnum = -1, int8_t ahigh = 0)
		{
			(tcustoms<false>::func<ARGS>(aprotocolnum, ahigh), ...);
		}

		template <typename ...ARGS>
		static void tp_customs_script(int32_t aprotocolnum = -1, int8_t ahigh = 0)
		{
			(tcustoms<true>::func<ARGS>(aprotocolnum, ahigh), ...);
		}

		template <typename T>
		static bool init_protobufs()
		{
			static std::string& lname = tools::type_name<T>();
			i32_protocolnum lprotocol = xmlprotocol::protocol(lname);
			if (lprotocol == -1)
			{
				return false;
			}
			info& linfo = m_keyval[nhash::code<T>()];
			linfo.m_name = lname;
			linfo.m_protocol = lprotocol;
			m_protocol[linfo.m_protocol] = &linfo;
			return true;
		}

		template <typename T>
		static info* get()
		{
			using TRC = std::remove_const<T>::type;
			nhashcode lcode = nhash::code<T>();
			tprotocol::info* lpinfo = tools::findmap(m_keyval, lcode);
			if (lpinfo == nullptr)
			{
				if (!init_protobufs<TRC>())
				{
					std::cout << tools::type_name<T>() << std::endl;
					tools::no_core_dump();
					return nullptr;
				}
				lpinfo = tools::findmap(m_keyval, lcode);
			}
			return lpinfo;
		}

		static info* get(const char* aname)
		{
			auto lpinfo = tools::findmap(m_nameprotocol, aname);
			if (lpinfo == nullptr)
			{
				return nullptr;
			}
			return *lpinfo;
		}

		// # 根据协议获取协议号
		template <typename T>
		static i32_protocolnum protocol()
		{
			info* lpinfo = get<T>();
			if (lpinfo == nullptr)
			{
				tools::no_core_dump();
				return -1;
			}
			return lpinfo->m_protocol;
		}

		static info* get(i32_protocolnum aprotocol)
		{
			info** linfo = tools::findmap(m_protocol, aprotocol);
			if (linfo == nullptr)
			{
				return nullptr;
			}
			return *linfo;
		}

		// # 根据协议号获取协议名称
		static const char* name(i32_protocolnum aprotocol)
		{
			info* linfo = get(aprotocol);
			if (linfo == nullptr)
			{
				return "none";
			}
			return linfo->m_name.c_str();
		}

		static int8_t highvalue(i32_protocolnum aprotocol)
		{
			info* linfo = get(aprotocol);
			if (linfo == nullptr)
			{
				return -1;
			}
			return linfo->m_highvalue;
		}

		// # 获取当前进程已注册的所有协议
		static void allprotocol(std::map<i32_protocolnum, std::string>& amap)
		{
			for (auto& [_key, _value] : m_keyval)
			{
				amap[_value.m_protocol] = _value.m_name;
			}
		}
	};
}// namespace ngl