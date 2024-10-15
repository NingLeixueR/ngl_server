#include "json_read.h"
#include "tools.h"

namespace ngl
{
	json_read::json_read(const char* astr) :
		m_json(cJSON_Parse(astr)),
		m_free(true)
	{}

	json_read::json_read() :
		m_json(nullptr),
		m_free(false)
	{}

	json_read::~json_read()
	{
		if (m_json != nullptr && m_free)
			cJSON_Delete(m_json);
	}

	bool json_read::read(const char* akey, std::string& adata) const
	{
		const cJSON* ret = cJSON_GetObjectItem(m_json, akey);
		if (nullptr == ret || ret->type != cJSON_String)
			return false;
		adata = ret->valuestring;
		return true;
	}

	class json_read_number
	{
	public:
		template <typename TNUMBER>
		static bool fun(const cJSON* ret, TNUMBER& adata)
		{
			if (ret->type == cJSON_Number)
			{
				adata = (TNUMBER)ret->valueint;
				return true;
			}
			if (ret->type == cJSON_String)
			{
				try
				{
					adata = tools::lexical_cast<TNUMBER>(ret->valuestring);
					return true;
				}
				catch (...)
				{
					return false;
				}
			}
			return true;
		}
	};
	
	bool json_read::read(const char* akey, int8_t& adata) const
	{
		const cJSON* ret = cJSON_GetObjectItem(m_json, akey);
		if (nullptr == ret)
			return false;
		return json_read_number::fun(ret, adata);
	}

	bool json_read::read(const char* akey, int16_t& adata) const
	{
		const cJSON* ret = cJSON_GetObjectItem(m_json, akey);
		if (nullptr == ret)
			return false;
		return json_read_number::fun(ret, adata);
	}

	bool json_read::read(const char* akey, int32_t& adata) const
	{
		const cJSON* ret = cJSON_GetObjectItem(m_json, akey);
		if (nullptr == ret)
			return false;
		return json_read_number::fun(ret, adata);
	}

	bool json_read::read(const char* akey, int64_t& adata) const
	{
		const cJSON* ret = cJSON_GetObjectItem(m_json, akey);
		if (nullptr == ret)
			return false;
		if (ret->type == cJSON_String)
		{
			adata = tools::lexical_cast<int64_t>(ret->valuestring);
			return true;
		}
		// ³¢ÊÔint32_t
		if (ret->type == cJSON_Number)
		{
			adata = ret->valueint;
			return true;
		}
		return false;
	}

	bool json_read::read(const char* akey, uint8_t& adata) const
	{
		const cJSON* ret = cJSON_GetObjectItem(m_json, akey);
		if (nullptr == ret)
			return false;
		return json_read_number::fun(ret, adata);
	}

	bool json_read::read(const char* akey, uint16_t& adata) const
	{
		const cJSON* ret = cJSON_GetObjectItem(m_json, akey);
		if (nullptr == ret)
			return false;
		return json_read_number::fun(ret, adata);
	}

	bool json_read::read(const char* akey, uint32_t& adata) const
	{
		const cJSON* ret = cJSON_GetObjectItem(m_json, akey);
		if (nullptr == ret)
			return false;
		return json_read_number::fun(ret, adata);
	}

	bool json_read::read(const char* akey, uint64_t& adata) const
	{
		const cJSON* ret = cJSON_GetObjectItem(m_json, akey);
		if (nullptr == ret)
			return false;
		if (ret->type == cJSON_String)
		{
			adata = tools::lexical_cast<uint64_t>(ret->valuestring);
			return true;
		}
		// ³¢ÊÔint32_t
		if (ret->type == cJSON_Number)
		{
			adata = ret->valueint;
			return true;
		}
		return false;
	}

	bool json_read::read(const char* akey, float& adata) const
	{
		const cJSON* ret = cJSON_GetObjectItem(m_json, akey);
		if (nullptr == ret)
			return false;
		return json_read_number::fun(ret, adata);
	}

	bool json_read::read(const char* akey, double& adata) const
	{
		const cJSON* ret = cJSON_GetObjectItem(m_json, akey);
		if (nullptr == ret || ret->type != cJSON_Number)
			return false;
		return json_read_number::fun(ret, adata);
	}

	bool json_read::read(const char* akey, bool& adata) const
	{
		const cJSON* ret = cJSON_GetObjectItem(m_json, akey);
		if (nullptr == ret)
			return false;
		return json_read_number::fun(ret, adata);
	}

	bool json_read::read(const char* akey, cJSON*& adata) const
	{
		cJSON* ret = cJSON_GetObjectItem(m_json, akey);
		if (nullptr == ret)
			return false;
		if (ret->type == cJSON_Object || ret->type == cJSON_Array)
			adata = ret;
		return true;
	}

