 // 注意【makeproto 工具生成文件，不要手动修改】
 // 创建时间【2025-05-29 22:01:35】
#include "nprotocol.h"
#include "tprotocol.h"
#include "actor.h"

#include "example.pb.h"
#include "net.pb.h"

#include <map>

namespace ngl
{
   template <typename PB>
   void help_role_tprotocol_forward_pb(const PB* apb)
   {
       int32_t lprotocolnum = tprotocol::protocol<PB>();
       tprotocol::tp_forward::template func<
           ngl::np_actor_forward<PB, EPROTOCOL_TYPE_PROTOCOLBUFF, true, ngl::forward>
           , ngl::np_actor_forward<PB, EPROTOCOL_TYPE_PROTOCOLBUFF, false, ngl::forward>
           , ngl::np_actor_forward<PB, EPROTOCOL_TYPE_PROTOCOLBUFF, true, PB>
           , ngl::np_actor_forward<PB, EPROTOCOL_TYPE_PROTOCOLBUFF, false, PB>
           , np_actormodule_forward<PB>
       >(EPROTOCOL_TYPE_PROTOCOLBUFF, lprotocolnum);
   }
   template <typename PB, typename ...ARG>
   void help_role_tprotocol_forward_pb(const PB* apb, const ARG*... arg)
   {
       help_role_tprotocol_forward_pb<PB>(apb);
       help_role_tprotocol_forward_pb<ARG...>(arg...);
   }
   void tprotocol_forward_pb()
   {
        help_role_tprotocol_forward_pb(
            null<pbnet::PROBUFF_NET_MSG_RESPONSE>,
            null<pbnet::PROBUFF_NET_HEARTBEAT>,
            null<pbnet::PROBUFF_NET_RECHARGE>,
            null<pbnet::PROBUFF_NET_RECHARGE_RESPONSE>,
            null<pbnet::PROBUFF_NET_DELIVER_GOODS_RECHARGE>,
            null<pbnet::PROBUFF_NET_ERROR>,
            null<pbnet::PROBUFF_NET_GET_TIME>,
            null<pbnet::PROBUFF_NET_GET_TIME_RESPONSE>,
            null<pbnet::PROBUFF_NET_NOTICE>,
            null<pbnet::PROBUFF_NET_NOTICE_RESPONSE>,
            null<pbnet::PROBUFF_NET_ACOUNT_LOGIN>,
            null<pbnet::PROBUFF_NET_ACOUNT_LOGIN_RESPONSE>,
            null<pbnet::PROBUFF_NET_ROLE_LOGIN>,
            null<pbnet::PROBUFF_NET_ROLE_SYNC>,
            null<pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE>,
            null<pbnet::PROBUFF_NET_BAG_SYNC>,
            null<pbnet::PROBUFF_NET_BAG_SYNC_RESPONSE>,
            null<pbnet::PROBUFF_NET_BAG_UPDATE>,
            null<pbnet::PROBUFF_NET_CMD>,
            null<pbnet::PROBUFF_NET_CHAT>,
            null<pbnet::PROBUFF_NET_CHAT_RESPONSE>,
            null<pbnet::PROBUFF_NET_SWITCH_LINE>,
            null<pbnet::PROBUFF_NET_SWITCH_LINE_RESPONSE>,
            null<pbnet::PROBUFF_NET_MAIL_LIST>,
            null<pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE>,
            null<pbnet::PROBUFF_NET_MAIL_READ>,
            null<pbnet::PROBUFF_NET_MAIL_READ_RESPONSE>,
            null<pbnet::PROBUFF_NET_MAIL_DRAW>,
            null<pbnet::PROBUFF_NET_MAIL_DRAW_RESPONSE>,
            null<pbnet::PROBUFF_NET_MAIL_DEL>,
            null<pbnet::PROBUFF_NET_MAIL_DEL_RESPONSE>,
            null<pbnet::PROBUFF_NET_KCPSESSION>,
            null<pbnet::PROBUFF_NET_KCPSESSION_RESPONSE>,
            null<pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD>,
            null<pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD_RESPONSE>,
            null<pbnet::PROBUFF_NET_FAMIL_CREATE>,
            null<pbnet::PROBUFF_NET_FAMIL_CREATE_RESPONSE>,
            null<pbnet::PROBUFF_NET_FAMIL_JOIN>,
            null<pbnet::PROBUFF_NET_FAMIL_JOIN_RESPONSE>,
            null<pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN>,
            null<pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN_RESPONSE>,
            null<pbnet::PROBUFF_NET_FAMIL_CEDE>,
            null<pbnet::PROBUFF_NET_FAMIL_CEDE_RESPONSE>,
            null<pbnet::PROBUFF_NET_FAMIL_LEAVE>,
            null<pbnet::PROBUFF_NET_FAMIL_LEAVE_RESPONSE>,
            null<pbnet::PROBUFF_NET_FAMIL_LIST>,
            null<pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE>,
            null<pbnet::PROBUFF_NET_FAMIL_CHANGENAME>,
            null<pbnet::PROBUFF_NET_FAMIL_CHANGENAME_RESPONSE>,
            null<pbnet::PROBUFF_NET_FAMIL_SIGN>,
            null<pbnet::PROBUFF_NET_FAMIL_SIGN_RESPONSE>,
            null<pbnet::PROBUFF_NET_REWARD_ITEM_RESPONSE>,
            null<pbnet::PROBUFF_NET_RANKLIST>,
            null<pbnet::PROBUFF_NET_RANKLIST_RESPONSE>,
            null<pbnet::PROBUFF_NET_FRIEND>,
            null<pbnet::PROBUFF_NET_FRIEND_RESPONSE>,
            null<pbnet::PROBUFF_NET_FRIEND_ADD>,
            null<pbnet::PROBUFF_NET_FRIEND_ADD_RESPONSE>,
            null<pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD>,
            null<pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD_RESPONSE>,
            null<pbnet::PROBUFF_NET_FRIEND_ERASE>,
            null<pbnet::PROBUFF_NET_FRIEND_ERASE_RESPONSE>,
            null<pbnet::PROBUFF_NET_ROLESTAT>,
            null<pbnet::PROBUFF_NET_SYNC_ATTRIBUTE>,
            null<pbnet::PROBUFF_NET_SYNC_UNIT>,
            null<pbnet::PROBUFF_NET_ENTER_LEAVE_VIEW>,
            null<pbnet::PROBUFF_NET_SYNC_POSITION>,
            null<pbnet::PROBUFF_NET_CHANGE_ANGLE>,
            null<pbexample::PROBUFF_EXAMPLE_PLAY_JOIN>,
            null<pbexample::PROBUFF_EXAMPLE_PLAY_JOIN_RESPONSE>,
            null<pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING>,
            null<pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING_RESULT>,
            null<pbexample::PROBUFF_EXAMPLE_PLAY_PLAYER_CONFIRM>,
            null<pbexample::PROBUFF_EXAMPLE_PLAY_CANCEL>,
            null<pbexample::PROBUFF_EXAMPLE_PLAY_CREATE>,
            null<pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE>,
            null<pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE_RESPONSE>,
            null<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_INFO>,
            null<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER>,
            null<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_ERROR>,
            null<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_BOMB>
        );
   }
}//namespace ngl
