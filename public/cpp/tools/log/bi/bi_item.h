#pragma once

#include "nprotocol.h"

namespace ngl
{
	struct bi_item
	{
		i32_serverid m_serverid;
		i64_actorid m_roleid;
		int32_t m_itemid;
		int32_t m_tid;
		int32_t m_count;
		int32_t m_time;
		int32_t m_adddec;// 1:Ôö¼Ó 2:¼õÉÙ
		std::string m_mask;

		void print()
		{
			log_bi()->print(
				"item|{}|{}|{}|{}|{}|{}|{}|{}", 
				m_serverid,
				m_roleid,
				m_itemid,
				m_tid,
				m_count,
				m_time,
				m_adddec,
				m_mask
				);
		}
	};


}