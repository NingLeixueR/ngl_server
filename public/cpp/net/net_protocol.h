#pragma once

#include "server_session.h"
#include "netbuff_pool.h"
#include "handle_pram.h"
#include "structbytes.h"
#include "actor_base.h"
#include "net_pack.h"
#include "asio_tcp.h"
#include "asio_kcp.h"
#include "segpack.h"
#include "impl.h"
#include "pack.h"

#include <vector>
#include <memory>


namespace ngl
{
	class net_protocol
	{
		net_protocol() = delete;
		net_protocol(const net_protocol&) = delete;
		net_protocol& operator=(const net_protocol&) = delete;

	protected:
		struct impl_net_protocol;
		ngl::impl<impl_net_protocol>	m_impl_net_protocol;
		int8_t							m_index;

		net_protocol(int8_t aindex);

		bool socket_recv(int asessionid, int aislanip, const char* abuff, int32_t abufflen);
	public:
		~net_protocol();

		bpool& get_pool();

		// # ��ʼ��net_protocol
		// # aport			i16_port		�˿ں�
		// # athreadnum		i32_threadsize 	�߳���
		// # aouternet		bool			�Ƿ������������������
		virtual bool init(i16_port aport, i32_threadsize athreadnum, bool aouternet);

		// # �ر�socket�����Լ����ص�����
		// # ֪ͨ�ϲ�Ӧ��
		virtual void close(i32_sessionid asession);

		// # �߼��������ر�����(�����Ͳ���Ҫ֪ͨ�ϲ�Ӧ��)
		virtual void close_net(i32_sessionid asession) = 0;

		// # ������Ϣ
		virtual bool net_send(i32_sessionid asession, std::shared_ptr<pack>& lpack) = 0;
		virtual bool net_send(i32_sessionid asession, std::shared_ptr<void>& lpack) = 0;

		//## �������Ƿ���ڴ�session id
		virtual bool exist_session(i32_sessionid asession) = 0;

		// # ��ȡ�߳�����
		i32_threadsize socketthreadnum();

		// # ��ȡ�����˿ں�
		i16_port port();

		// # ����pack
		bool sendpack(i32_sessionid asession, std::shared_ptr<pack>& apack);
		bool sendpack(i32_sessionid asession, std::shared_ptr<void>& apack);

		// # ��ĳ������������pack
		bool sendpackbyserver(i32_serverid aserverid, std::shared_ptr<pack>& apack);

		virtual void set_close(int asession, const std::string& aip, i16_port aport, const std::function<void(i32_sessionid)>& afun) = 0;

		virtual bool connect(const std::string& aip, i16_port aport, const std::function<void(i32_sessionid)>& afun) = 0;

		// # aip/aport		Ҫ���ӵ�ip/�˿�
		// # afun			���ӻص�
		// # await			�Ƿ�ȴ����ӳɹ�
		// # areconnection	�����Ƿ�����
		virtual bool connect(const std::string& aip, i16_port aport, const std::function<void(i32_sessionid)>& afun, bool await, bool areconnection);

		// # ������Ϣ
		template <typename Y, typename T = Y>
		bool send(i32_sessionid asession, const Y& adata, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			std::shared_ptr<pack> lpack = net_pack<T>::npack(&get_pool(), adata, aactorid, arequestactorid);
			if (lpack == nullptr)
			{
				return false;
			}
			if (sendpack(asession, lpack) == false)
			{
				return false;
			}
			return true;
		}

		template <typename Y, typename T = Y>
		bool sendbyserver(i32_serverid aserverid, const Y& adata, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			i32_sessionid lsession = server_session::sessionid(aserverid);
			if (lsession == -1)
			{
				return false;
			}
			return send<Y, T>(lsession, adata, aactorid, arequestactorid);
		}

		// # ��һ��sesion������Ϣ
		// # key: session values:aactorid
		// # std::map<uint32_t, uint32_t>& asession
		bool sendmore(
			const std::map<i32_sessionid, i64_actorid>& asession, 
			i64_actorid aactorid, 
			std::shared_ptr<pack>& apack
		);

		bool sendmore(
			const std::set<i32_sessionid>& asession,
			i64_actorid aactorid,
			i64_actorid arequestactorid,
			std::shared_ptr<pack>& apack
		);

		template <typename T>
		bool send_client(i32_actordataid auid, i16_area aarea, i32_gatewayid agateway, T& adata)
		{
			np_actor_forward<T, forward_g2c<forward>> pro;
			pro.m_data.m_uid.push_back(auid);
			pro.m_data.m_area.push_back(aarea);

			forward& lforward = pro.m_data.m_data;

			ngl::ser::serialize_byte lserializebyte;
			ngl::ser::nserialize::bytes(&lserializebyte, adata);

			lforward.m_bufflen = lserializebyte.pos();
			lforward.m_buff = netbuff_pool::instance().malloc_private(lforward.m_bufflen);

			ngl::ser::serialize_push lserializepush(lforward.m_buff, lforward.m_bufflen);
			if (ngl::ser::nserialize::push(&lserializepush, adata))
			{
				if (agateway != 0)
				{
					i32_session lsession = server_session::sessionid(agateway);
					if (lsession > 0)
					{
						send(lsession, pro, nguid::make(), nguid::make());
						netbuff_pool::instance().free((char*)lforward.m_buff);
						return true;
					}
				}
			}
			netbuff_pool::instance().free((char*)lforward.m_buff);
			return true;
		}

		template <typename T>
		void send_client(const std::vector<std::pair<i32_actordataid, i16_area>>& avec, i32_gatewayid agateway, T& adata)
		{
			np_actor_forward<T, forward_g2c<forward>> pro;
			for (int i = 0; i < avec.size(); ++i)
			{
				pro.m_data.m_uid.push_back(avec[i].first);
				pro.m_data.m_area.push_back(avec[i].second);
			}

			forward& lforward = pro.m_data.m_data;

			ngl::ser::serialize_byte lserializebyte;
			ngl::ser::nserialize::bytes(&lserializebyte, adata);

			lforward.m_bufflen = lserializebyte.pos();
			lforward.m_buff = netbuff_pool::instance().malloc_private(lforward.m_bufflen);

			ngl::ser::serialize_push lserializepush(lforward.m_buff, lforward.m_bufflen);
			if (ngl::ser::nserialize::push(&lserializepush, adata))
			{
				if (agateway != 0)
				{
					i32_session lsession = server_session::sessionid(agateway);
					if (lsession > 0)
					{
						send(lsession, pro, nguid::make(), nguid::make());
						netbuff_pool::instance().free((char*)lforward.m_buff);
						return;
					}
				}
			}
			netbuff_pool::instance().free((char*)lforward.m_buff);
		}
	};
}// namespace ngl