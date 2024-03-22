#include "ojson.h"
#include <boost/lexical_cast.hpp>

namespace ngl
{
	ojson::ojson(const char* astr) :
		m_json(cJSON_Parse(astr)),
		m_free(true)
	{}

	ojson::ojson() :
		m_json(nullptr),
		m_free(false)
	{}

	ojson::~ojson()
	{
		if (m_json != nullptr && m_free)
			cJSON_Delete(m_json);
	}

	bool ojson::dec(const char* akey, std::string& adata)
	{
		cJSON* ret = cJSON_GetObjectItem(m_json, akey);
		if (nullptr == ret || ret->type != cJSON_String)
			return false;
		adata = ret->valuestring;
		return true;
	}
	
	bool ojson::dec(const char* akey, int8_t& adata)
	{
		cJSON* ret = cJSON_GetObjectItem(m_json, akey);
		if (nullptr == ret || ret->type != cJSON_Number)
			return false;
		adata = (int8_t)ret->valueint;
		return true;
	}

	bool ojson::dec(const char* akey, int16_t& adata)
	{
		cJSON* ret = cJSON_GetObjectItem(m_json, akey);
		if (nullptr == ret || ret->type != cJSON_Number)
			return false;
		adata = (int16_t)ret->valueint;
		return true;
	}

	bool ojson::dec(const char* akey, int32_t& adata)
	{
		cJSON* ret = cJSON_GetObjectItem(m_json, akey);
		if (nullptr == ret || ret->type != cJSON_Number)
			return false;
		adata = ret->valueint;
		return true;
	}

	bool ojson::dec(const char* akey, int64_t& adata)
	{
		cJSON* ret = cJSON_GetObjectItem(m_json, akey);
		if (nullptr == ret || ret->type != cJSON_String)
			return false;
		adata = boost::lexical_cast<int64_t>(ret->valuestring);
		return true;
	}

	bool ojson::dec(const char* akey, uint8_t& adata)
	{
		cJSON* ret = cJSON_GetObjectItem(m_json, akey);
		if (nullptr == ret || ret->type != cJSON_Number)
			return false;
		adata = (uint8_t)ret->valueint;
		return true;
	}

	bool ojson::dec(const char* akey, uint16_t& adata)
	{
		cJSON* ret = cJSON_GetObjectItem(m_json, akey);
		if (nullptr == ret || ret->type != cJSON_Number)
			return false;
		adata = (uint16_t)ret->valueint;
		return true;
	}

	bool ojson::dec(const char* akey, uint32_t& adata)
	{
		cJSON* ret = cJSON_GetObjectItem(m_json, akey);
		if (nullptr == ret || ret->type != cJSON_Number)
			return false;
		adata = (uint32_t)ret->valueint;
		return true;
	}

	bool ojson::dec(const char* akey, uint64_t& adata)
	{
		cJSON* ret = cJSON_GetObjectItem(m_json, akey);
		if (nullptr == ret || ret->type != cJSON_String)
			return false;
		adata = boost::lexical_cast<uint64_t>(ret->valuestring);
		return true;
	}

	bool ojson::dec(const char* akey, float& adata)
	{
		cJSON* ret = cJSON_GetObjectItem(m_json, akey);
		if (nullptr == ret || ret->type != cJSON_Number)
			return false;
		adata = (float)ret->valuedouble;
		return true;
	}

	bool ojson::dec(const char* akey, double& adata)
	{
		cJSON* ret = cJSON_GetObjectItem(m_json, akey);
		if (nullptr == ret || ret->type != cJSON_Number)
			return false;
		adata = ret->valuedouble;
		return true;
	}

	bool ojson::dec(const char* akey, bool& adata)
	{
		cJSON* ret = cJSON_GetObjectItem(m_json, akey);
		if (nullptr == ret)
			return false;
		adata = ret->type != cJSON_False;
		return true;
	}

	bool ojson::dec(const char* akey, cJSON*& adata)
	{
		cJSON* ret = cJSON_GetObjectItem(m_json, akey);
		if (nullptr == ret)
			return false;
		if (ret->type == cJSON_Object || ret->type == cJSON_Array)
			adata = ret;
		return true;
	}

