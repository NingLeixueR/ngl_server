 // 注意【makeproto 工具生成文件，不要手动修改】
 // 创建时间【2025-06-23 17:32:49】
#include "actor_base.h"
#include "tools.h"
#include "pack.h"

#include "example.pb.h"
#include "net.pb.h"

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
            #define mkdef_map(NAME) lmap[#NAME] = std::bind_front(&j2p<NAME>::fun)
            mkdef_map(pbnet::PROBUFF_NET_MSG_RESPONSE);
            mkdef_map(pbnet::PROBUFF_NET_HEARTBEAT);
            mkdef_map(pbnet::PROBUFF_NET_RECHARGE);
            mkdef_map(pbnet::PROBUFF_NET_RECHARGE_RESPONSE);
            mkdef_map(pbnet::PROBUFF_NET_DELIVER_GOODS_RECHARGE);
            mkdef_map(pbnet::PROBUFF_NET_ERROR);
            mkdef_map(pbnet::PROBUFF_NET_GET_TIME);
            mkdef_map(pbnet::PROBUFF_NET_GET_TIME_RESPONSE);
            mkdef_map(pbnet::PROBUFF_NET_NOTICE);
            mkdef_map(pbnet::PROBUFF_NET_NOTICE_RESPONSE);
            mkdef_map(pbnet::PROBUFF_NET_ACOUNT_LOGIN);
            mkdef_map(pbnet::PROBUFF_NET_ACOUNT_LOGIN_RESPONSE);
            mkdef_map(pbnet::PROBUFF_NET_ROLE_LOGIN);
            mkdef_map(pbnet::PROBUFF_NET_ROLE_SYNC);
            mkdef_map(pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE);
            mkdef_map(pbnet::PROBUFF_NET_BAG_SYNC);
            mkdef_map(pbnet::PROBUFF_NET_BAG_SYNC_RESPONSE);
            mkdef_map(pbnet::PROBUFF_NET_BAG_UPDATE);
            mkdef_map(pbnet::PROBUFF_NET_CMD);
            mkdef_map(pbnet::PROBUFF_NET_CHAT);
            mkdef_map(pbnet::PROBUFF_NET_CHAT_RESPONSE);
            mkdef_map(pbnet::PROBUFF_NET_SWITCH_LINE);
            mkdef_map(pbnet::PROBUFF_NET_SWITCH_LINE_RESPONSE);
            mkdef_map(pbnet::PROBUFF_NET_MAIL_LIST);
            mkdef_map(pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE);
            mkdef_map(pbnet::PROBUFF_NET_MAIL_READ);
            mkdef_map(pbnet::PROBUFF_NET_MAIL_READ_RESPONSE);
            mkdef_map(pbnet::PROBUFF_NET_MAIL_DRAW);
            mkdef_map(pbnet::PROBUFF_NET_MAIL_DRAW_RESPONSE);
            mkdef_map(pbnet::PROBUFF_NET_MAIL_DEL);
            mkdef_map(pbnet::PROBUFF_NET_MAIL_DEL_RESPONSE);
            mkdef_map(pbnet::PROBUFF_NET_KCPSESSION);
            mkdef_map(pbnet::PROBUFF_NET_KCPSESSION_RESPONSE);
            mkdef_map(pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD);
            mkdef_map(pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD_RESPONSE);
            mkdef_map(pbnet::PROBUFF_NET_FAMIL_CREATE);
            mkdef_map(pbnet::PROBUFF_NET_FAMIL_CREATE_RESPONSE);
            mkdef_map(pbnet::PROBUFF_NET_FAMIL_INFO);
            mkdef_map(pbnet::PROBUFF_NET_FAMIL_INFO_RESPONSE);
            mkdef_map(pbnet::PROBUFF_NET_FAMIL_JOIN);
            mkdef_map(pbnet::PROBUFF_NET_FAMIL_JOIN_RESPONSE);
            mkdef_map(pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN);
            mkdef_map(pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN_RESPONSE);
            mkdef_map(pbnet::PROBUFF_NET_FAMIL_CEDE);
            mkdef_map(pbnet::PROBUFF_NET_FAMIL_CEDE_RESPONSE);
            mkdef_map(pbnet::PROBUFF_NET_FAMIL_LEAVE);
            mkdef_map(pbnet::PROBUFF_NET_FAMIL_LEAVE_RESPONSE);
            mkdef_map(pbnet::PROBUFF_NET_FAMIL_LIST);
            mkdef_map(pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE);
            mkdef_map(pbnet::PROBUFF_NET_FAMIL_CHANGENAME);
            mkdef_map(pbnet::PROBUFF_NET_FAMIL_CHANGENAME_RESPONSE);
            mkdef_map(pbnet::PROBUFF_NET_FAMIL_SIGN);
            mkdef_map(pbnet::PROBUFF_NET_FAMIL_SIGN_RESPONSE);
            mkdef_map(pbnet::PROBUFF_NET_REWARD_ITEM_RESPONSE);
            mkdef_map(pbnet::PROBUFF_NET_RANKLIST);
            mkdef_map(pbnet::PROBUFF_NET_RANKLIST_RESPONSE);
            mkdef_map(pbnet::PROBUFF_NET_FRIEND);
            mkdef_map(pbnet::PROBUFF_NET_FRIEND_RESPONSE);
            mkdef_map(pbnet::PROBUFF_NET_FRIEND_ADD);
            mkdef_map(pbnet::PROBUFF_NET_FRIEND_ADD_RESPONSE);
            mkdef_map(pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD);
            mkdef_map(pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD_RESPONSE);
            mkdef_map(pbnet::PROBUFF_NET_FRIEND_ERASE);
            mkdef_map(pbnet::PROBUFF_NET_FRIEND_ERASE_RESPONSE);
            mkdef_map(pbnet::PROBUFF_NET_ROLESTAT);
            mkdef_map(pbnet::PROBUFF_NET_SYNC_ATTRIBUTE);
            mkdef_map(pbnet::PROBUFF_NET_SYNC_UNIT);
            mkdef_map(pbnet::PROBUFF_NET_ENTER_LEAVE_VIEW);
            mkdef_map(pbnet::PROBUFF_NET_SYNC_POSITION);
            mkdef_map(pbnet::PROBUFF_NET_CHANGE_ANGLE);
            mkdef_map(pbexample::PROBUFF_EXAMPLE_PLAY_JOIN);
            mkdef_map(pbexample::PROBUFF_EXAMPLE_PLAY_JOIN_RESPONSE);
            mkdef_map(pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING);
            mkdef_map(pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING_RESULT);
            mkdef_map(pbexample::PROBUFF_EXAMPLE_PLAY_PLAYER_CONFIRM);
            mkdef_map(pbexample::PROBUFF_EXAMPLE_PLAY_CANCEL);
            mkdef_map(pbexample::PROBUFF_EXAMPLE_PLAY_CREATE);
            mkdef_map(pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE);
            mkdef_map(pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE_RESPONSE);
            mkdef_map(pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_INFO);
            mkdef_map(pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER);
            mkdef_map(pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_ERROR);
            mkdef_map(pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_BOMB);
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
