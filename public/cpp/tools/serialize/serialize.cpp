#include "serialize.h"
#include "varint.h"
#include "db.pb.h"
#include "bit.h"

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

	serialize::serialize(char* abuff, int32_t alen)
		:m_buff(abuff), m_len(alen), m_pos(0)
	{}

	char* serialize::buff()
	{
		return m_buff;
	}

	int serialize::byte()
	{
		return m_pos;
	}

	int serialize::len()
	{
		return m_len;
	}

	void serialize::add_bytes(int abytes)
	{
		m_pos += abytes;
	}

	void serialize::dec_bytes(int abytes)
	{
		m_pos -= abytes;
	}

	bool serialize::basetype(void* adata, int32_t alen)
	{
		if (m_pos + alen > m_len)
			return false;
		memcpy(&m_buff[m_pos], adata, alen);
		m_pos += alen;
		return true;
	}

	bool serialize::push(const forward& adata)
	{
		return basetype((void*)adata.m_buff, adata.m_len);
	}

	bool serialize::push(forward& adata)
	{
		return basetype((void*)adata.m_buff, adata.m_len);
	}

	bool serialize::push(const int8_t adata)
	{
		return basetype(adata);
	}

	bool serialize::push(const uint8_t adata)
	{
		return basetype(adata);
	}

	bool serialize::push(const int16_t adata)
	{
		int16_t lvalues = adata;
		lvalues = bytes_order::transformlittle(lvalues);
		return basetype(lvalues);
	}

	bool serialize::push(const uint16_t adata)
	{
		return push((const int16_t)adata);
	}

	bool serialize::push(const int32_t adata)
	{
		int32_t lvalues = adata;
		lvalues = bytes_order::transformlittle(lvalues);
		varint::parm<int32_t> lparm
		{
			.m_value = lvalues,
			.m_buf = &m_buff[m_pos],
			.m_len = m_len - m_pos,
			.m_bytes = &m_pos,
		};
		return varint::encode(lparm);
	}

	bool serialize::push(const uint32_t adata)
	{
		return push((const int32_t)adata);
	}

	bool serialize::push(const int64_t adata)
	{
		int64_t lvalues = adata;
		lvalues = bytes_order::transformlittle(lvalues);
		varint::parm<int64_t> lparm
		{
			.m_value = lvalues,
			.m_buf = &m_buff[m_pos],
			.m_len = m_len - m_pos,
			.m_bytes = &m_pos,
		};
		return varint::encode(lparm);
	}

	bool serialize::push(const uint64_t adata)
	{
		return push((const int64_t)adata);
	}

	bool serialize::push(const float adata)
	{
		return basetype(adata);
	}

	bool serialize::push(const double adata)
	{
		return basetype(adata);
	}

	bool serialize::push(const bool adata)
	{
		return basetype(adata);
	}

	bool serialize::push(const std::vector<bool>& avec)
	{
		if (push_stlsize(avec) == false)
			return false;
		for (bool lbool : avec)
		{
			if (push(lbool) == false)
				return false;
		}
		return true;
	}

	bool serialize::push(const std::vector<int8_t>& avec)
	{
		return push_vector_number(avec);
	}

	bool serialize::push(const std::vector<uint8_t>& avec)
	{
		return push_vector_number(avec);
	}

	bool serialize::push(const std::vector<int32_t>& avec)
	{
		return push_vector_number_compile(avec);
	}

	bool serialize::push(const std::vector<uint32_t>& avec)
	{
		return push_vector_number_compile(avec);
	}

	bool serialize::push(const std::vector<int64_t>& avec)
	{
		return push_vector_number_compile(avec);
	}

	bool serialize::push(const std::vector<uint64_t>& avec)
	{
		return push_vector_number_compile(avec);
	}

	bool serialize::push(const std::vector<float>& avec)
	{
		return push_vector_number(avec);
	}

	bool serialize::push(const std::vector<double>& avec)
	{
		return push_vector_number(avec);
	}

	bool serialize::push(std::pair<uint32_t, const char*>& adata)
	{
		if (!push(adata.first))
			return false;
		return basetype((void*)adata.second, adata.first);
	}

	bool serialize::push(const std::string& astr)
	{
		if (!push_stlsize(astr))
			return false;
		return basetype((void*)astr.c_str(), sizeof(char) * astr.size());
	}

	bool serialize::push()
	{
		return true;
	}

	unserialize::unserialize(const char* abuff, int32_t alen) :
		m_buff(abuff),
		m_len(alen),
		m_pos(0)
	{}

	const char* unserialize::buff()
	{
		return m_buff;
	}

	int unserialize::byte()
	{
		return m_pos;
	}

	int unserialize::len()
	{
		return m_len;
	}

	void unserialize::add_bytes(int abytes)
	{
		m_pos += abytes;
	}

	void unserialize::dec_bytes(int abytes)
	{
		m_pos -= abytes;
	}

	bool unserialize::basetype(void* adata, int32_t alen)
	{
		if (m_pos + alen > m_len)
			return false;
		memcpy(adata, &m_buff[m_pos], alen);
		m_pos += alen;
		return true;
	}

	 bool unserialize::pop(forward& adata)
	 {
		 adata.m_len = m_len - m_pos;
		 adata.m_buff = &m_buff[m_pos];
		 m_pos = m_len;
		 return true;
	 }

	 bool unserialize::pop(int8_t& adata)
	 {
		 return basetype(adata);
	 }

	 bool unserialize::pop(uint8_t& adata)
	 {
		 return basetype(adata);
	 }

	 bool unserialize::pop(int16_t& adata)
	 {
		 if (basetype(adata) == false)
			 return false;
		 adata = bytes_order::transformlittle(adata);
		 return true;
	 }

	 bool unserialize::pop(uint16_t& adata)
	 {
		 int16_t ltemp = adata;
		 if (pop(ltemp) == false)
			 return false;
		 adata = ltemp;
		 return true;
	 }

	 bool unserialize::pop(int32_t& adata)
	 {
		 varint::parm<int32_t> lparm
		 {
			 .m_value = adata,
			 .m_buf = (char*)&m_buff[m_pos],
			 .m_len = m_len - m_pos,
			 .m_bytes = &m_pos,
		 };
		 if (varint::decode(lparm) == false)
			 return false;
		 adata = lparm.m_value;
		 adata = bytes_order::transformlittle(adata);
		 return true;
	 }

	 bool unserialize::pop(uint32_t& adata)
	 {
		 int32_t ldata = 0;
		 if (pop(ldata) == false)
			 return false;
		 adata = ldata;
		 return true;
	 }

	 bool unserialize::pop(int64_t& adata)
	 {
		 varint::parm<int64_t> lparm
		 {
			 .m_value = adata,
			 .m_buf = (char*)&m_buff[m_pos],
			 .m_len = m_len - m_pos,
			 .m_bytes = &m_pos,
		 };
		 if (varint::decode(lparm) == false)
			 return false;
		 adata = lparm.m_value;
		 adata = bytes_order::transformlittle(adata);
		 return true;
	 }

	 bool unserialize::pop(uint64_t& adata)
	 {
		 int64_t ldata = 0;
		 if (pop(ldata) == false)
			 return false;
		 adata = ldata;
		 return true;
	 }

	 bool unserialize::pop(float& adata)
	 {
		 return basetype(adata);
	 }

	 bool unserialize::pop(double& adata)
	 {
		 return basetype(adata);
	 }

	 bool unserialize::pop(bool& adata)
	 {
		 if (basetype(adata))
			 return true;
		 return false;
	 }

	 bool unserialize::pop(std::vector<bool>& adata)
	 {
		 int16_t lsize = 0;
		 if (!pop(lsize))
			 return false;
		 if (lsize < 0)
			 return false;
		 adata.resize(lsize);
		 for (int i = 0; i < lsize; ++i)
		 {
			 bool lbool = false;
			 if (pop(lbool) == false)
				 return false;
			 adata[i] = lbool;
		 }
		 return true;
	 }

	 bool unserialize::pop(std::vector<int8_t>& adata)
	 {
		 return pop_vector_number(adata);
	 }

	 bool unserialize::pop(std::vector<uint8_t>& adata)
	 {
		 return pop_vector_number(adata);
	 }

	 bool unserialize::pop(std::vector<int32_t>& adata)
	 {
		 int16_t lsize = 0;
		 if (!pop(lsize))
			 return false;
		 if (lsize < 0)
			 return false;
		 adata.resize(lsize);
		 for (auto& item : adata)
		 {
			 if (pop(item) == false)
				 return false;
		 }
		 return true;
	 }

	 bool unserialize::pop(std::vector<uint32_t>& adata)
	 {
		 int16_t lsize = 0;
		 if (!pop(lsize))
			 return false;
		 if (lsize < 0)
			 return false;
		 adata.resize(lsize);
		 for (auto& item : adata)
		 {
			 if (pop(item) == false)
				 return false;
		 }
		 return true;
	 }

	 bool unserialize::pop(std::vector<int64_t>& adata)
	 {
		 int16_t lsize = 0;
		 if (!pop(lsize))
			 return false;
		 if (lsize < 0)
			 return false;
		 adata.resize(lsize);
		 for (auto& item : adata)
		 {
			 if (pop(item) == false)
				 return false;
		 }
		 return true;
	 }

	 bool unserialize::pop(std::vector<uint64_t>& adata)
	 {
		 int16_t lsize = 0;
		 if (!pop(lsize))
			 return false;
		 if (lsize < 0)
			 return false;
		 adata.resize(lsize);
		 for (auto& item : adata)
		 {
			 if (pop(item) == false)
				 return false;
		 }
		 return true;
	 }

	 bool unserialize::pop(std::vector<float>& adata)
	 {
		 return pop_vector_number(adata);
	 }

	 bool unserialize::pop(std::vector<double>& adata)
	 {
		 return pop_vector_number(adata);
	 }

	 bool unserialize::pop(std::pair<uint32_t, const char*>& adata)
	 {
		 if (!pop(adata.first))
			 return false;
		 if (m_pos + adata.first > m_len)
			 return false;
		 adata.second = &m_buff[m_pos];
		 m_pos += adata.first;
		 return true;
	 }

	 bool unserialize::pop(std::string& adata)
	 {
		 uint16_t lsize = 0;
		 if (!pop(lsize))
			 return false;
		 if (lsize > 0)
		 {
			 adata.resize(lsize);
			 if (!basetype((void*)&adata[0], lsize))
				 return false;
		 }
		 return true;
	 }

	 bool unserialize::pop()
	 {
		 return true;
	 }

	 serialize_bytes::serialize_bytes() :
		 m_size(0)
	 {}

	 int serialize_bytes::bytes()
	 {
		 return m_size;
	 }

	 int serialize_bytes::add_bytes(int abytes)
	 {
		 return m_size += abytes;
	 }


	 int serialize_bytes::bytes(const forward& adata)
	 {
		 return m_size += adata.m_len;
	 }

	 int serialize_bytes::bytes(forward& adata)
	 {
		 return m_size += adata.m_len;
	 }

	 int serialize_bytes::bytes(const int8_t adata)
	 {
		 return m_size += sizeof(int8_t);
	 }

	 int serialize_bytes::bytes(const uint8_t adata)
	 {
		 return m_size += sizeof(uint8_t);
	 }

	 int serialize_bytes::bytes(const int16_t adata)
	 {
		 return m_size += sizeof(int16_t);
	 }

	 int serialize_bytes::bytes(const uint16_t adata)
	 {
		 return m_size += sizeof(uint16_t);
	 }

	 int serialize_bytes::bytes(const int32_t adata)
	 {
		 int32_t lvalues = adata;
		 lvalues = bytes_order::transformlittle(lvalues);
		 varint::parm_length<int32_t> laprm{ .m_value = lvalues };
		 return m_size += varint::length(laprm);
	 }

	 int serialize_bytes::bytes(const uint32_t adata)
	 {
		 return bytes((const int32_t)adata);
	 }

	 int serialize_bytes::bytes(const int64_t adata)
	 {
		 int64_t lvalues = adata;
		 lvalues = bytes_order::transformlittle(lvalues);
		 varint::parm_length<int64_t> laprm{ .m_value = lvalues };
		 return m_size += varint::length(laprm);
	 }

	 int serialize_bytes::bytes(const uint64_t adata)
	 {
		 return bytes((const int64_t)adata);
	 }

	 int serialize_bytes::bytes(const float adata)
	 {
		 return m_size += sizeof(adata);
	 }

	 int serialize_bytes::bytes(const double adata)
	 {
		 return m_size += sizeof(adata);
	 }

	 int serialize_bytes::bytes(const bool adata)
	 {
		 return m_size += sizeof(adata);
	 }

	 int serialize_bytes::bytes(const std::vector<bool>& avec)
	 {
		 bytes(uint16_t(avec.size()));
		 return m_size += avec.size() * sizeof(bool);
	 }

	 int serialize_bytes::bytes(const std::vector<int8_t>& avec)
	 {
		 bytes(int16_t(avec.size()));
		 return m_size += sizeof(int8_t) * avec.size();
	 }

	 int serialize_bytes::bytes(const std::vector<uint8_t>& avec)
	 {
		 bytes(int16_t(avec.size()));
		 return m_size += sizeof(uint8_t) * avec.size();
	 }

	 int serialize_bytes::bytes(const std::vector<int16_t>& avec)
	 {
		 bytes(int16_t(avec.size()));
		 return m_size += sizeof(int16_t) * avec.size();
	 }

	 int serialize_bytes::bytes(const std::vector<uint16_t>& avec)
	 {
		 bytes(int16_t(avec.size()));
		 return m_size += sizeof(uint16_t) * avec.size();
	 }

	 int serialize_bytes::bytes(const std::vector<int32_t>& avec)
	 {
		 bytes(int16_t(avec.size()));
		 for (int i = 0; i < avec.size(); ++i)
		 {
			 bytes(avec[i]);
		 }
		 return m_size;
	 }

	 int serialize_bytes::bytes(const std::vector<uint32_t>& avec)
	 {
		 bytes(int16_t(avec.size()));
		 for (int i = 0; i < avec.size(); ++i)
		 {
			 bytes(avec[i]);
		 }
		 return m_size;
	 }

	 int serialize_bytes::bytes(const std::vector<int64_t>& avec)
	 {
		 bytes(int16_t(avec.size()));
		 for (int i = 0; i < avec.size(); ++i)
		 {
			 bytes(avec[i]);
		 }
		 return m_size;
	 }

	 int serialize_bytes::bytes(const std::vector<uint64_t>& avec)
	 {
		 bytes(int16_t(avec.size()));
		 for (int i = 0; i < avec.size(); ++i)
		 {
			 bytes(avec[i]);
		 }
		 return m_size;
	 }

	 int serialize_bytes::bytes(const std::vector<float>& avec)
	 {
		 bytes(int16_t(avec.size()));
		 return m_size += sizeof(float) * avec.size();
	 }

	 int serialize_bytes::bytes(const std::vector<double>& avec)
	 {
		 bytes(int16_t(avec.size()));
		 return m_size += sizeof(double) * avec.size();
	 }

	 int serialize_bytes::bytes(const std::list<int8_t>& avec)
	 {
		 bytes(int16_t(avec.size()));
		 return m_size += sizeof(int8_t) * avec.size();
	 }

	 int serialize_bytes::bytes(const std::list<uint8_t>& avec)
	 {
		 bytes(int16_t(avec.size()));
		 return m_size += sizeof(uint8_t) * avec.size();
	 }

	 int serialize_bytes::bytes(const std::list<int16_t>& avec)
	 {
		 bytes(int16_t(avec.size()));
		 return m_size += sizeof(int16_t) * avec.size();
	 }

	 int serialize_bytes::bytes(const std::list<uint16_t>& avec)
	 {
		 bytes(int16_t(avec.size()));
		 return m_size += sizeof(uint16_t) * avec.size();
	 }

	 int serialize_bytes::bytes(const std::list<int32_t>& avec)
	 {
		 bytes(int16_t(avec.size()));
		 for (int32_t item : avec)
			 bytes(item);
		 return m_size;
	 }

	 int serialize_bytes::bytes(const std::list<uint32_t>& avec)
	 {
		 bytes(int16_t(avec.size()));
		 for (uint32_t item : avec)
			 bytes(item);
		 return m_size;
	 }

	 int serialize_bytes::bytes(const std::list<int64_t>& avec)
	 {
		 bytes(int16_t(avec.size()));
		 for (int64_t item : avec)
			 bytes(item);
		 return m_size;
	 }

	 int serialize_bytes::bytes(const std::list<uint64_t>& avec)
	 {
		 bytes(int16_t(avec.size()));
		 for (uint64_t item : avec)
			 bytes(item);
		 return m_size;
	 }

	 int serialize_bytes::bytes(const std::list<float>& avec)
	 {
		 bytes(int16_t(avec.size()));
		 return m_size += sizeof(float) * avec.size();
	 }

	 int serialize_bytes::bytes(const std::list<double>& avec)
	 {
		 bytes(int16_t(avec.size()));
		 return m_size += sizeof(double) * avec.size();
	 }

	 int serialize_bytes::bytes(const std::set<int8_t>& avec)
	 {
		 bytes(int16_t(avec.size()));
		 return m_size += sizeof(int8_t) * avec.size();
	 }

	 int serialize_bytes::bytes(const std::set<uint8_t>& avec)
	 {
		 bytes(int16_t(avec.size()));
		 return m_size += sizeof(uint8_t) * avec.size();
	 }

	 int serialize_bytes::bytes(const std::set<int16_t>& avec)
	 {
		 bytes(int16_t(avec.size()));
		 return m_size += sizeof(int16_t) * avec.size();
	 }

	 int serialize_bytes::bytes(const std::set<uint16_t>& avec)
	 {
		 bytes(int16_t(avec.size()));
		 return m_size += sizeof(uint16_t) * avec.size();
	 }

	 int serialize_bytes::bytes(const std::set<int32_t>& avec)
	 {
		 bytes(int16_t(avec.size()));
		 for (int32_t item : avec)
			 bytes(item);
		 return m_size;
	 }

	 int serialize_bytes::bytes(const std::set<uint32_t>& avec)
	 {
		 bytes(int16_t(avec.size()));
		 for (uint32_t item : avec)
			 bytes(item);
		 return m_size;
	 }

	 int serialize_bytes::bytes(const std::set<int64_t>& avec)
	 {
		 bytes(int16_t(avec.size()));
		 for (int64_t item : avec)
			 bytes(item);
		 return m_size;
	 }

	 int serialize_bytes::bytes(const std::set<uint64_t>& avec)
	 {
		 bytes(int16_t(avec.size()));
		 for (uint64_t item : avec)
			 bytes(item);
		 return m_size;
	 }

	 int serialize_bytes::bytes(const std::set<float>& avec)
	 {
		 bytes(int16_t(avec.size()));
		 return m_size += sizeof(float) * avec.size();
	 }

	 int serialize_bytes::bytes(const std::set<double>& avec)
	 {
		 bytes(int16_t(avec.size()));
		 return m_size += sizeof(double) * avec.size();
	 }

	 int serialize_bytes::bytes(const std::unordered_set<int8_t>& avec)
	 {
		 bytes(int16_t(avec.size()));
		 return m_size += sizeof(int8_t) * avec.size();
	 }

	 int serialize_bytes::bytes(const std::unordered_set<uint8_t>& avec)
	 {
		 bytes(int16_t(avec.size()));
		 return m_size += sizeof(uint8_t) * avec.size();
	 }

	 int serialize_bytes::bytes(const std::unordered_set<int16_t>& avec)
	 {
		 bytes(int16_t(avec.size()));
		 return m_size += sizeof(int16_t) * avec.size();
	 }

	 int serialize_bytes::bytes(const std::unordered_set<uint16_t>& avec)
	 {
		 bytes(int16_t(avec.size()));
		 return m_size += sizeof(uint16_t) * avec.size();
	 }

	 int serialize_bytes::bytes(const std::unordered_set<int32_t>& avec)
	 {
		 bytes(int16_t(avec.size()));
		 for (int32_t item : avec)
			 bytes(item);
		 return m_size;
	 }

	 int serialize_bytes::bytes(const std::unordered_set<uint32_t>& avec)
	 {
		 bytes(int16_t(avec.size()));
		 for (uint32_t item : avec)
			 bytes(item);
		 return m_size;
	 }

	 int serialize_bytes::bytes(const std::unordered_set<int64_t>& avec)
	 {
		 bytes(int16_t(avec.size()));
		 for (int64_t item : avec)
			 bytes(item);
		 return m_size;
	 }

	 int serialize_bytes::bytes(const std::unordered_set<uint64_t>& avec)
	 {
		 bytes(int16_t(avec.size()));
		 for (uint64_t item : avec)
			 bytes(item);
		 return m_size;
	 }

	 int serialize_bytes::bytes(const std::unordered_set<float>& avec)
	 {
		 bytes(int16_t(avec.size()));
		 return m_size += sizeof(float) * avec.size();
	 }

	 int serialize_bytes::bytes(const std::unordered_set<double>& avec)
	 {
		 bytes(int16_t(avec.size()));
		 return m_size += sizeof(double) * avec.size();
	 }

	 int serialize_bytes::bytes(const std::string& astr)
	 {
		 bytes(int16_t(astr.size()));
		 return m_size += astr.size() * sizeof(char);
	 }
}// namespace ngl