	bool ojson::dec(const char* akey, ojson& adata)
	{
		if (dec(akey, adata.m_json) == false)
			return false;
		adata.m_free = false;
		return true;
	}

	bool ojson::dec(const char* akey, std::vector<int8_t>& adata)
	{
		return dec_number<int8_t>(akey, adata, [](cJSON* ajson, int8_t& aval)->bool
			{
				if (nullptr == ajson || ajson->type != cJSON_Number)
					return false;
				aval = (int8_t)ajson->valueint;
				return true;
			});
	}
	bool ojson::dec(const char* akey, std::vector<int16_t>& adata)
	{
		return dec_number<int16_t>(akey, adata, [](cJSON* ajson, int16_t& aval)->bool
			{
				if (nullptr == ajson || ajson->type != cJSON_Number)
					return false;
				aval = (int16_t)ajson->valueint;
				return true;
			});
	}
	bool ojson::dec(const char* akey, std::vector<int32_t>& adata)
	{
		return dec_number<int32_t>(akey, adata, [](cJSON* ajson, int32_t& aval)->bool
			{
				if (nullptr == ajson || ajson->type != cJSON_Number)
					return false;
				aval = (int32_t)ajson->valueint;
				return true;
			});
	}

	bool ojson::dec(const char* akey, std::vector<int64_t>& adata)
	{
		return dec_number<int64_t>(akey, adata, [](cJSON* ajson, int64_t& aval)->bool
			{
				if (nullptr == ajson || ajson->type != cJSON_String)
					return false;
				aval = boost::lexical_cast<int64_t>(ajson->valuestring);
				return true;
			});
	}

	bool ojson::dec(const char* akey, std::vector<uint8_t>& adata)
	{
		return dec_number<uint8_t>(akey, adata, [](cJSON* ajson, uint8_t& aval)->bool
			{
				if (nullptr == ajson || ajson->type != cJSON_Number)
					return false;
				aval = (uint8_t)ajson->valueint;
				return true;
			});
	}

	bool ojson::dec(const char* akey, std::vector<uint16_t>& adata)
	{
		return dec_number<uint16_t>(akey, adata, [](cJSON* ajson, uint16_t& aval)->bool
			{
				if (nullptr == ajson || ajson->type != cJSON_Number)
					return false;
				aval = (uint16_t)ajson->valueint;
				return true;
			});
	}
	bool ojson::dec(const char* akey, std::vector<uint32_t>& adata)
	{
		return dec_number<uint32_t>(akey, adata, [](cJSON* ajson, uint32_t& aval)->bool
			{
				if (nullptr == ajson || ajson->type != cJSON_Number)
					return false;
				aval = (uint32_t)ajson->valueint;
				return true;
			});
	}

	bool ojson::dec(const char* akey, std::vector<uint64_t>& adata)
	{
		return dec_number<uint64_t>(akey, adata, [](cJSON* ajson, uint64_t& aval)->bool
			{
				if (nullptr == ajson || ajson->type != cJSON_String)
					return false;
				aval = boost::lexical_cast<uint64_t>(ajson->valuestring);
				return true;
			});
	}

	bool ojson::dec(const char* akey, std::vector<float>& adata)
	{
		return dec_number<float>(akey, adata, [](cJSON* ajson, float& aval)->bool
			{
				if (nullptr == ajson || ajson->type != cJSON_Number)
					return false;
				aval = (float)ajson->valuedouble;
				return true;
			});
	}
	bool ojson::dec(const char* akey, std::vector<double>& adata)
	{
		return dec_number<double>(akey, adata, [](cJSON* ajson, double& aval)->bool
			{
				if (nullptr == ajson || ajson->type != cJSON_Number)
					return false;
				aval = (double)ajson->valuedouble;
				return true;
			});
	}

	bool ojson::dec(const char* akey, std::vector<bool>& adata)
	{
		return dec_number<bool>(akey, adata, [](cJSON* ajson, bool& aval)->bool
			{
				if (nullptr == ajson)
					return false;
				aval = ajson->type != cJSON_False;
				return true;
			});
	}
}// namespace ngl