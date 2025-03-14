 // 注意【makeproto 工具生成文件，不要手动修改】
 // 创建时间【2025-03-14 17:22:25】
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
            null<pbnet::PROBUFF_NET_BAG_UPDATE_RESPONSE>,
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
            null<pbnet::PROBUFF_NET_SYNC_ATTRIBUTE>,
            null<pbnet::PROBUFF_NET_SYNC_UNIT>,
            null<pbnet::PROBUFF_NET_ENTER_LEAVE_VIEW>,
            null<pbnet::PROBUFF_NET_SYNC_POSITION>,
            null<pbnet::PROBUFF_NET_CHANGE_ANGLE>,
            null<pbnet::PROBUFF_NET_MATCHING>,
            null<pbnet::PROBUFF_NET_MATCHING_RESPONSE>,
            null<pbnet::PROBUFF_NET_MATCHING_CANCEL>,
            null<pbnet::PROBUFF_NET_MATCHING_CANCEL_RESPONSE>,
            null<pbnet::PROBUFF_NET_MEMBER_MATCHING_CANCEL>,
            null<pbnet::PROBUFF_NET_MATCHING_WAITCONFIRM>,
            null<pbnet::PROBUFF_NET_MATCHING_CONFIRM>,
            null<pbnet::PROBUFF_NET_MATCHING_CONFIRM_RESPONSE>,
            null<pbnet::PROBUFF_NET_SYNC_MATCHING_CONFIRM>,
            null<pbnet::PROBUFF_NET_MATCHING_SYNC>,
            null<pbnet::PROBUFF_NET_MATCHING_SUCCESS_RESPONSE>,
            null<pbnet::PROBUFF_NET_MATCHING_SUCCESS>,
            null<pbnet::PROBUFF_NET_ENTER_PLAYS>,
            null<pbnet::PROBUFF_NET_KCPSESSION>,
            null<pbnet::PROBUFF_NET_KCPSESSION_RESPONSE>,
            null<pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD>,
            null<pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD_RESPONSE>,
            null<pbnet::PROBUFF_NET_CREATE_FAMIL>,
            null<pbnet::PROBUFF_NET_CREATE_FAMIL_RESPONSE>,
            null<pbnet::PROBUFF_NET_JOIN_FAMIL>,
            null<pbnet::PROBUFF_NET_JOIN_FAMIL_RESPONSE>,
            null<pbnet::PROBUFF_NET_RATIFY_JOIN_FAMIL>,
            null<pbnet::PROBUFF_NET_RATIFY_JOIN_FAMIL_RESPONSE>,
            null<pbnet::PROBUFF_NET_CEDE_FAMIL>,
            null<pbnet::PROBUFF_NET_CEDE_FAMIL_RESPONSE>,
            null<pbnet::PROBUFF_NET_LEAVE_FAMIL>,
            null<pbnet::PROBUFF_NET_LEAVE_FAMIL_RESPONSE>,
            null<pbnet::PROBUFF_NET_FAMIL_LIST>,
            null<pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE>,
            null<pbnet::PROBUFF_NET_CHANGE_FAMILNAME>,
            null<pbnet::PROBUFF_NET_CHANGE_FAMILNAME_RESPONSE>,
            null<pbnet::PROBUFF_NET_FAMILSIGN>,
            null<pbnet::PROBUFF_NET_FAMILSIGN_RESPONSE>,
            null<pbnet::PROBUFF_NET_REWARD_ITEM_RESPONSE>,
            null<pbnet::PROBUFF_NET_RANKLIST>,
            null<pbnet::PROBUFF_NET_RANKLIST_RESPONSE>,
            null<pbnet::PROBUFF_NET_FRIEND>,
            null<pbnet::PROBUFF_NET_FRIEND_RESPONSE>,
            null<pbnet::PROBUFF_NET_ADDFRIEND>,
            null<pbnet::PROBUFF_NET_ADDFRIEND_RESPONSE>,
            null<pbnet::PROBUFF_NET_RATIFY_ADDFRIEND>,
            null<pbnet::PROBUFF_NET_RATIFY_ADDFRIEND_RESPONSE>,
            null<pbnet::PROBUFF_NET_ERASEFRIEND>,
            null<pbnet::PROBUFF_NET_ERASEFRIEND_RESPONSE>,
            null<pbnet::PROBUFF_NET_ROLESTAT>,
            null<pbexample::PROBUFF_NET_EXAMPLE_PLAY_JOIN>,
            null<pbexample::PROBUFF_NET_EXAMPLE_PLAY_JOIN_RESPONSE>,
            null<pbexample::PROBUFF_NET_EXAMPLE_PLAY_MATCHING>,
            null<pbexample::PROBUFF_NET_EXAMPLE_PLAY_MATCHING_RESULT>,
            null<pbexample::PROBUFF_NET_EXAMPLE_PLAY_PLAYER_CONFIRM>,
            null<pbexample::PROBUFF_NET_EXAMPLE_PLAY_CANCEL>
        );
   }
}//namespace ngl
