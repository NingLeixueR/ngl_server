 // 注意【makeproto 工具生成文件，不要手动修改】
 // 创建时间【2026-01-16 18:55:07】
#include "nprotocol.h"
#include "tprotocol.h"
#include "actor.h"

#include "example.pb.h"
#include "net.pb.h"

#include <map>

namespace ngl
{
   template <typename PB>
   void hrole_forward_pb()
   {
       int32_t lprotocolnum = tprotocol::protocol<PB>();
       tprotocol::tp_customs<
           ngl::np_actor_forward<PB, forward_g2c<PB>>
           , ngl::np_actor_forward<PB, forward_c2g<PB>>
           , ngl::np_actor_forward<PB, forward_g2c<forward>>
           , ngl::np_actor_forward<PB, forward_c2g<forward>>
           , np_actormodule_forward<PB>
       >(lprotocolnum, 0);
   }
   template <typename ...ARG>
   void help_role_forward_pb()
   {
      (hrole_forward_pb<ARG>(), ...);
   }
   void tprotocol_forward_pb()
   {
        help_role_forward_pb<
            pbnet::PROBUFF_NET_MSG_RESPONSE,
            pbnet::PROBUFF_NET_HEARTBEAT,
            pbnet::PROBUFF_NET_RECHARGE,
            pbnet::PROBUFF_NET_RECHARGE_RESPONSE,
            pbnet::PROBUFF_NET_DELIVER_GOODS_RECHARGE,
            pbnet::PROBUFF_NET_ERROR,
            pbnet::PROBUFF_NET_GET_TIME,
            pbnet::PROBUFF_NET_GET_TIME_RESPONSE,
            pbnet::PROBUFF_NET_NOTICE,
            pbnet::PROBUFF_NET_NOTICE_RESPONSE,
            pbnet::PROBUFF_NET_ACOUNT_LOGIN,
            pbnet::PROBUFF_NET_ACOUNT_LOGIN_RESPONSE,
            pbnet::PROBUFF_NET_ROLE_LOGIN,
            pbnet::PROBUFF_NET_ROLE_NOT_CREATE,
            pbnet::PROBUFF_NET_ROLE_CREATE,
            pbnet::PROBUFF_NET_ROLE_CREATE_RESPONSE,
            pbnet::PROBUFF_NET_ROLE_SYNC,
            pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE,
            pbnet::PROBUFF_NET_BAG_SYNC,
            pbnet::PROBUFF_NET_BAG_SYNC_RESPONSE,
            pbnet::PROBUFF_NET_BAG_UPDATE,
            pbnet::PROBUFF_NET_CMD,
            pbnet::PROBUFF_NET_CHAT,
            pbnet::PROBUFF_NET_CHAT_RESPONSE,
            pbnet::PROBUFF_NET_SWITCH_LINE,
            pbnet::PROBUFF_NET_SWITCH_LINE_RESPONSE,
            pbnet::PROBUFF_NET_MAIL_LIST,
            pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE,
            pbnet::PROBUFF_NET_MAIL_READ,
            pbnet::PROBUFF_NET_MAIL_READ_RESPONSE,
            pbnet::PROBUFF_NET_MAIL_DRAW,
            pbnet::PROBUFF_NET_MAIL_DRAW_RESPONSE,
            pbnet::PROBUFF_NET_MAIL_DEL,
            pbnet::PROBUFF_NET_MAIL_DEL_RESPONSE,
            pbnet::PROBUFF_NET_KCPSESSION,
            pbnet::PROBUFF_NET_KCPSESSION_RESPONSE,
            pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD,
            pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD_RESPONSE,
            pbnet::PROBUFF_NET_FAMIL_CREATE,
            pbnet::PROBUFF_NET_FAMIL_CREATE_RESPONSE,
            pbnet::PROBUFF_NET_FAMIL_INFO,
            pbnet::PROBUFF_NET_FAMIL_INFO_RESPONSE,
            pbnet::PROBUFF_NET_FAMIL_JOIN,
            pbnet::PROBUFF_NET_FAMIL_JOIN_RESPONSE,
            pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN,
            pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN_RESPONSE,
            pbnet::PROBUFF_NET_FAMIL_CEDE,
            pbnet::PROBUFF_NET_FAMIL_CEDE_RESPONSE,
            pbnet::PROBUFF_NET_FAMIL_LEAVE,
            pbnet::PROBUFF_NET_FAMIL_LEAVE_RESPONSE,
            pbnet::PROBUFF_NET_FAMIL_LIST,
            pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE,
            pbnet::PROBUFF_NET_FAMIL_CHANGENAME,
            pbnet::PROBUFF_NET_FAMIL_CHANGENAME_RESPONSE,
            pbnet::PROBUFF_NET_FAMIL_SIGN,
            pbnet::PROBUFF_NET_FAMIL_SIGN_RESPONSE,
            pbnet::PROBUFF_NET_REWARD_ITEM_RESPONSE,
            pbnet::PROBUFF_NET_RANKLIST,
            pbnet::PROBUFF_NET_RANKLIST_RESPONSE,
            pbnet::PROBUFF_NET_FRIEND,
            pbnet::PROBUFF_NET_FRIEND_RESPONSE,
            pbnet::PROBUFF_NET_FRIEND_ADD,
            pbnet::PROBUFF_NET_FRIEND_ADD_RESPONSE,
            pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD,
            pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD_RESPONSE,
            pbnet::PROBUFF_NET_FRIEND_ERASE,
            pbnet::PROBUFF_NET_FRIEND_ERASE_RESPONSE,
            pbnet::PROBUFF_NET_ROLESTAT,
            pbnet::PROBUFF_NET_SYNC_ATTRIBUTE,
            pbnet::PROBUFF_NET_SYNC_UNIT,
            pbnet::PROBUFF_NET_ENTER_LEAVE_VIEW,
            pbnet::PROBUFF_NET_SYNC_POSITION,
            pbnet::PROBUFF_NET_CHANGE_ANGLE,
            pbnet::PROBUFF_NET_TESTLUA,
            pbexample::PROBUFF_EXAMPLE_PLAY_JOIN,
            pbexample::PROBUFF_EXAMPLE_PLAY_JOIN_RESPONSE,
            pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING,
            pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING_RESULT,
            pbexample::PROBUFF_EXAMPLE_PLAY_PLAYER_CONFIRM,
            pbexample::PROBUFF_EXAMPLE_PLAY_CANCEL,
            pbexample::PROBUFF_EXAMPLE_PLAY_CREATE,
            pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE,
            pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE_RESPONSE,
            pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_INFO,
            pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER,
            pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_ERROR,
            pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_BOMB
        >();
   }
}//namespace ngl