	bool json_read::read(const char* akey, json_read& adata) const
	{
		if (read(akey, adata.m_json) == false)
			return false;
		adata.m_free = false;
		return true;
	}

	bool json_read::read(const char* akey, std::vector<int8_t>& adata) const
	{
		return read_number<int8_t>(akey, adata, [](const cJSON* ajson, int8_t& aval)->bool
			{
				if (nullptr == ajson || ajson->type != cJSON_Number)
					return false;
				aval = (int8_t)ajson->valueint;
				return true;
			});
	}
	bool json_read::read(const char* akey, std::vector<int16_t>& adata) const
	{
		return read_number<int16_t>(akey, adata, [](const cJSON* ajson, int16_t& aval)->bool
			{
				if (nullptr == ajson || ajson->type != cJSON_Number)
					return false;
				aval = (int16_t)ajson->valueint;
				return true;
			});
	}
	bool json_read::read(const char* akey, std::vector<int32_t>& adata) const
	{
		return read_number<int32_t>(akey, adata, [](const cJSON* ajson, int32_t& aval)->bool
			{
				if (nullptr == ajson || ajson->type != cJSON_Number)
					return false;
				aval = (int32_t)ajson->valueint;
				return true;
			});
	}

	bool json_read::read(const char* akey, std::vector<int64_t>& adata) const
	{
		return read_number<int64_t>(akey, adata, [](const cJSON* ajson, int64_t& aval)->bool
			{
				if (nullptr == ajson || ajson->type != cJSON_String)
					return false;
				aval = tools::lexical_cast<int64_t>(ajson->valuestring);
				return true;
			});
	}

	bool json_read::read(const char* akey, std::vector<uint8_t>& adata) const
	{
		return read_number<uint8_t>(akey, adata, [](const cJSON* ajson, uint8_t& aval)->bool
			{
				if (nullptr == ajson || ajson->type != cJSON_Number)
					return false;
				aval = (uint8_t)ajson->valueint;
				return true;
			});
	}

	bool json_read::read(const char* akey, std::vector<uint16_t>& adata) const
	{
		return read_number<uint16_t>(akey, adata, [](const cJSON* ajson, uint16_t& aval)->bool
			{
				if (nullptr == ajson || ajson->type != cJSON_Number)
					return false;
				aval = (uint16_t)ajson->valueint;
				return true;
			});
	}
	bool json_read::read(const char* akey, std::vector<uint32_t>& adata) const
	{
		return read_number<uint32_t>(akey, adata, [](const cJSON* ajson, uint32_t& aval)->bool
			{
				if (nullptr == ajson || ajson->type != cJSON_Number)
					return false;
				aval = (uint32_t)ajson->valueint;
				return true;
			});
	}

	bool json_read::read(const char* akey, std::vector<uint64_t>& adata) const
	{
		return read_number<uint64_t>(akey, adata, [](const cJSON* ajson, uint64_t& aval)->bool
			{
				if (nullptr == ajson || ajson->type != cJSON_String)
					return false;
				aval = tools::lexical_cast<uint64_t>(ajson->valuestring);
				return true;
			});
	}

	bool json_read::read(const char* akey, std::vector<float>& adata) const
	{
		return read_number<float>(akey, adata, [](const cJSON* ajson, float& aval)->bool
			{
				if (nullptr == ajson || ajson->type != cJSON_Number)
					return false;
				aval = (float)ajson->valuedouble;
				return true;
			});
	}
	bool json_read::read(const char* akey, std::vector<double>& adata) const
	{
		return read_number<double>(akey, adata, [](const cJSON* ajson, double& aval)->bool
			{
				if (nullptr == ajson || ajson->type != cJSON_Number)
					return false;
				aval = ajson->valuedouble;
				return true;
			});
	}

	bool json_read::read(const char* akey, std::vector<bool>& adata) const
	{
		return read_number<bool>(akey, adata, [](const cJSON* ajson, bool& aval)->bool
			{
				if (nullptr == ajson)
					return false;
				aval = ajson->type != cJSON_False;
				return true;
			});
	}

	bool json_read::read(const char* akey, std::vector<std::string>& adata) const
	{
		cJSON* ltemp = nullptr;
		if (read(akey, ltemp) == false)
			return false;
		int lsize = cJSON_GetArraySize(ltemp);
		for (int i = 0; i < lsize; ++i)
		{
			const cJSON* ret = cJSON_GetArrayItem(ltemp, i);
			if (nullptr == ret || ret->type != cJSON_String)
				continue;
			std::string lval;
			adata.emplace_back(ret->valuestring);
		}
		return true;
	}
}// namespace ngl