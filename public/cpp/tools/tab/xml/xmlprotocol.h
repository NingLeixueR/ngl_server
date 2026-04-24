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
// File overview: XML protocol definition loader for message-type metadata.

#pragma once

#include "tools/tab/xml/xmlinfo.h"
#include "tools/log/nlog.h"
#include "tools/type.h"

#include <functional>
#include <optional>
#include <string>
#include <map>

namespace ngl
{
	struct xarg_protocols
	{
		struct info
		{
			int32_t m_client = 0;
			std::string m_name;
			int32_t m_number = 0;

			DXMLSERIALIZE(config, true, m_client, m_name, m_number)
		};
		std::vector<info> m_config;

		DXMLSERIALIZE(con, false, m_config)
	};

	class xmlprotocol
	{
		static std::map<std::string, int32_t> m_protocol;

		static void read(const char* axml);
	public:
		// Loads protocol name -> number overrides from XML into a static map.
		static void load();

		// Looks up a protocol number by name.
		static std::optional<int32_t> protocol(const std::string& aname);

		// Allocates a runtime-only protocol id outside the configured XML set.
		static int32_t free_protocol();
	};
}// namespace ngl
