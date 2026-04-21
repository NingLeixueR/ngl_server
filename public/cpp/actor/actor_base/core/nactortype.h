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
// File overview: Actor type enum (ENUM_ACTOR) and singleton/dynamic classification helpers.

#pragma once

#include "actor/actor_base/core/nactortype.h"
#include "actor/generated/pb/net.pb.h"
#include "actor/generated/pb/db.pb.h"
#include "tools/serialize/ndefine.h"
#include "actor/tab/csvtable.h"
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
		ACTOR_TESTAI							= 5, // Test AI behavior tree actor.

		// Singleton/system actors.
		ACTOR_SIGNLE_START						= 1000,
		ACTOR_SERVER							= ACTOR_SIGNLE_START, // Server-side address/routing manager.
		ACTOR_CLIENT							= 1001, // Client-side address/routing manager.
		ACTOR_LOGIN								= 1002, // Login coordinator.
		ACTOR_GATEWAY							= 1003, // Gateway
		ACTOR_GATEWAY_C2G						= 1004, // Gateway [Client]->[Game]
		ACTOR_GATEWAY_G2C						= 1005, // Gateway [Game]->[Client]
		ACTOR_CREATE							= 1006, // Specified[Server]oncreate[Actor]
		ACTOR_ROLE_MANAGE						= 1007, // Responsible forcreateactor_role
		ACTOR_KCP								= 1008, // kcp
		ACTOR_ROBOT_MANAGE						= 1009, // Managerobot
		ACTOR_CSVSERVER							= 1010, // csv server
		ACTOR_CSVCLIENT							= 1011, // csv client
		ACTOR_NOTICE							= 1012, // Notice
		ACTOR_GM								= 1013, // GM (world )
		ACTOR_GMCLIENT							= 1014, // GM CLIENT ( world this this )
		ACTOR_MAIL								= 1015, // Mail
		ACTOR_CHAT								= 1016, // Chat
		ACTOR_RANKLIST							= 1017, // Rank list
		ACTOR_ACTIVITY_MANAGE					= 1018, // Activitymanage
		ACTOR_BRIEF								= 1019, // Briefsummaryinfo
		ACTOR_KEYVALUE							= 1020, // Sharedkey/value
		ACTOR_FAMILY							= 1021, // Guild
		ACTOR_FRIENDS							= 1022, // Friends
		ACTOR_EXAMPLE_MATCH						= 1023, // Example match actor.
		ACTOR_EXAMPLE_MANAGE					= 1024, // Example actor manager.
		ACTOR_TESTLUA							= 1025, // Testlua
		ACTOR_TESTLUA2							= 1026, // Testlua2

		ACTOR_EVENTS							= 10000, // Event
		ACTOR_EVENTS_MAX_COUNT					= ACTOR_EVENTS + 1000, // Event 10

		ACTOR_DB								= 20000, // DB actor range starts here.
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
