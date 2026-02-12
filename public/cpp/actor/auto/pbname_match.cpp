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

 // 注意【makeproto 工具生成文件，不要手动修改】
 // 创建时间【2026-02-12 18:06:22】

#include "example.pb.h"
#include "actor_base.h"
#include "net.pb.h"
#include "tools.h"
#include "pack.h"

namespace ngl
{
	template <typename T>
	static bool jsontobinarypack(const char* aname, const char* ajson, std::shared_ptr<pack>& apack, i64_actorid aactorid, i64_actorid arequestactorid)
	{
		T ltemp;
		if (tools::json2proto(ajson, ltemp))
		{
			apack = actor_base::net_pack(ltemp, aactorid, arequestactorid);
			return true;
		}
        log_error()->print("jsontobinarypack fail pbname[{}] json[{}]", aname, ajson);
		return false;
	}

	template <typename T>
	class j2p
	{
	public:
		static void fun(const std::string& aname, const std::string& ajson, std::shared_ptr<ngl::pack>& apack, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			ngl::jsontobinarypack<T>(aname.c_str(), ajson.c_str(), apack, aactorid, arequestactorid);
		}
	};

	std::shared_ptr<pack> actor_base::jsonpack(const std::string& apbname, const std::string& ajson, i64_actorid aactorid, i64_actorid arequestactorid)
	{
		using type_j2pfun = std::function<void(const std::string&, const std::string&, std::shared_ptr<ngl::pack>&, i64_actorid, i64_actorid)>;
		static std::map<std::string, type_j2pfun> lmap;
		if (lmap.empty())
		{
            auto fun = [&]<typename ...TARG>()
            {
                ((lmap[tools::type_name<TARG>()] = std::bind_front(&j2p<TARG>::fun)), ...);
            };
            fun.operator()<
               pbnet::PROBUFF_NET_MSG_RESPONSE
               , pbnet::PROBUFF_NET_HEARTBEAT
               , pbnet::PROBUFF_NET_RECHARGE
               , pbnet::PROBUFF_NET_RECHARGE_RESPONSE
               , pbnet::PROBUFF_NET_DELIVER_GOODS_RECHARGE
               , pbnet::PROBUFF_NET_ERROR
               , pbnet::PROBUFF_NET_GET_TIME
               , pbnet::PROBUFF_NET_GET_TIME_RESPONSE
               , pbnet::PROBUFF_NET_NOTICE
               , pbnet::PROBUFF_NET_NOTICE_RESPONSE
               , pbnet::PROBUFF_NET_ACOUNT_LOGIN
               , pbnet::PROBUFF_NET_ACOUNT_LOGIN_RESPONSE
               , pbnet::PROBUFF_NET_ROLE_LOGIN
               , pbnet::PROBUFF_NET_ROLE_NOT_CREATE
               , pbnet::PROBUFF_NET_ROLE_CREATE
               , pbnet::PROBUFF_NET_ROLE_CREATE_RESPONSE
               , pbnet::PROBUFF_NET_ROLE_SYNC
               , pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE
               , pbnet::PROBUFF_NET_BAG_SYNC
               , pbnet::PROBUFF_NET_BAG_SYNC_RESPONSE
               , pbnet::PROBUFF_NET_BAG_UPDATE
               , pbnet::PROBUFF_NET_CMD
               , pbnet::PROBUFF_NET_CHAT
               , pbnet::PROBUFF_NET_CHAT_RESPONSE
               , pbnet::PROBUFF_NET_SWITCH_LINE
               , pbnet::PROBUFF_NET_SWITCH_LINE_RESPONSE
               , pbnet::PROBUFF_NET_MAIL_LIST
               , pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE
               , pbnet::PROBUFF_NET_MAIL_READ
               , pbnet::PROBUFF_NET_MAIL_READ_RESPONSE
               , pbnet::PROBUFF_NET_MAIL_DRAW
               , pbnet::PROBUFF_NET_MAIL_DRAW_RESPONSE
               , pbnet::PROBUFF_NET_MAIL_DEL
               , pbnet::PROBUFF_NET_MAIL_DEL_RESPONSE
               , pbnet::PROBUFF_NET_KCPSESSION
               , pbnet::PROBUFF_NET_KCPSESSION_RESPONSE
               , pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD
               , pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD_RESPONSE
               , pbnet::PROBUFF_NET_FAMIL_CREATE
               , pbnet::PROBUFF_NET_FAMIL_CREATE_RESPONSE
               , pbnet::PROBUFF_NET_FAMIL_INFO
               , pbnet::PROBUFF_NET_FAMIL_INFO_RESPONSE
               , pbnet::PROBUFF_NET_FAMIL_JOIN
               , pbnet::PROBUFF_NET_FAMIL_JOIN_RESPONSE
               , pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN
               , pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN_RESPONSE
               , pbnet::PROBUFF_NET_FAMIL_CEDE
               , pbnet::PROBUFF_NET_FAMIL_CEDE_RESPONSE
               , pbnet::PROBUFF_NET_FAMIL_LEAVE
               , pbnet::PROBUFF_NET_FAMIL_LEAVE_RESPONSE
               , pbnet::PROBUFF_NET_FAMIL_LIST
               , pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE
               , pbnet::PROBUFF_NET_FAMIL_CHANGENAME
               , pbnet::PROBUFF_NET_FAMIL_CHANGENAME_RESPONSE
               , pbnet::PROBUFF_NET_FAMIL_SIGN
               , pbnet::PROBUFF_NET_FAMIL_SIGN_RESPONSE
               , pbnet::PROBUFF_NET_REWARD_ITEM_RESPONSE
               , pbnet::PROBUFF_NET_RANKLIST
               , pbnet::PROBUFF_NET_RANKLIST_RESPONSE
               , pbnet::PROBUFF_NET_FRIEND
               , pbnet::PROBUFF_NET_FRIEND_RESPONSE
               , pbnet::PROBUFF_NET_FRIEND_ADD
               , pbnet::PROBUFF_NET_FRIEND_ADD_RESPONSE
               , pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD
               , pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD_RESPONSE
               , pbnet::PROBUFF_NET_FRIEND_ERASE
               , pbnet::PROBUFF_NET_FRIEND_ERASE_RESPONSE
               , pbnet::PROBUFF_NET_ROLESTAT
               , pbnet::PROBUFF_NET_SYNC_ATTRIBUTE
               , pbnet::PROBUFF_NET_SYNC_UNIT
               , pbnet::PROBUFF_NET_ENTER_LEAVE_VIEW
               , pbnet::PROBUFF_NET_SYNC_POSITION
               , pbnet::PROBUFF_NET_CHANGE_ANGLE
               , pbnet::PROBUFF_NET_TESTLUA
               , pbexample::PROBUFF_EXAMPLE_PLAY_JOIN
               , pbexample::PROBUFF_EXAMPLE_PLAY_JOIN_RESPONSE
               , pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING
               , pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING_RESULT
               , pbexample::PROBUFF_EXAMPLE_PLAY_PLAYER_CONFIRM
               , pbexample::PROBUFF_EXAMPLE_PLAY_CANCEL
               , pbexample::PROBUFF_EXAMPLE_PLAY_CREATE
               , pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE
               , pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE_RESPONSE
               , pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_INFO
               , pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER
               , pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_ERROR
               , pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_BOMB
            >();
         }
		auto lpfun = tools::findmap(lmap, apbname);
		if (lpfun != nullptr)
		{
			auto lpack = std::make_shared<ngl::pack>();
			(*lpfun)(apbname, ajson, lpack, aactorid, arequestactorid);
			return lpack;
		}
		return nullptr;
	}
}//namespace ngl
