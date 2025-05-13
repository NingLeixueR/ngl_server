// 注意【IDL 工具生成文件，不要手动修改】
// 创建时间 // 创建时间 25-05-13 19:01:54
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
			/*200000002*/, np_robot_pram
			/*200000003*/, np_actor_server_register
			/*200000004*/, np_connect_actor_server
			/*200000005*/, np_actornode_register
			/*200000006*/, np_actornode_register_response
			/*200000007*/, np_actorclient_node_connect
			/*200000008*/, np_actornode_update
			/*200000009*/, np_actornode_update_server
			/*200000010*/, np_actornode_update_mass
			/*200000011*/, np_actornode_connect_task
			/*200000012*/, np_actorrole_login
			/*200000013*/, np_gateway_close_session
			/*200000014*/, np_actorswitch_process_role
			/*200000015*/, np_actorswitch_process_plays
			/*200000016*/, np_actorserver_connect
			/*200000017*/, np_actor_session_close
			/*200000018*/, np_actor_disconnect_close
			/*200000019*/, np_actor_gatewayid_updata
			/*200000020*/, np_actor_broadcast
			/*200000021*/, np_actor_reloadcsv
			/*200000022*/, np_actor_csv_verify_version
			/*200000023*/, np_actor_senditem
			/*200000024*/, np_actor_gatewayinfo_updata
			/*200000025*/, np_actor_addmail
			/*200000026*/, np_calendar_actor_activity
			/*200000027*/, np_calendar_actor_task
			/*200000028*/, np_calendar_actor_respond
			/*200000029*/, np_actor_calendar_requst
			/*200000030*/, np_actor_kcp
			/*200000031*/, np_actor_addnotice
			/*200000032*/, np_actor_close
			/*200000033*/, np_logitem
			/*200000034*/, np_channel_check
			/*200000035*/, np_login_request_info
			/*200000036*/, np_create_example
			/*200000037*/, np_example_entergame_ready
			/*200000038*/, np_example_actorid
			/*200000039*/, np_example_equit
			/*200000040*/, np_thruput_test
		> (EPROTOCOL_TYPE_CUSTOM);
	}
}//namespace ngl