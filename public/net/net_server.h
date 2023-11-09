#pragma once

#include "net.h"
#include "actor_manage.h"
#include "actor_base.h"

namespace ngl
{
	class net_server
	{
	public:
		template <typename T>
		static void send2client(int aroleid, T& adata)
		{
			actor_gateway_forward<T, true, T> pro;
			pro.m_uid.push_back(aroleid);
			pro.m_data = &adata;
			xmlinfo* info = xmlnode::local();
			uint32_t lgateway = 0;
			Assert(info->gateway(lgateway));
			ngl::net::sendtoserver(lgateway, pro);
		}
	};
}

