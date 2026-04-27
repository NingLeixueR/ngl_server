/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Declares interfaces for protocol.

#pragma once

#include "actor/protocol/nprotocol_template.h"
#include "tools/tools/tools_template_arg.h"
#include "tools/tab/xml/xmlprotocol.h"
#include "actor/generated/pb/net.pb.h"
#include "tools/tools/tools_nhash.h"
#include "tools/tools.h"
#include "lua.hpp"

#include <optional>
#include <array>
#include <map>

namespace ngl
{
	enum enscript
	{
		enscript_none = -1,		// No scripting bridge.
		enscript_lua = 0,		// Lua scripting bridge.
		enscript_count,			// Number of supported scripting backends.
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
			int32_t			m_highvalue = 0; // Lower values may pass earlier readiness gates.
			std::string		m_name;          // Debug/display name.

			// Script bridge callbacks for sending the protocol to clients.
			std::array<funclientc, enscript_count> m_toclient;
			// Script bridge callbacks for sending the protocol to actors.
			std::array<func, enscript_count> m_toactor;

			template <enscript SCRIPT>
			const func* toactor()
			{
				if constexpr (SCRIPT >= 0 && SCRIPT < enscript_count)
				{
					return &m_toactor[(size_t)SCRIPT];
				}
				else
				{
					return nullptr;
				}
			}

			template <enscript SCRIPT>
			const funclientc* toclient()
			{
				if constexpr (SCRIPT >= 0 && SCRIPT < enscript_count)
				{
					return &m_toclient[(size_t)SCRIPT];
				}
				else
				{
					return nullptr;
				}
			}
		};
	private:
		static std::map<tools::nhashcode, info>		m_keyval;
		static std::map<i32_protocolnum, info*>		m_protocol;
		static std::map<std::string, info*>			m_nameprotocol;
		static int32_t								m_customs;

		// Register custom protocols that are not driven by protobuf xmlprotocol ids.
		template <bool SCRIPT>
		class tcustoms
		{
			template <typename T>
			static info* func_base(int32_t aprotocolnum = -1, int32_t ahigh = 0)
			{
				tools::nhashcode lcode = tools::nhash::code<T>();
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
			static info* funcx(int32_t aprotocolnum = -1, int32_t ahigh = 0)
			{
				info* lpinfo = func_base<T>(aprotocolnum, ahigh);
				func_base<np_mass_actor<T>>(-1);
				return lpinfo;
			}
		public:
			template <typename T>
			static info* func(int32_t aprotocolnum = -1, int32_t ahigh = 0);
		};

		struct tforward
		{
			template <typename T>
			static void func(int32_t aprotocolnum);
		};
	public:
		// Set the next custom protocol id base before a registration block runs.
		static bool set_customs_index(int32_t acustoms)
		{
			m_customs = acustoms;
			return true;
		}

		// Register one or more custom C++ protocols.
		template <typename ...ARGS>
		static void tp_customs(int32_t aprotocolnum = -1, int32_t ahigh = 0)
		{
			(tcustoms<false>::func<ARGS>(aprotocolnum, ahigh), ...);
		}

		// Register one or more custom protocols and expose script send bridges for them.
		template <typename ...ARGS>
		static void tp_customs_script(int32_t aprotocolnum = -1, int32_t ahigh = 0)
		{
			(tcustoms<true>::func<ARGS>(aprotocolnum, ahigh), ...);
		}

		// Lazily resolve protobuf-backed protocol ids from xmlprotocol metadata.
		template <typename T>
		static bool init_protobufs()
		{
			auto lprotocol = xmlprotocol::protocol(tools::type_name<T>());
			if (!lprotocol.has_value())
			{
				return false;
			}
			info& linfo = m_keyval[tools::nhash::code<T>()];
			linfo.m_name = tools::type_name<T>();
			linfo.m_protocol = *lprotocol;
			m_protocol[linfo.m_protocol] = &linfo;
			m_nameprotocol[linfo.m_name] = &linfo;
			return true;
		}

		// Resolve the metadata block for a C++ type, creating it lazily if needed.
		template <typename T>
		static info* get()
		{
			using TRC = std::remove_const<T>::type;
			tools::nhashcode lcode = tools::nhash::code<T>();
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

		template <typename T, typename ...TAGES>
		static bool set_hightvalue(int32_t ahightlevel)
		{
			info* lpinfo = get<T>();
			if (lpinfo == nullptr)
			{
				return false;
			}
			lpinfo->m_highvalue = ahightlevel;
			if constexpr (sizeof...(TAGES) > 0)
			{
				return set_hightvalue<TAGES...>(ahightlevel);
			}
			return true;
		}

		// Return the protocol number for a C++ type.
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

		// Return the registered display name for a protocol id.
		static const char* name(i32_protocolnum aprotocol)
		{
			info* linfo = get(aprotocol);
			if (linfo == nullptr)
			{
				return "none";
			}
			return linfo->m_name.c_str();
		}

		// Return the scheduler privilege level for a protocol id.
		static std::optional<int32_t> highvalue(i32_protocolnum aprotocol)
		{
			info* linfo = get(aprotocol);
			if (linfo == nullptr)
			{
				return std::nullopt;
			}
			return linfo->m_highvalue;
		}

		// Dump all currently registered protocol ids and names.
		static void allprotocol(std::map<i32_protocolnum, std::string>& amap)
		{
			for (auto& [_key, _value] : m_keyval)
			{
				amap[_value.m_protocol] = _value.m_name;
			}
		}
	};
}// namespace ngl
