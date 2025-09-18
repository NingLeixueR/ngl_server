#pragma once

#include "nprotocol.h"
#include "naddress.h"
#include "actor.h"

namespace ngl
{
	//####################################################################
	//# ÿ�����̶���Ҫ��һ��actor_client��actor_server
	//# ͨ��һ������ֻ��һ�������а���actor_server 
	//# �������н��̶��������actor_client
	//# actor_client ������¼����nguid�������id�Ķ�Ӧ��ϵ��
	//# actor_client�൱��actor��ܵ�·������Ϊactor�ṩ·�ɹ���
	//# ��actor_server����ַ�����actor_client�б��ص�nguid��Ӧ��ϵ
	//###################################################################
	class actor_client : 
		public actor
	{
		actor_client(const actor_client&) = delete;
		actor_client& operator=(const actor_client&) = delete;

		std::map<i32_serverid, std::list<std::function<void()>>>	m_connectfun;			// ����[ָ��������]�ɹ���ִ��
		std::set<i32_serverid>										m_connectserverid;		// �����ӵķ�����id

		actor_client();
		virtual ~actor_client();
	public:
		friend class actor_instance<actor_client>;
		static actor_client& instance() 
		{ 
			return actor_instance<actor_client>::instance(); 
		}
		
		//# ע����Ҫ�������Ϣ
		static void nregister();
	private:
		//# ������actor_serverע��actor_client
		void actor_server_register(i32_serverid aserverid);

		//# �������Ӻ�ʱ�� ����actor_dbclient��ע�����Ӻ��¼����������ݣ�
		void set_connect_fnish(i32_serverid aserverid);

		//# ���ӳɹ������
		void connect_fnish();

		//# ��������
		void activ_connect(i32_serverid aserverid);
	public:
		//# ��actor_serverע����
		void actor_server_register();

		//# actor_client��actorid
		static i64_actorid actorid();

		//# net����actor server�ɹ�
		bool handle(const message<np_connect_actor_server>& adata);

		//# ע����
		bool handle(const message<np_actor_server_register>& adata);

		//# actor_server�ظ�ע����
		bool handle(const message<np_actornode_register_response>& adata);

		//# actor�ͻ��˼��໥����
		bool handle(const message<np_actorclient_node_connect>& adata);

		//# ��������actor�ͻ���ͬ�������Ϣ
		bool handle(const message<np_actornode_update>& adata);

		//# ��actor�ͻ���ͬ�������Ϣ(Ⱥ��)
		bool handle(const message<np_actornode_update_mass>& adata);

		//# ���ӳɹ���ִ������
		bool handle(const message<np_actornode_connect_task>& adata);

		//# ͬ��actor_role��Ӧ��gateway
		bool handle(const message<np_actor_gatewayid_updata>& adata);
	};
}//namespace ngl
