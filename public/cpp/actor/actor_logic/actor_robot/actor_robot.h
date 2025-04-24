#pragma once

#include "actor_manage.h"
#include "ndbclient.h"
#include "nprotocol.h"
#include "db_manage.h"
#include "db_data.h"
#include "db_pool.h"
#include "net.h"
#include "db.h"

#include "example.pb.h"
#include "net.pb.h"

namespace ngl
{
	class actor_robot : public actor
	{
		// ----- Data Begin -----
		pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE	m_data;
		std::string								m_kcpsessionmd5;
	public:
		i32_session								m_session;
		int16_t									m_kcp;
		// ----- Data End   -----
	public:
		actor_robot() = delete;
		actor_robot(const actor_robot&) = delete;
		actor_robot& operator=(const actor_robot&) = delete;

		actor_robot(i16_area aarea, i32_actordataid arobotid, void*);

		virtual ~actor_robot() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid(int32_t adataid);

		virtual void init();

		virtual void loaddb_finish(bool adbishave);

		static void nregister();

		virtual const char* kcp_session();

		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<np_arg_null>&);
		bool handle(const message<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_BOMB>& adata);
		bool handle(const message<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_ERROR>& adata);
		// 匹配结果
		bool handle(const message<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_INFO>& adata);
		// 创建例子游戏成功
		bool handle(const message<pbexample::PROBUFF_EXAMPLE_PLAY_CREATE>& adata);
		bool handle(const message<pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE_RESPONSE>& adata);
		bool handle(const message<pbexample::PROBUFF_EXAMPLE_PLAY_JOIN_RESPONSE>& adata);
		// 同步匹配信息
		bool handle(const message<pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING>& adata);
		// 匹配结果
		bool handle(const message<pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING_RESULT>& adata);
		// [回复]添加好友
		bool handle(const message<pbnet::PROBUFF_NET_ADDFRIEND_RESPONSE>& adata);
		// [回复]获取背包信息
		bool handle(const message<pbnet::PROBUFF_NET_BAG_SYNC_RESPONSE>& adata);
		// [回复]更新背包信息
		bool handle(const message<pbnet::PROBUFF_NET_BAG_UPDATE>& adata);
		// [回复]军团长转让军团
		bool handle(const message<pbnet::PROBUFF_NET_CEDE_FAMIL_RESPONSE>& adata);
		// [回复]修改军团名称
		bool handle(const message<pbnet::PROBUFF_NET_CHANGE_FAMILNAME_RESPONSE>& adata);
		// [回复]聊天
		bool handle(const message<pbnet::PROBUFF_NET_CHAT_RESPONSE>& adata);
		// [回复]通知客户端充值发货
		bool handle(const message<pbnet::PROBUFF_NET_DELIVER_GOODS_RECHARGE>& adata);
		// 进入视野
		bool handle(const message<pbnet::PROBUFF_NET_ENTER_LEAVE_VIEW>& adata);
		// [回复]删除好友
		bool handle(const message<pbnet::PROBUFF_NET_ERASEFRIEND_RESPONSE>& adata);
		// [发送]错误码
		bool handle(const message<pbnet::PROBUFF_NET_ERROR>& adata);
		// [回复]军团签到
		bool handle(const message<pbnet::PROBUFF_NET_FAMILSIGN_RESPONSE>& adata);
		// [回复]创建军团
		bool handle(const message<pbnet::PROBUFF_NET_FAMIL_CREATE_RESPONSE>& adata);
		// [回复]加入军团
		bool handle(const message<pbnet::PROBUFF_NET_FAMIL_JOIN_RESPONSE>& adata);
		// [回复]获取军团列表
		bool handle(const message<pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE>& adata);
		// [回复]添加好友
		bool handle(const message<pbnet::PROBUFF_NET_FRIEND_RESPONSE>& adata);
		// [回复]服务器时间
		bool handle(const message<pbnet::PROBUFF_NET_GET_TIME_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_JOIN_FAMIL_RESPONSE>& adata);
		// [回复]kcp-session
		bool handle(const message<pbnet::PROBUFF_NET_KCPSESSION_RESPONSE>& adata);
		// [回复]离开军团
		bool handle(const message<pbnet::PROBUFF_NET_LEAVE_FAMIL_RESPONSE>& adata);
		// [回复]领取邮件附件
		bool handle(const message<pbnet::PROBUFF_NET_MAIL_DEL_RESPONSE>& adata);
		// [回复]领取邮件附件
		bool handle(const message<pbnet::PROBUFF_NET_MAIL_DRAW_RESPONSE>& adata);
		// [回复]获取邮件列表
		bool handle(const message<pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE>& adata);
		// [回复]读取邮件
		bool handle(const message<pbnet::PROBUFF_NET_MAIL_READ_RESPONSE>& adata);
		// [回复] 服务器用来回复端，一般用于cmd协议，用来返回一个字符串回显
		bool handle(const message<pbnet::PROBUFF_NET_MSG_RESPONSE>& adata);
		// [回复]获取公告列表
		bool handle(const message<pbnet::PROBUFF_NET_NOTICE_RESPONSE>& adata);
		// [回复]获取排行榜
		bool handle(const message<pbnet::PROBUFF_NET_RANKLIST_RESPONSE>& adata);
		// [回复]同意添加好友
		bool handle(const message<pbnet::PROBUFF_NET_RATIFY_ADDFRIEND_RESPONSE>& adata);
		// [回复]军团长同意/不同意玩家加入军团
		bool handle(const message<pbnet::PROBUFF_NET_RATIFY_JOIN_FAMIL_RESPONSE>& adata);
		// [回复]充值
		bool handle(const message<pbnet::PROBUFF_NET_RECHARGE_RESPONSE>& adata);
		// [回复]恭喜获得x物品
		bool handle(const message<pbnet::PROBUFF_NET_REWARD_ITEM_RESPONSE>& adata);
		// [回复]玩家上线
		bool handle(const message<pbnet::PROBUFF_NET_ROLESTAT>& adata);
		// [回复]获取角色信息
		bool handle(const message<pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE>& adata);
		// [回复]切换线路
		bool handle(const message<pbnet::PROBUFF_NET_SWITCH_LINE_RESPONSE>& adata);
		// 同步属性值
		bool handle(const message<pbnet::PROBUFF_NET_SYNC_ATTRIBUTE>& adata);
		// 同步位置方向信息
		bool handle(const message<pbnet::PROBUFF_NET_SYNC_POSITION>& adata);
		// 同步玩家信息
		bool handle(const message<pbnet::PROBUFF_NET_SYNC_UNIT>& adata);
		// [回复]领取任务奖励
		bool handle(const message<pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD_RESPONSE>& adata);
	};
}//namespace ngl
	
