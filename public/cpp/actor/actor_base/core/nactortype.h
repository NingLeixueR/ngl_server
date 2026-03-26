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
// File overview: Declares interfaces for actor base.

#pragma once

#include "actor/actor_base/core/nactortype.h"
#include "tools/serialize/ndefine.h"
#include "actor/tab/csvtable.h"
#include "actor/generated/pb/net.pb.h"
#include "actor/generated/pb/db.pb.h"
#include "tools/log/nlog.h"

#include <cstdint>
#include <string>
#include <map>

namespace ngl
{
	// Global actor type registry. Values are serialized into routes, nguid
	// identifiers, and protocol metadata, so the numeric ids are stable.
	enum ENUM_ACTOR
	{
		ACTOR_NONE								= -1,
		// Core long-lived actors that usually exist once per logical owner.
		ACTOR_ROLE								= 1,
		ACTOR_ROBOT								= 2, // One actor per robot/test client.
		ACTOR_LOG								= 3, // Log
		ACTOR_EXAMPLE_GUESS_NUMBER				= 4, // Example gameplay actor.

		// Singleton/system actors.
		ACTOR_SIGNLE_START						= 20,
		ACTOR_SERVER							= ACTOR_SIGNLE_START, // Server-side address/routing manager.
		ACTOR_CLIENT							= 21, // Client-side address/routing manager.
		ACTOR_LOGIN								= 22, // Login coordinator.
		ACTOR_GATEWAY							= 23, // Gateway
		ACTOR_GATEWAY_C2G						= 24, // Gateway [Client]->[Game]
		ACTOR_GATEWAY_G2C						= 25, // Gateway [Game]->[Client]
		ACTOR_CREATE							= 26, // Specified[Server]oncreate[Actor]
		ACTOR_ROLE_MANAGE						= 27, // Responsible forcreateactor_role
		ACTOR_KCP								= 28, // kcp
		ACTOR_ROBOT_MANAGE						= 29, // Managerobot
		ACTOR_CSVSERVER							= 30, // csv server
		ACTOR_CSVCLIENT							= 31, // csv client
		ACTOR_NOTICE							= 32, // Notice
		ACTOR_GM								= 33, // GM (world )
		ACTOR_GMCLIENT							= 34, // GM CLIENT ( world this this )
		ACTOR_MAIL								= 35, // Mail
		ACTOR_CHAT								= 36, // Chat
		ACTOR_RANKLIST							= 37, // Rank list
		ACTOR_ACTIVITY_MANAGE					= 38, // Activitymanage
		ACTOR_BRIEF								= 39, // Briefsummaryinfo
		ACTOR_KEYVALUE							= 40, // Sharedkey/value
		ACTOR_FAMILY							= 41, // Guild
		ACTOR_FRIENDS							= 42, // Friends
		ACTOR_EVENTS							= 43, // Event
		ACTOR_EVENTS_MAX_COUNT					= ACTOR_EVENTS + 10, // Event 10
		ACTOR_EXAMPLE_MATCH						= 54, // Example match actor.
		ACTOR_EXAMPLE_MANAGE					= 55, // Example actor manager.
		ACTOR_TESTLUA							= 56, // Testlua
		ACTOR_TESTLUA2							= 57, // Testlua2
		ACTOR_DB								= 1000, // After
		ACTOR_SIGNLE_FINISH						= static_cast<int>(ACTOR_DB) + static_cast<int>(pbdb::ENUM_DB_COUNT),
		ACTOR_COUNT								= ACTOR_SIGNLE_FINISH,
	};

	// Helper for actor kinds that are allocated from the singleton/system range.
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
				tools::no_core_dump();
			}
			return m_type;
		}
	};

	template <typename T, typename ENUMT>
	ENUMT type_enum<T, ENUMT>::m_type = type_enum<T, ENUMT>::enum_null();

	template <typename TACTOR>
	using nactor_type = type_enum<TACTOR, ENUM_ACTOR>;

	// Map a DB table enum to the corresponding DB actor type.
	ENUM_ACTOR db_enum(pbdb::ENUM_DB TDBTAB_TYPE);

	// Bind actor classes to enum values.
	void auto_actor();

	// Register custom protocol ids.
	void tprotocol_customs();

	// Register generated forwarding protocol wrappers.
	void tprotocol_forward_pb();

	// Register actor lifecycle and event handlers.
	void event_register();
}//namespace ngl
