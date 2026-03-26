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

#pragma once

#include "actor/actor_base/core/ndb_modular.h"
#include "actor/actor_base/nsp/nsp_server.h"
#include "actor/actor_base/nsp/nsp_write.h"
#include "actor/actor_base/nsp/nsp_read.h"
#include "actor/actor_base/core/actor_db.h"
#include "actor/generated/pb/db.pb.h"


namespace ngl
{
	void tprotocol_customs_200000000()
	{
		tprotocol::set_customs_index(200000000);
		// Newly added protocolneed to
		// RegisterTand"np_mass_actor<T>"
		tprotocol::tp_customs_script<
			/*200000002*/np_timerparm
			/*200000004*/, np_robot_pram
			/*200000006*/, np_actor_server_register
			/*200000008*/, np_connect_actor_server
			/*200000010*/, np_actornode_register
			/*200000012*/, np_actornode_register_response
			/*200000014*/, np_actorclient_node_connect
			/*200000016*/, np_actornode_update
			/*200000018*/, np_actornode_update_server
			/*200000020*/, np_actornode_update_mass
			/*200000022*/, np_actornode_connect_task
			/*200000024*/, np_actorrole_login
			/*200000026*/, np_gateway_close_session
			/*200000028*/, np_actorswitch_process_role
			/*200000030*/, np_actorswitch_process_plays
			/*200000032*/, np_actorserver_connect
			/*200000034*/, np_actor_session_close
			/*200000036*/, np_actor_disconnect_close
			/*200000038*/, np_actor_gatewayid_updata
			/*200000040*/, np_actor_broadcast
			/*200000042*/, np_actor_reloadcsv
			/*200000044*/, np_actor_csv_verify_version
			/*200000046*/, np_actor_senditem
			/*200000048*/, np_actor_gatewayinfo_updata
			/*200000050*/, np_actor_addmail
			/*200000052*/, np_actor_kcp
			/*200000054*/, np_actor_addnotice
			/*200000056*/, np_actor_close
			/*200000058*/, np_logitem
			/*200000060*/, np_login_request_info
			/*200000062*/, np_create_example
			/*200000064*/, np_example_entergame_ready
			/*200000066*/, np_example_actorid
			/*200000068*/, np_example_equit
			/*200000070*/, np_thruput_test
			/*200000072*/, np_operator_task
			/*200000074*/, np_operator_task_response
			/*200000076*/, np_get_rank
			/*200000078*/, np_get_rank_response
			/*200000080*/, np_activityrank_operator
			/*200000082*/, np_attribute_remove
		>();
		tprotocol::set_customs_index(210000000);
		tprotocol::tp_customs_script<
			/*210000002*/mforward<np_timerparm>
			/*210000004*/, mforward<np_robot_pram>
			/*210000006*/, mforward<np_actor_server_register>
			/*210000008*/, mforward<np_connect_actor_server>
			/*210000010*/, mforward<np_actornode_register>
			/*210000012*/, mforward<np_actornode_register_response>
			/*210000014*/, mforward<np_actorclient_node_connect>
			/*210000016*/, mforward<np_actornode_update>
			/*210000018*/, mforward<np_actornode_update_server>
			/*210000020*/, mforward<np_actornode_update_mass>
			/*210000022*/, mforward<np_actornode_connect_task>
			/*210000024*/, mforward<np_actorrole_login>
			/*210000026*/, mforward<np_gateway_close_session>
			/*210000028*/, mforward<np_actorswitch_process_role>
			/*210000030*/, mforward<np_actorswitch_process_plays>
			/*210000032*/, mforward<np_actorserver_connect>
			/*210000034*/, mforward<np_actor_session_close>
			/*210000036*/, mforward<np_actor_disconnect_close>
			/*210000038*/, mforward<np_actor_gatewayid_updata>
			/*210000040*/, mforward<np_actor_broadcast>
			/*210000042*/, mforward<np_actor_reloadcsv>
			/*210000044*/, mforward<np_actor_csv_verify_version>
			/*210000046*/, mforward<np_actor_senditem>
			/*210000048*/, mforward<np_actor_gatewayinfo_updata>
			/*210000050*/, mforward<np_actor_addmail>
			/*210000052*/, mforward<np_actor_kcp>
			/*210000054*/, mforward<np_actor_addnotice>
			/*210000056*/, mforward<np_actor_close>
			/*210000058*/, mforward<np_logitem>
			/*210000060*/, mforward<np_login_request_info>
			/*210000062*/, mforward<np_create_example>
			/*210000064*/, mforward<np_example_entergame_ready>
			/*210000066*/, mforward<np_example_actorid>
			/*210000068*/, mforward<np_example_equit>
			/*210000070*/, mforward<np_thruput_test>
			/*210000072*/, mforward<np_operator_task>
			/*210000074*/, mforward<np_operator_task_response>
			/*210000076*/, mforward<np_get_rank>
			/*210000078*/, mforward<np_get_rank_response>
			/*210000080*/, mforward<np_activityrank_operator>
			/*210000082*/, mforward<np_attribute_remove>
		>();
	}
}//namespace ngl
