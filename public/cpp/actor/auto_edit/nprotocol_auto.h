// 注意【IDL 工具生成文件，不要手动修改】
// 创建时间 // 创建时间 25-06-19 15:52:10
#pragma once

#include "ndb_modular.h"
#include "nsp_client.h"
#include "nsp_server.h"
#include "actor_db.h"
#include "db.pb.h"


namespace ngl
{
	void tprotocol_customs_200000000()
	{
		tprotocol::set_customs_index(200000000);
		// 新增内部协议需要补充
		tprotocol::tp_customs::template func <
			/*200000001*/np_timerparm
			/*200000002, np_mass_actor<np_timerparm>*/
			/*200000003*/, np_robot_pram
			/*200000004, np_mass_actor<np_robot_pram>*/
			/*200000005*/, np_actor_server_register
			/*200000006, np_mass_actor<np_actor_server_register>*/
			/*200000007*/, np_connect_actor_server
			/*200000008, np_mass_actor<np_connect_actor_server>*/
			/*200000009*/, np_actornode_register
			/*200000010, np_mass_actor<np_actornode_register>*/
			/*200000011*/, np_actornode_register_response
			/*200000012, np_mass_actor<np_actornode_register_response>*/
			/*200000013*/, np_actorclient_node_connect
			/*200000014, np_mass_actor<np_actorclient_node_connect>*/
			/*200000015*/, np_actornode_update
			/*200000016, np_mass_actor<np_actornode_update>*/
			/*200000017*/, np_actornode_update_server
			/*200000018, np_mass_actor<np_actornode_update_server>*/
			/*200000019*/, np_actornode_update_mass
			/*200000020, np_mass_actor<np_actornode_update_mass>*/
			/*200000021*/, np_actornode_connect_task
			/*200000022, np_mass_actor<np_actornode_connect_task>*/
			/*200000023*/, np_actorrole_login
			/*200000024, np_mass_actor<np_actorrole_login>*/
			/*200000025*/, np_gateway_close_session
			/*200000026, np_mass_actor<np_gateway_close_session>*/
			/*200000027*/, np_actorswitch_process_role
			/*200000028, np_mass_actor<np_actorswitch_process_role>*/
			/*200000029*/, np_actorswitch_process_plays
			/*200000030, np_mass_actor<np_actorswitch_process_plays>*/
			/*200000031*/, np_actorserver_connect
			/*200000032, np_mass_actor<np_actorserver_connect>*/
			/*200000033*/, np_actor_session_close
			/*200000034, np_mass_actor<np_actor_session_close>*/
			/*200000035*/, np_actor_disconnect_close
			/*200000036, np_mass_actor<np_actor_disconnect_close>*/
			/*200000037*/, np_actor_gatewayid_updata
			/*200000038, np_mass_actor<np_actor_gatewayid_updata>*/
			/*200000039*/, np_actor_broadcast
			/*200000040, np_mass_actor<np_actor_broadcast>*/
			/*200000041*/, np_actor_reloadcsv
			/*200000042, np_mass_actor<np_actor_reloadcsv>*/
			/*200000043*/, np_actor_csv_verify_version
			/*200000044, np_mass_actor<np_actor_csv_verify_version>*/
			/*200000045*/, np_actor_senditem
			/*200000046, np_mass_actor<np_actor_senditem>*/
			/*200000047*/, np_actor_gatewayinfo_updata
			/*200000048, np_mass_actor<np_actor_gatewayinfo_updata>*/
			/*200000049*/, np_actor_addmail
			/*200000050, np_mass_actor<np_actor_addmail>*/
			/*200000051*/, np_actor_kcp
			/*200000052, np_mass_actor<np_actor_kcp>*/
			/*200000053*/, np_actor_addnotice
			/*200000054, np_mass_actor<np_actor_addnotice>*/
			/*200000055*/, np_actor_close
			/*200000056, np_mass_actor<np_actor_close>*/
			/*200000057*/, np_logitem
			/*200000058, np_mass_actor<np_logitem>*/
			/*200000059*/, np_login_request_info
			/*200000060, np_mass_actor<np_login_request_info>*/
			/*200000061*/, np_create_example
			/*200000062, np_mass_actor<np_create_example>*/
			/*200000063*/, np_example_entergame_ready
			/*200000064, np_mass_actor<np_example_entergame_ready>*/
			/*200000065*/, np_example_actorid
			/*200000066, np_mass_actor<np_example_actorid>*/
			/*200000067*/, np_example_equit
			/*200000068, np_mass_actor<np_example_equit>*/
			/*200000069*/, np_thruput_test
			/*200000070, np_mass_actor<np_thruput_test>*/
			/*200000071*/, np_operator_task
			/*200000072, np_mass_actor<np_operator_task>*/
			/*200000073*/, np_operator_task_response
			/*200000074, np_mass_actor<np_operator_task_response>*/
			/*200000075*/, np_get_rank
			/*200000076, np_mass_actor<np_get_rank>*/
			/*200000077*/, np_get_rank_response
			/*200000078, np_mass_actor<np_get_rank_response>*/
			/*200000079*/, np_activityrank_operator
			/*200000080, np_mass_actor<np_activityrank_operator>*/
		>();
		tprotocol::set_customs_index(210000000);
		tprotocol::tp_customs::template func<
			/*210000001*/mforward<np_timerparm>
			/*210000002*/, mforward<np_robot_pram>
			/*210000003*/, mforward<np_actor_server_register>
			/*210000004*/, mforward<np_connect_actor_server>
			/*210000005*/, mforward<np_actornode_register>
			/*210000006*/, mforward<np_actornode_register_response>
			/*210000007*/, mforward<np_actorclient_node_connect>
			/*210000008*/, mforward<np_actornode_update>
			/*210000009*/, mforward<np_actornode_update_server>
			/*210000010*/, mforward<np_actornode_update_mass>
			/*210000011*/, mforward<np_actornode_connect_task>
			/*210000012*/, mforward<np_actorrole_login>
			/*210000013*/, mforward<np_gateway_close_session>
			/*210000014*/, mforward<np_actorswitch_process_role>
			/*210000015*/, mforward<np_actorswitch_process_plays>
			/*210000016*/, mforward<np_actorserver_connect>
			/*210000017*/, mforward<np_actor_session_close>
			/*210000018*/, mforward<np_actor_disconnect_close>
			/*210000019*/, mforward<np_actor_gatewayid_updata>
			/*210000020*/, mforward<np_actor_broadcast>
			/*210000021*/, mforward<np_actor_reloadcsv>
			/*210000022*/, mforward<np_actor_csv_verify_version>
			/*210000023*/, mforward<np_actor_senditem>
			/*210000024*/, mforward<np_actor_gatewayinfo_updata>
			/*210000025*/, mforward<np_actor_addmail>
			/*210000026*/, mforward<np_actor_kcp>
			/*210000027*/, mforward<np_actor_addnotice>
			/*210000028*/, mforward<np_actor_close>
			/*210000029*/, mforward<np_logitem>
			/*210000030*/, mforward<np_login_request_info>
			/*210000031*/, mforward<np_create_example>
			/*210000032*/, mforward<np_example_entergame_ready>
			/*210000033*/, mforward<np_example_actorid>
			/*210000034*/, mforward<np_example_equit>
			/*210000035*/, mforward<np_thruput_test>
			/*210000036*/, mforward<np_operator_task>
			/*210000037*/, mforward<np_operator_task_response>
			/*210000038*/, mforward<np_get_rank>
			/*210000039*/, mforward<np_get_rank_response>
			/*210000040*/, mforward<np_activityrank_operator>
		>();
	}
}//namespace ngl
