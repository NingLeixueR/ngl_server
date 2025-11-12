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
#pragma once 

#include "nprotocol_template.h"
#include "ttab_servers.h"
#include "pb_field.h"
#include "nguid.h"
#include "actor.h"

namespace ngl
{
	class nsp_regload
	{
		std::map<i16_area, i64_actorid>	 m_nspserver;
		std::map<i16_area, bool> m_register;
		std::map<i16_area, bool> m_loadfinish;
	public:
		void init(i64_actorid aactorid);

		// 设置[area]完成注册
		void set_register(i16_area aarea);

		// [area]是否完成注册
		bool is_register(i16_area aarea)const;

		// 所有[area]是否都完成注册
		bool is_register()const;

		// 设置[area]加载数据完成
		void set_loadfinish(i16_area aarea);

		// [area]是否加载数据完成
		bool is_loadfinish(i16_area aarea)const;

		// 所有[area]是否都加载数据完成
		bool is_loadfinish()const;

		// 根据[area]获取nsp server
		i64_actorid nspserid(i16_area aarea)const;

		// 遍历所有需要连接的nsp server
		void foreach_nspser(const std::function<void(i16_area, i64_actorid)>& afun)const;
	};

	// 对"数据字段"进行封装
	class operator_field
	{
		std::map<i16_actortype, std::map<i32_fieldnumber, epb_field>> m_node_fieldnumbers;
	public:

		void set_field(i16_actortype atype, const std::map<i32_fieldnumber, epb_field>& anode_fieldnumbers);
		// # 设置
		void set_field(const std::map<i16_actortype, std::map<i32_fieldnumber, epb_field>>& anode_fieldnumbers);

		// # 添加
		void add_field(i16_actortype atype, i32_fieldnumber afieldnumber, epb_field afieldtype);

		// # 根据类型获取
		std::map<i32_fieldnumber, epb_field>* get_field(i16_actortype atype);

		// # 根据字段类型进行数据拷贝
		template <typename T>
		bool field_copy(i16_actortype atype, const T& asource, T& atarget)
		{
			auto itor = m_node_fieldnumbers.find(atype);
			if (itor == m_node_fieldnumbers.end())
			{
				tools::no_core_dump();
				return false;
			}
			pb_field::copy(asource, &atarget, itor->second);
			return true;
		}

		std::map<i16_actortype, std::map<i32_fieldnumber, epb_field>>& field_numbers();
	};

	class care_data
	{
		nsp_care m_core;
	public:
		void init(bool aread);

		// # "部分读" 构造
		void init(const std::set<i64_actorid>& aids);

		// # "部分读,部分写" 构造
		void init(const std::set<i64_actorid>& areadids, const std::set<i64_actorid>& awriteids);

		// # "全部读,部分写" 构造
		void init(bool aread, const std::set<i64_actorid>& awriteids);

		void init(const nsp_care& acore);

		// # 是否关心
		bool is_care(i64_actorid adataid)const;

		// # 是否可读
		bool is_read(i64_actorid adataid)const;

		// # 是否可写(既然可写 当然可读)
		bool is_write(i64_actorid adataid)const;

		// # 是否可读全部数据
		bool is_readall()const;

		// # 是否可写全部数据
		bool is_writeall()const;

		// # 可读列表
		std::set<i64_actorid>& readids();

		// # 可写列表
		std::set<i64_actorid>& writeids();

		const nsp_care& get_core()const;
	};

	template <typename T>
	class nsp_instance
	{
	private:
		static std::map<i64_actorid, std::shared_ptr<T>>	m_instance;
		static std::mutex									m_mutex;
	public:
		static T* nclient(i64_actorid aactorid)
		{
			monopoly_lock(m_mutex);
			auto itor = m_instance.find(aactorid);
			if (itor == m_instance.end())
			{
				tools::no_core_dump();
				return nullptr;
			}
			return itor->second.get();
		}

		static T* init(i64_actorid aactorid, std::shared_ptr<T>& athis)
		{
			monopoly_lock(m_mutex);
			if (m_instance.contains(aactorid))
			{
				tools::no_core_dump();
			}
			m_instance[aactorid] = athis;
			return athis.get();
		}

		static void exit(i64_actorid aactorid)
		{
			monopoly_lock(m_mutex);
			m_instance.erase(aactorid);
		}

		template <typename TDerived, typename TMESSAGE>
		static void register_handle()
		{
			actor::register_actor_s<TDerived, TMESSAGE>([](TDerived* aacotor, const message<TMESSAGE>& adata)
				{
					T* lpclass = nclient(aacotor->id_guid());
					if (lpclass != nullptr)
					{
						lpclass->handle(aacotor, adata);
					}
				}, false);
		}
	};

	template <typename T>
	std::map<i64_actorid, std::shared_ptr<T>> nsp_instance<T>::m_instance;

	template <typename T>
	std::mutex nsp_instance<T>::m_mutex;

	template <typename T>
	class nsp_callback
	{
		template <typename TDATA>
		struct tcallback
		{
			std::function<void(int64_t, const TDATA&, bool)>				m_changedatafun = nullptr;	// [回调] 当数据发生变化
			std::function<void(int64_t)>									m_deldatafun = nullptr;		// [回调] 当数据被删除
			std::function<void()>											m_loadfinishfun = nullptr;	// [回调] 数据加载完成
		};
		tcallback<T> m_call;
	public:
		void set_changedatafun(const std::function<void(int64_t, const T&, bool)>& afun)
		{
			m_call.m_changedatafun = afun;
		}

		void set_deldatafun(const std::function<void(int64_t)>& afun)
		{
			m_call.m_deldatafun = afun;
		}

		void set_loadfinishfun(const std::function<void()>& afun)
		{
			m_call.m_loadfinishfun = afun;
		}

		void changedatafun(int64_t aid, const T& adata, bool afrist)
		{
			if (m_call.m_changedatafun != nullptr)
			{
				m_call.m_changedatafun(aid, adata, afrist);
			}
		}

		void deldatafun(int64_t aid)
		{
			if (m_call.m_deldatafun != nullptr)
			{
				m_call.m_deldatafun(aid);
			}
		}

		void loadfinishfun()
		{
			if (m_call.m_loadfinishfun != nullptr)
			{
				m_call.m_loadfinishfun();
			}
		}
	};

	template <typename TDerived>
	class nsp_handle_print
	{
	public:
		template <typename TRECV>
		static void print(const char* aname, TDerived* aactor, const TRECV* arecv)
		{
			TRECV* lrecv = (TRECV*)arecv;

			std::string lcustomstr;
			tools::custom2json(*lrecv, lcustomstr);
			log_error_net()->print(
				"{}::handle<{}>( actor({}) : {} )"
				, aname
				, tools::type_name<TRECV>()
				, nguid(aactor->id_guid())
				, lcustomstr
			);
		}

		template <typename TACTOR, typename TX>
		static void msg_info(TX& adata)
		{
			adata.m_msg = std::format(
				"{}:{}:{}"
				, tools::type_name<TDerived>()
				, tools::type_name<TACTOR>()
				, tools::type_name<typename TX::T>()
			);
		}
	};


}//namespace ngl