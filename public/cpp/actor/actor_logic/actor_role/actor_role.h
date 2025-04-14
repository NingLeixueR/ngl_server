#pragma once

#include "actor_create.h"
#include "actor_manage.h"
#include "events_logic.h"
#include "db_manage.h"
#include "ndbclient.h"
#include "nprotocol.h"
#include "attribute.h"
#include "autoitem.h"
#include "roleinfo.h"
#include "remakes.h"
#include "db_pool.h"
#include "db_data.h"
#include "ntimer.h"
#include "rolekv.h"
#include "task.h"
#include "cmd.h"
#include "bag.h"
#include "net.h"
#include "db.h"

#include "example.pb.h"
#include "net.pb.h"

namespace ngl
{
	class actor_role : public actor
	{
	public:
		remakes		 m_remakes;
		roleinfo	 m_info;
		bag			 m_bag;
		task		 m_task;
		rolekv		 m_rolekv;
		attribute	 m_attribute;
		i32_serverid m_gatewayid;
		std::pair<pbexample::EPLAY_TYPE, i64_actorid> m_example;
	public:
		actor_role(const actor_role&) = delete;
		actor_role& operator=(const actor_role&) = delete;

		actor_role(i16_area aarea, i32_actordataid aroleid, void* adata);

		virtual i32_serverid get_getwayserverid();

		virtual void init();

		static void nregister();

		virtual ~actor_role() = default;

		virtual void loaddb_finish(bool adbishave);

		//# 执行handle之后调用
		virtual void handle_after();

		virtual void erase_actor_before();

		i64_actorid roleid();

		//# 设置更新角色属性
		void update_attribute(EnumModule amodule, attribute_value& avalue);

		void sync_data_client();

#pragma region forward //转发相关
		enum ecross
		{
			ecross_ordinary			= 1,			// 本服转发
			ecross_cross_ordinary	= 2,			// 跨服转发
			ecross_cross_example	= 3,			// 例子转发
			ecross_none				= 4,			// 错误转发
		};

		//# 重载forward_type来指定转发类型
		template <typename T>
		ecross forward_type(const T& adata)
		{
			return ecross_ordinary;
		}

		//# 聊天的转发类型
		ecross forward_type(const pbnet::PROBUFF_NET_CHAT& adata)
		{
			if (m_info.bantalk())
			{
				return ecross_none;
			}
			switch (adata.m_channelid())
			{
			case pbnet::enum_chat_channel::enum_chat_ordinary:
				return ecross_ordinary;
			case pbnet::enum_chat_channel::enum_chat_cross_ordinary:
				return ecross_cross_ordinary;
			case pbnet::enum_chat_channel::enum_chat_none:
				return ecross_none;
			}
			return ecross_none;
		}

		//# ranklist的转发类型
		ecross forward_type(const pbnet::PROBUFF_NET_RANKLIST& adata)
		{
			return adata.m_iscross() ? ecross_cross_ordinary : ecross_ordinary;
		}

		//# 加入例子小游戏匹配的转发类型
		ecross forward_type(const pbexample::PROBUFF_EXAMPLE_PLAY_JOIN& adata)
		{
			if (m_example.second != 0)
			{
				return ecross_none;
			}
			return adata.m_cross() == pbexample::ECROSS_CROSS_ORDINARY ? ecross_cross_ordinary : ecross_ordinary;
		}

		ecross forward_type(const pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE& adata)
		{
			return adata.m_cross() == pbexample::ECROSS_CROSS_ORDINARY ? ecross_cross_ordinary : ecross_ordinary;
		}

		//# 例子小游戏匹配的转发类型
		ecross example_type()
		{
			return m_example.second == nguid::make() ? ecross_none : ecross_cross_example;
		}

		ecross forward_type(const pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER& adata)
		{
			return example_type();
		}

		//# 重载forward_before来指定转发前事件
		template <typename T>
		bool forward_before(const T& adata)
		{
			return true;
		}

		//# 转发"创建军团"前
		bool forward_before(const pbnet::PROBUFF_NET_CREATE_FAMIL& adata);

		//# 重载dataid来指定转发模块的dataid
		template <typename T>
		int32_t forward_dataid(const T& adata)
		{
			return nguid::none_actordataid();
		}

		template <ENUM_ACTOR ACTOR, typename T>
		bool handle_forward(const message<T>& adata)
		{
			const T& lparm = adata.get_shared_data() == nullptr ? *adata.get_data() : *adata.get_shared_data();
			if (forward_before(lparm) == false)
			{
				return false;
			}
			std::shared_ptr<mforward<T>> pro(nullptr);
			pro = std::make_shared<mforward<T>>(id_guid(), lparm);
			i64_actorid lguid;
			switch (forward_type(lparm))
			{
			case ecross_ordinary:
				lguid = nguid::make(ACTOR, tab_self_area, forward_dataid(lparm));
				break;
			case ecross_cross_ordinary:
				lguid = nguid::make(ACTOR, tab_self_cros_area, forward_dataid(lparm));
				break; 
			case ecross_cross_example:
				lguid = m_example.second;
				break;
			default:
				return true;
			}
			send_actor(lguid, pro);
			return true;
		}
#pragma endregion

		struct prorechange
		{
			std::string m_orderid;
			int32_t m_rechargeid = 0;
			int64_t m_roleid = 0;

			jsonfunc("orderid", m_orderid, "rechargeid", m_rechargeid, "roleid", m_roleid)
		};

		//# 回显给客户端一个字符串
		void echo_msg(const char* amsg);

		int32_t rechange(std::string& aorderid, int32_t arechargeid, bool agm, bool areporting);

		//# 登录请求未发货充值
		void loginpay();

		//# 请求创建订单
		void createorder(std::string& aorder, int32_t arechargeid);

		//# 是否首次充值
		bool is_first_recharge(int32_t arechargeid);

		//# CMD协议
		using handle_cmd = cmd<actor_role, std::string, const pack*, actor_role*, const char*>;
		//# GM协议
		using handle_gm = cmd<actor_role, std::string, int, const ngl::json_read&>;

		//# 定时器
		bool timer_handle(const message<np_timerparm>& adata);

		//# GM请求
		bool handle(const message<mforward<np_gm>>& adata);

		//# CMD请求 
		bool handle(const message<pbnet::PROBUFF_NET_CMD>& adata);

		//# 获取role数据
		bool handle(const message<pbnet::PROBUFF_NET_ROLE_SYNC>& adata);

		//# 获取服务器时间
		bool handle(const message<pbnet::PROBUFF_NET_GET_TIME>& adata);

		//# 切换线路服务器
		bool handle(const message<pbnet::PROBUFF_NET_SWITCH_LINE>& adata);

		//# 发送物品
		bool handle(const message<np_actor_senditem>& adata);

		//# 断开连接
		bool handle(const message<np_actor_disconnect_close>& adata);

		//# 领取任务奖励
		bool handle(const message<pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD>& adata);

		bool handle(const message<np_eevents_logic_rolelogin>& adata);

		//# 同步例子小游戏
		bool handle(const message<np_example_actorid>& adata);
	};
}//namespace ngl

