#include "localtime.h"
#include "protocol.h"
#include "segpack.h"
#include "nlog.h"

namespace ngl
{
	segpack_rate_interval::segpack_rate_interval()
		:m_resetutc(localtime::gettime()), m_ratecount(0)
	{}

	void segpack_rate_interval::reset()
	{
		m_resetutc = localtime::gettime();
		m_ratecount = 0;
	}

	bool segpack_rate::add(i32_socket aid)
	{
		int lnow = localtime::gettime();
		auto itor = m_data.find(aid);
		if (itor == m_data.end())
		{
			m_data.insert(std::make_pair(aid, segpack_rate_interval()));
			return true;
		}
		if (itor->second.m_resetutc == 0)
		{
			itor->second.reset();
			return true;
		}
		if (lnow >= itor->second.m_resetutc + sysconfig::rate_interval())
		{
			itor->second.reset();
			return true;
		}
		++itor->second.m_ratecount;
		if (sysconfig::rate_count() < itor->second.m_ratecount)
			return false;
		return true;
	}

	bool segpack_heartbeat::is_heartbeat(int aprotocolnum)
	{		
		return tprotocol::protocol<pbnet::PROBUFF_NET_HEARTBEAT>() == aprotocolnum;
	}

	struct segpack::impl_segpack
	{
		std::map<i32_socket, std::shared_ptr<pack>> m_data; //残包暂时保管
		bpool			m_pool;
		segpack_rate	m_rate;
		segpack*		m_segpack;

		impl_segpack(segpack* asegpack) :
			m_segpack(asegpack)
		{}

		void erase(i32_socket aid)
		{
			m_data.erase(aid);
		}

		enum class edopush
		{
			e_continue,		// 继续循环
			e_break,		// 跳出循环
			e_error,		// 出错
		};

		inline edopush do_push(i32_socket aid, const char*& ap, int& alen, bool aislanip)
		{
			if (alen <= 0)
				return edopush::e_break;
			std::shared_ptr<pack> lpack = nullptr;
			// # 查看有没有残包
			if (auto itor = m_data.find(aid); itor != m_data.end())
			{
				lpack = itor->second;
				m_data.erase(itor);
			}
			else
			{
				lpack = pack::make_pack(&m_pool, 0);
				lpack->m_id = aid;
				lpack->m_segpack = m_segpack;
			}
			if (EPH_HEAD_VAL lval = lpack->m_head.push(ap, alen); lval == EPH_HEAD_MASK_FAIL)
			{
				if (aislanip == false)
				{
					return edopush::e_error;
				}
				// 支持telnet命令访问，telnet ip port 后记得['CTRL+]']
				// 只支持256个字符的进程命令
				if (lpack->m_buff == nullptr)
				{
					lpack->malloc(256);
					memcpy(lpack->m_buff, lpack->m_head.m_data, lpack->m_head.m_wpos);
					lpack->m_pos += lpack->m_head.m_wpos;
				}
				if (lpack->m_pos + alen >= 256)
				{
					return edopush::e_error;
				}
				memcpy(&lpack->m_buff[lpack->m_pos], ap, alen);
				lpack->m_pos += alen;
				ap += alen;
				alen -= alen;
				lpack->m_buff[lpack->m_pos] = '\0';
				protocol::cmd(lpack);
				return edopush::e_break;
			}
			else
			{
				switch (lval)
				{
				case ngl::EPH_HEAD_MASK_SUCCESS:
				case ngl::EPH_HEAD_FOLLOW:
					m_data.try_emplace(aid, lpack);
					return edopush::e_break;
				default:
					break;
				}
			}

			if (alen < 0)
				return edopush::e_break;
			int len = lpack->m_head.getvalue(EPH_BYTES);
			if (len < 0)
			{
				return edopush::e_error;
			}
			if (aislanip == false && nconfig::node_type() != ROBOT && len >= net_config_recv_buff_maxbyte)
			{
				m_data.erase(aid);
				log_error()->print("sockect recv {} len >= SOCKECT_MAX_BUFF_SIZE({})"
					, lpack->m_head, (int)net_config_recv_buff_maxbyte
				);
				return edopush::e_error;
			}

			if (len == 0)
			{
				if (!aislanip && !m_rate.add(aid))
				{
					return edopush::e_error;
				}
				//只有包头的包
				//检测是否是心跳包
				if (segpack_heartbeat::is_heartbeat(lpack->m_head.get_protocolnumber()))
				{
					return edopush::e_break;
				}
				protocol::push(lpack);
				return edopush::e_continue;
			}
			if (lpack->m_buff == nullptr)
			{
				lpack->malloc(len);
			}
			if (len != 0 && lpack->m_buff == nullptr)
				return edopush::e_error;
			int ltemp = len - lpack->m_pos;
			if (ltemp < 0)
				return edopush::e_error;
			ltemp = ltemp > alen ? alen : ltemp;
			memcpy(&lpack->m_buff[lpack->m_pos], ap, ltemp);
			lpack->m_pos += ltemp;
			alen -= ltemp;
			ap += ltemp;
			if (lpack->isready())
			{
				if (!aislanip && !m_rate.add(aid))
				{
					return edopush::e_error;
				}

				if (localtime::gettime() < lpack->m_head.getvalue(EPH_TIME) + sysconfig::net_timeout())
				{
					protocol::push(lpack);
				}
				else
				{
					log_error()->print("time[{} < {} + {} ]"
						, localtime::gettime(), lpack->m_head.getvalue(EPH_TIME), sysconfig::net_timeout()
					);
				}
				return edopush::e_continue;
			}
			m_data[aid] = lpack;
			return edopush::e_break;
		}

		bool push(i32_socket aid, const char* ap, int alen, bool aislanip)
		{
			if (ap == nullptr && alen == 0)
			{
				erase(aid);
			}

			for (;;)
			{
				edopush levalue = do_push(aid, ap, alen, aislanip);
				if (levalue == edopush::e_error)
					return false;
				if (levalue == edopush::e_break)
					break;
			}
			if (alen > 0)
				return false;
			return true;
		}
	};

	segpack::segpack()
	{
		m_impl_segpack.make_unique(this);
	}

	bool segpack::push(i32_socket aid, const char* ap, int alen, bool aislanip)
	{
		return m_impl_segpack()->push(aid, ap, alen, aislanip);
	}

	void segpack::close(i32_socket aid)
	{
		//有可能缓存残包
		m_impl_segpack()->erase(aid);
	}
}// namespace ngl