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

		bool push(i32_socket aid, const char* ap, int alen, bool aislanip)
		{
			if (ap == nullptr && alen == 0)
			{
				erase(aid);
			}

			while (true)
			{
				if (alen <= 0)
					return true;
				std::shared_ptr<pack> lpack = nullptr;
				// # 查看有没有残包
				auto itor = m_data.find(aid);
				if (itor != m_data.end())
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
				EPH_HEAD_VAL lval = lpack->m_head.push(ap, alen);
				if (lval == EPH_HEAD_MASK_FAIL)
				{	
					if (aislanip == false)
					{
						return false;
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
						return false;
					}
					memcpy(&lpack->m_buff[lpack->m_pos], ap, alen);
					lpack->m_pos += alen;
					ap += alen;
					alen -= alen;
					lpack->m_buff[lpack->m_pos] = '\0';
					protocol::cmd(lpack);
					break;
				}
				else
				{
					switch (lval)
					{
					case ngl::EPH_HEAD_MASK_SUCCESS:
					case ngl::EPH_HEAD_FOLLOW:
						m_data.insert(std::make_pair(aid, lpack));
						return true;
					}
				}
				
				if (alen < 0)
					return true;
				int len = lpack->m_head.getvalue(EPH_BYTES);
				if (len < 0)
				{
					return false;
				}
				if (aislanip == false && nconfig::node_type() != ROBOT)
				{
					if (len >= net_config_recv_buff_maxbyte)
					{
						m_data.erase(aid);
						log_error()->print("sockect recv {} len >= SOCKECT_MAX_BUFF_SIZE({})"
							, lpack->m_head, (int)net_config_recv_buff_maxbyte
						);
						return false;
					}
				}
				
				if (len == 0)
				{
					if (!aislanip)
					{
						if (!m_rate.add(aid))
						{
							return false;
						}
					}
					//只有包头的包
					//检测是否是心跳包
					if (segpack_heartbeat::is_heartbeat(lpack->m_head.get_protocolnumber()))
					{
						return true;
					}
					protocol::push(lpack);
					continue;
				}
				if (lpack->m_buff == nullptr)
				{
					lpack->malloc(len);
				}
				if (len != 0 && lpack->m_buff == nullptr)
					return false;
				int ltemp = len - lpack->m_pos;
				if (ltemp < 0)
					return false;
				ltemp = ltemp > alen ? alen : ltemp;
				memcpy(&lpack->m_buff[lpack->m_pos], ap, ltemp);
				lpack->m_pos += ltemp;
				alen -= ltemp;
				ap += ltemp;
				if (lpack->isready())
				{
					if (!aislanip)
					{
						if (!m_rate.add(aid))
							return false;
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
					continue;
				}
				m_data[aid] = lpack;
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