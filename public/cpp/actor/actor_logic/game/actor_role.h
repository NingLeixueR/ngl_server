#pragma once

#include "actor_create.h"
#include "actor_manage.h"
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
#include "net.pb.h"
#include "rolekv.h"
#include "task.h"
#include "cmd.h"
#include "bag.h"
#include "net.h"
#include "db.h"

namespace ngl
{
	class actor_manage_role;
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
		i64_actorid  m_playactorid;
	public:
		actor_role(const actor_role&) = delete;
		actor_role& operator=(const actor_role&) = delete;

		actor_role(i16_area aarea, i32_actordataid aroleid, void* adata);

		virtual i32_serverid get_getwayserverid();

		virtual void init();

		//# 注册需要处理的协议
		static void nregister();

		virtual ~actor_role();

		virtual void loaddb_finish(bool adbishave);

		using handle_php = cmd<actor_role, std::string, int, ngl::json_read&>;

		//# 执行handle之后调用
		virtual void handle_after();

		i64_actorid roleid();

		//# 设置更新角色属性
		void update_attribute(EnumModule amodule, attribute_value& avalue)
		{
			m_attribute.updata(amodule, avalue);
		}

		void sync_data_client();

		enum ecross
		{
			ecross_ordinary,			// 本服转发
			ecross_cross_ordinary,		// 跨服转发
			ecross_none,				// 错误转发
		};

		//# 重载forward_type来指定转发类型
		template <typename T>
		ecross forward_type(T& adata)
		{
			return ecross_ordinary;
		}

		ecross forward_type(pbnet::PROBUFF_NET_CHAT& adata)
		{
			int lnow = localtime::gettime();
			if (lnow < m_info.notalkutc())
			{

				auto pro = std::make_shared<pbnet::PROBUFF_NET_ERROR>();
				pro->set_m_errmessage("no talk!!!!");
				send_client(id_guid(), pro);
				return ecross_none;
			}
			return adata.m_channelid() == 2 ? ecross_cross_ordinary : ecross_ordinary;
		}

		//# 重载forward_before来指定转发前事件
		template <typename T>
		void forward_before(T& adata)
		{
		}

		//# 转发"创建军团"前
		void forward_before(pbnet::PROBUFF_NET_CREATE_FAMIL& adata);

		//# 重载dataid来指定转发模块的dataid
		template <typename T>
		int32_t forward_dataid(T& adata)
		{
			return nguid::none_actordataid();
		}

		template <ENUM_ACTOR ACTOR, typename T>
		bool handle_forward(message<T>& adata)
		{
			std::shared_ptr<mforward<T>> pro(nullptr);
			if (adata.get_shared_data() == nullptr)
			{
				pro = std::make_shared<mforward<T>>(id_guid(), *adata.get_data());
			}
			else
			{
				pro = std::make_shared<mforward<T>>(id_guid(), adata.get_shared_data());
			}
			i64_actorid lguid;
			switch (forward_type(*adata.get_data()))
			{
			case ecross_ordinary:
				lguid = nguid::make(ACTOR, ttab_servers::tab()->m_area, forward_dataid(*adata.get_data()));
				break;
			case ecross_cross_ordinary:
				lguid = nguid::make(ACTOR, ttab_servers::tab()->m_crossarea, forward_dataid(*adata.get_data()));
				break; 
			default:
				return true;
			}
			send_actor(lguid, pro);
			return true;
		}

		//# 登录请求未发货充值
		void loginpay();

		//# CMD 协议
		using handle_cmd = cmd<actor_role, std::string, actor_role*, const char*>;
		bool handle(message<pbnet::PROBUFF_NET_CMD>& adata);

		//# 获取role数据
		bool handle(message<pbnet::PROBUFF_NET_ROLE_SYNC>& adata);

		//# 获取服务器时间
		bool handle(message<pbnet::PROBUFF_NET_GET_TIME>& adata);

		//# 切换线路服务器
		bool handle(message<pbnet::PROBUFF_NET_SWITCH_LINE>& adata);

		//# 发送物品
		bool handle(message<np_actor_senditem>& adata);

		//# 断开连接
		bool handle(message<np_actor_disconnect_close>& adata);

		//# 玩法创建成功  记录玩法actorid
		bool handle(message<pbnet::PROBUFF_NET_MATCHING_SUCCESS_RESPONSE>& adata);

		//# 领取任务奖励
		bool handle(message<pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD>& adata);

		int32_t rechange(std::string& aorderid, int32_t arechargeid, bool agm, bool areporting);

		//# 请求创建订单
		void createorder(std::string& aorder, int32_t arechargeid);

		//# gm请求
		bool handle(message<mforward<np_gm>>& adata);

		//# 是否首次充值
		bool is_first_recharge(int32_t arechargeid);

		//# 定时器
		bool timer_handle(message<timerparm>& adata);
	};
}//namespace ngl

