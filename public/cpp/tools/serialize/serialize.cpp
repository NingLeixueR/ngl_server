#include "serialize.h"
#include "db.pb.h"

namespace ngl
{
	bool forward::pop(unserialize& ser)
	{
		return ser.pop(*this);
	}

	bool forward::push(serialize& ser)const
	{
		return ser.push(*this);
	}

	int forward::bytes(serialize_bytes& abytes)const
	{
		return abytes.bytes(*this);
	}

	bool serialize::push(const forward& adata)
	{
		return basetype((void*)adata.m_buff, adata.m_len);
	}

	bool serialize::push(forward& adata)
	{
		return basetype((void*)adata.m_buff, adata.m_len);
	}

	bool unserialize::pop(forward& adata)
	{
		adata.m_len = m_len - m_pos;
		adata.m_buff = &m_buff[m_pos];
		m_pos = m_len;
		return true;
	}

	int serialize_bytes::bytes(const forward& adata)
	{
		return m_size += adata.m_len;
	}

	int serialize_bytes::bytes(forward& adata)
	{
		return m_size += adata.m_len;
	}
}// namespace ngl