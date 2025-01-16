#pragma once

#include "server_session.h"
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
	class forward_pack
	{
		forward_pack() = delete;
		forward_pack(const forward_pack&) = delete;
		forward_pack& operator=(const forward_pack&) = delete;

	public:
		template <typename Y>
		static std::shared_ptr<pack>& get_pack(Y&)
		{
			static std::shared_ptr<pack> lnull = nullptr;
			return lnull;
		}

		template <typename Y>
		static std::shared_ptr<pack>& get_pack(np_actor_forward<Y, EPROTOCOL_TYPE_CUSTOM, true, ngl::forward>& adata)
		{
			return adata.m_recvpack;
		}

		template <typename Y>
		static std::shared_ptr<pack>& get_pack(np_actor_forward<Y, EPROTOCOL_TYPE_PROTOCOLBUFF, true, ngl::forward>& adata)
		{
			return adata.m_recvpack;
		}

		template <typename Y>
		static std::shared_ptr<pack>& get_pack(np_actor_forward<Y, EPROTOCOL_TYPE_CUSTOM, false, ngl::forward>& adata)
		{
			return adata.m_recvpack;
		}

		template <typename Y>
		static std::shared_ptr<pack>& get_pack(np_actor_forward<Y, EPROTOCOL_TYPE_PROTOCOLBUFF, false, ngl::forward>& adata)
		{
			return adata.m_recvpack;
		}
	};

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
		template <typename T>
		bool send(i32_sessionid asession, T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
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
			std::shared_ptr<pack>& lpack2 = forward_pack::get_pack(adata);
			if (lpack2 == nullptr)
			{
				return true;
			}
			if (encryption_bytexor::check_xor(lpack2))
			{
				ngl::tools::bytexor(lpack2->m_buff, lpack2->m_len, lpack->m_len - pack_head::size());
			}
			if (sendpack(asession, lpack2) == false)
			{
				return false;
			}
			return true;
		}

		template <typename T>
		bool sendbyserver(i32_serverid aserverid, T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			i32_sessionid lsession = server_session::sessionid(aserverid);
			if (lsession == -1)
			{
				return false;
			}
			return send(lsession, adata, aactorid, arequestactorid);
		}

		template <typename T>
		static std::pair<std::shared_ptr<pack>, std::shared_ptr<pack>> more_pack(T& adata, i64_actorid aactorid);

		// # ��һ��sesion������Ϣ
		// # key: session values:aactorid
		// # std::map<uint32_t, uint32_t>& asession
		bool sendmore(const std::map<i32_sessionid, i64_actorid>& asession, i64_actorid aactorid, std::pair<std::shared_ptr<pack>, std::shared_ptr<pack>>& apair);
	private:
		template <typename TSTL>
		bool sendmore_stl(const TSTL& asession, i64_actorid aactorid, i64_actorid arequestactorid, std::pair<std::shared_ptr<pack>, std::shared_ptr<pack>>& apair);
	public:
		bool sendmore(const std::vector<i32_sessionid>& asession, i64_actorid aactorid, i64_actorid arequestactorid, std::pair<std::shared_ptr<pack>, std::shared_ptr<pack>>& apair)
		{
			return sendmore_stl(asession, aactorid, arequestactorid, apair);
		}

		bool sendmore(const std::list<i32_sessionid>& asession, i64_actorid aactorid, i64_actorid arequestactorid, std::pair<std::shared_ptr<pack>, std::shared_ptr<pack>>& apair)
		{
			return sendmore_stl(asession, aactorid, arequestactorid, apair);
		}

		bool sendmore(const std::set<i32_sessionid>& asession, i64_actorid aactorid, i64_actorid arequestactorid, std::pair<std::shared_ptr<pack>, std::shared_ptr<pack>>& apair)
		{
			return sendmore_stl(asession, aactorid, arequestactorid, apair);
		}

		template <typename T>
		void send_client(i32_actordataid auid, i16_area aarea, i32_gatewayid agateway, T& adata)
		{
			np_actor_forward<T, EPROTOCOL_TYPE_PROTOCOLBUFF, true, T> pro;
			pro.m_uid.push_back(auid);
			pro.m_area.push_back(aarea);
			pro.set_data(&adata);
			if (agateway != 0)
			{
				i32_session lsession = server_session::sessionid(agateway);
				if (lsession == -1)
				{
					return;
				}
				send(lsession, pro, nguid::make(), nguid::make());
			}
		}

		template <typename T>
		void send_client(const std::vector<std::pair<i32_actordataid, i16_area>>& avec, i32_gatewayid agateway, T& adata)
		{
			np_actor_forward<T, EPROTOCOL_TYPE_PROTOCOLBUFF, true, T> pro;
			for (int i = 0; i < avec.size(); ++i)
			{
				pro.m_uid.push_back(avec[i].first);
				pro.m_area.push_back(avec[i].second);
			}
			pro.set_data(&adata);
			if (agateway != 0)
			{
				i32_session lsession = server_session::sessionid(agateway);
				if (lsession == -1)
				{
					return;
				}
				send(lsession, pro, nguid::make(), nguid::make());
			}
		}
	};
}// namespace ngl