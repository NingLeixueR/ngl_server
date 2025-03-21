 // 注意【makeproto 工具生成文件，不要手动修改】
 // 创建时间【2025-03-21 13:40:53】
#include "actor_base.h"
#include "tools.h"
#include "pack.h"

#include "example.pb.h"
#include "net.pb.h"

namespace ngl
{
	template <typename T>
	static bool jsontobinarypack(const char* ajson, std::shared_ptr<pack>& apack, i64_actorid aactorid, i64_actorid arequestactorid)
	{
		T ltemp;
		if (tools::jsontopro(ajson, ltemp))
		{
			apack = actor_base::net_pack(ltemp, aactorid, arequestactorid);
			return true;
		}
		return false;
	}

	template <typename T>
	class j2p
	{
	public:
		static void fun(const std::string& ajson, std::shared_ptr<ngl::pack>& apack, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			ngl::jsontobinarypack<T>(ajson.c_str(), apack, aactorid, arequestactorid);
		}
	};

	
	std::shared_ptr<pack> actor_base::jsonpack(const std::string& apbname, const std::string& ajson, i64_actorid aactorid, i64_actorid arequestactorid)
	{
		using type_j2pfun = std::function<void(const std::string&, std::shared_ptr<ngl::pack>&, i64_actorid, i64_actorid)>;
		static std::map<std::string, type_j2pfun> lmap;
		if (lmap.empty())
		{
           lmap["pbnet::PROBUFF_NET_MSG_RESPONSE"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_MSG_RESPONSE>::fun);
           lmap["pbnet::PROBUFF_NET_HEARTBEAT"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_HEARTBEAT>::fun);
           lmap["pbnet::PROBUFF_NET_RECHARGE"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_RECHARGE>::fun);
           lmap["pbnet::PROBUFF_NET_RECHARGE_RESPONSE"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_RECHARGE_RESPONSE>::fun);
           lmap["pbnet::PROBUFF_NET_DELIVER_GOODS_RECHARGE"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_DELIVER_GOODS_RECHARGE>::fun);
           lmap["pbnet::PROBUFF_NET_ERROR"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_ERROR>::fun);
           lmap["pbnet::PROBUFF_NET_GET_TIME"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_GET_TIME>::fun);
           lmap["pbnet::PROBUFF_NET_GET_TIME_RESPONSE"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_GET_TIME_RESPONSE>::fun);
           lmap["pbnet::PROBUFF_NET_NOTICE"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_NOTICE>::fun);
           lmap["pbnet::PROBUFF_NET_NOTICE_RESPONSE"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_NOTICE_RESPONSE>::fun);
           lmap["pbnet::PROBUFF_NET_ACOUNT_LOGIN"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_ACOUNT_LOGIN>::fun);
           lmap["pbnet::PROBUFF_NET_ACOUNT_LOGIN_RESPONSE"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_ACOUNT_LOGIN_RESPONSE>::fun);
           lmap["pbnet::PROBUFF_NET_ROLE_LOGIN"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_ROLE_LOGIN>::fun);
           lmap["pbnet::PROBUFF_NET_ROLE_SYNC"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_ROLE_SYNC>::fun);
           lmap["pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE>::fun);
           lmap["pbnet::PROBUFF_NET_BAG_SYNC"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_BAG_SYNC>::fun);
           lmap["pbnet::PROBUFF_NET_BAG_SYNC_RESPONSE"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_BAG_SYNC_RESPONSE>::fun);
           lmap["pbnet::PROBUFF_NET_BAG_UPDATE"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_BAG_UPDATE>::fun);
           lmap["pbnet::PROBUFF_NET_BAG_UPDATE_RESPONSE"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_BAG_UPDATE_RESPONSE>::fun);
           lmap["pbnet::PROBUFF_NET_CMD"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_CMD>::fun);
           lmap["pbnet::PROBUFF_NET_CHAT"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_CHAT>::fun);
           lmap["pbnet::PROBUFF_NET_CHAT_RESPONSE"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_CHAT_RESPONSE>::fun);
           lmap["pbnet::PROBUFF_NET_SWITCH_LINE"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_SWITCH_LINE>::fun);
           lmap["pbnet::PROBUFF_NET_SWITCH_LINE_RESPONSE"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_SWITCH_LINE_RESPONSE>::fun);
           lmap["pbnet::PROBUFF_NET_MAIL_LIST"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_MAIL_LIST>::fun);
           lmap["pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE>::fun);
           lmap["pbnet::PROBUFF_NET_MAIL_READ"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_MAIL_READ>::fun);
           lmap["pbnet::PROBUFF_NET_MAIL_READ_RESPONSE"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_MAIL_READ_RESPONSE>::fun);
           lmap["pbnet::PROBUFF_NET_MAIL_DRAW"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_MAIL_DRAW>::fun);
           lmap["pbnet::PROBUFF_NET_MAIL_DRAW_RESPONSE"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_MAIL_DRAW_RESPONSE>::fun);
           lmap["pbnet::PROBUFF_NET_MAIL_DEL"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_MAIL_DEL>::fun);
           lmap["pbnet::PROBUFF_NET_MAIL_DEL_RESPONSE"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_MAIL_DEL_RESPONSE>::fun);
           lmap["pbnet::PROBUFF_NET_SYNC_ATTRIBUTE"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_SYNC_ATTRIBUTE>::fun);
           lmap["pbnet::PROBUFF_NET_SYNC_UNIT"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_SYNC_UNIT>::fun);
           lmap["pbnet::PROBUFF_NET_ENTER_LEAVE_VIEW"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_ENTER_LEAVE_VIEW>::fun);
           lmap["pbnet::PROBUFF_NET_SYNC_POSITION"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_SYNC_POSITION>::fun);
           lmap["pbnet::PROBUFF_NET_CHANGE_ANGLE"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_CHANGE_ANGLE>::fun);
           lmap["pbnet::PROBUFF_NET_MATCHING"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_MATCHING>::fun);
           lmap["pbnet::PROBUFF_NET_MATCHING_RESPONSE"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_MATCHING_RESPONSE>::fun);
           lmap["pbnet::PROBUFF_NET_MATCHING_CANCEL"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_MATCHING_CANCEL>::fun);
           lmap["pbnet::PROBUFF_NET_MATCHING_CANCEL_RESPONSE"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_MATCHING_CANCEL_RESPONSE>::fun);
           lmap["pbnet::PROBUFF_NET_MEMBER_MATCHING_CANCEL"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_MEMBER_MATCHING_CANCEL>::fun);
           lmap["pbnet::PROBUFF_NET_MATCHING_WAITCONFIRM"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_MATCHING_WAITCONFIRM>::fun);
           lmap["pbnet::PROBUFF_NET_MATCHING_CONFIRM"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_MATCHING_CONFIRM>::fun);
           lmap["pbnet::PROBUFF_NET_MATCHING_CONFIRM_RESPONSE"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_MATCHING_CONFIRM_RESPONSE>::fun);
           lmap["pbnet::PROBUFF_NET_SYNC_MATCHING_CONFIRM"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_SYNC_MATCHING_CONFIRM>::fun);
           lmap["pbnet::PROBUFF_NET_MATCHING_SYNC"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_MATCHING_SYNC>::fun);
           lmap["pbnet::PROBUFF_NET_MATCHING_SUCCESS_RESPONSE"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_MATCHING_SUCCESS_RESPONSE>::fun);
           lmap["pbnet::PROBUFF_NET_MATCHING_SUCCESS"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_MATCHING_SUCCESS>::fun);
           lmap["pbnet::PROBUFF_NET_ENTER_PLAYS"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_ENTER_PLAYS>::fun);
           lmap["pbnet::PROBUFF_NET_KCPSESSION"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_KCPSESSION>::fun);
           lmap["pbnet::PROBUFF_NET_KCPSESSION_RESPONSE"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_KCPSESSION_RESPONSE>::fun);
           lmap["pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD>::fun);
           lmap["pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD_RESPONSE"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD_RESPONSE>::fun);
           lmap["pbnet::PROBUFF_NET_CREATE_FAMIL"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_CREATE_FAMIL>::fun);
           lmap["pbnet::PROBUFF_NET_CREATE_FAMIL_RESPONSE"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_CREATE_FAMIL_RESPONSE>::fun);
           lmap["pbnet::PROBUFF_NET_JOIN_FAMIL"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_JOIN_FAMIL>::fun);
           lmap["pbnet::PROBUFF_NET_JOIN_FAMIL_RESPONSE"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_JOIN_FAMIL_RESPONSE>::fun);
           lmap["pbnet::PROBUFF_NET_RATIFY_JOIN_FAMIL"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_RATIFY_JOIN_FAMIL>::fun);
           lmap["pbnet::PROBUFF_NET_RATIFY_JOIN_FAMIL_RESPONSE"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_RATIFY_JOIN_FAMIL_RESPONSE>::fun);
           lmap["pbnet::PROBUFF_NET_CEDE_FAMIL"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_CEDE_FAMIL>::fun);
           lmap["pbnet::PROBUFF_NET_CEDE_FAMIL_RESPONSE"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_CEDE_FAMIL_RESPONSE>::fun);
           lmap["pbnet::PROBUFF_NET_LEAVE_FAMIL"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_LEAVE_FAMIL>::fun);
           lmap["pbnet::PROBUFF_NET_LEAVE_FAMIL_RESPONSE"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_LEAVE_FAMIL_RESPONSE>::fun);
           lmap["pbnet::PROBUFF_NET_FAMIL_LIST"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_FAMIL_LIST>::fun);
           lmap["pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE>::fun);
           lmap["pbnet::PROBUFF_NET_CHANGE_FAMILNAME"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_CHANGE_FAMILNAME>::fun);
           lmap["pbnet::PROBUFF_NET_CHANGE_FAMILNAME_RESPONSE"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_CHANGE_FAMILNAME_RESPONSE>::fun);
           lmap["pbnet::PROBUFF_NET_FAMILSIGN"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_FAMILSIGN>::fun);
           lmap["pbnet::PROBUFF_NET_FAMILSIGN_RESPONSE"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_FAMILSIGN_RESPONSE>::fun);
           lmap["pbnet::PROBUFF_NET_REWARD_ITEM_RESPONSE"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_REWARD_ITEM_RESPONSE>::fun);
           lmap["pbnet::PROBUFF_NET_RANKLIST"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_RANKLIST>::fun);
           lmap["pbnet::PROBUFF_NET_RANKLIST_RESPONSE"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_RANKLIST_RESPONSE>::fun);
           lmap["pbnet::PROBUFF_NET_FRIEND"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_FRIEND>::fun);
           lmap["pbnet::PROBUFF_NET_FRIEND_RESPONSE"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_FRIEND_RESPONSE>::fun);
           lmap["pbnet::PROBUFF_NET_ADDFRIEND"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_ADDFRIEND>::fun);
           lmap["pbnet::PROBUFF_NET_ADDFRIEND_RESPONSE"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_ADDFRIEND_RESPONSE>::fun);
           lmap["pbnet::PROBUFF_NET_RATIFY_ADDFRIEND"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_RATIFY_ADDFRIEND>::fun);
           lmap["pbnet::PROBUFF_NET_RATIFY_ADDFRIEND_RESPONSE"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_RATIFY_ADDFRIEND_RESPONSE>::fun);
           lmap["pbnet::PROBUFF_NET_ERASEFRIEND"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_ERASEFRIEND>::fun);
           lmap["pbnet::PROBUFF_NET_ERASEFRIEND_RESPONSE"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_ERASEFRIEND_RESPONSE>::fun);
           lmap["pbnet::PROBUFF_NET_ROLESTAT"] = std::bind_front(&j2p<pbnet::PROBUFF_NET_ROLESTAT>::fun);
           lmap["pbexample::PROBUFF_EXAMPLE_PLAY_JOIN"] = std::bind_front(&j2p<pbexample::PROBUFF_EXAMPLE_PLAY_JOIN>::fun);
           lmap["pbexample::PROBUFF_EXAMPLE_PLAY_JOIN_RESPONSE"] = std::bind_front(&j2p<pbexample::PROBUFF_EXAMPLE_PLAY_JOIN_RESPONSE>::fun);
           lmap["pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING"] = std::bind_front(&j2p<pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING>::fun);
           lmap["pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING_RESULT"] = std::bind_front(&j2p<pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING_RESULT>::fun);
           lmap["pbexample::PROBUFF_EXAMPLE_PLAY_PLAYER_CONFIRM"] = std::bind_front(&j2p<pbexample::PROBUFF_EXAMPLE_PLAY_PLAYER_CONFIRM>::fun);
           lmap["pbexample::PROBUFF_EXAMPLE_PLAY_CANCEL"] = std::bind_front(&j2p<pbexample::PROBUFF_EXAMPLE_PLAY_CANCEL>::fun);
         }
		type_j2pfun* lpfun = tools::findmap(lmap, apbname);
		if (lpfun != nullptr)
		{
			std::shared_ptr<ngl::pack> lpack = std::make_shared<ngl::pack>();
			(*lpfun)(ajson, lpack, aactorid, arequestactorid);
			return lpack;
		}
		return nullptr;
	}
}//namespace ngl
