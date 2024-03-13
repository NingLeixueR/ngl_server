#include "ojson.h"

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

	bool ojson::operator >> (std::pair<const char*, std::string>& adata)
	{
		cJSON* ret = cJSON_GetObjectItem(m_json, adata.first);
		if (nullptr == ret || ret->type != cJSON_String)
			return false;
		//memcpy(&adata.second, &ret->valuedouble, sizeof(T));
		adata.second = ret->valuestring;
		return true;
	}

	bool ojson::operator >> (std::pair<const char*, int8_t>& adata)
	{
		return _fun_number32(adata);
	}

	bool ojson::operator >> (std::pair<const char*, int16_t>& adata)
	{
		return _fun_number32(adata);
	}

	bool ojson::operator >> (std::pair<const char*, int32_t>& adata)
	{
		return _fun_number32(adata);
	}

	bool ojson::operator >> (std::pair<const char*, int64_t>& adata)
	{
		return _fun_number64(adata);
	}

	bool ojson::operator >> (std::pair<const char*, uint8_t>& adata)
	{
		return _fun_number32(adata);
	}

	bool ojson::operator >> (std::pair<const char*, uint16_t>& adata)
	{
		return _fun_number32(adata);
	}

	bool ojson::operator >> (std::pair<const char*, uint32_t>& adata)
	{
		return _fun_number32(adata);
	}

	bool ojson::operator >> (std::pair<const char*, uint64_t>& adata)
	{
		return _fun_number64(adata);
	}

	bool ojson::operator >> (std::pair<const char*, float>& adata)
	{
		cJSON* ret = cJSON_GetObjectItem(m_json, adata.first);
		if (nullptr == ret || ret->type != cJSON_Number)
			return false;
		adata.second = (float)ret->valuedouble;
		return true;
	}

	bool ojson::operator >> (std::pair<const char*, double>& adata)
	{
		cJSON* ret = cJSON_GetObjectItem(m_json, adata.first);
		if (nullptr == ret || ret->type != cJSON_Number)
			return false;
		adata.second = ret->valuedouble;
		return true;
	}

	bool ojson::operator >> (std::pair<const char*, const char*>& adata)
	{
		cJSON* ret = cJSON_GetObjectItem(m_json, adata.first);
		if (nullptr == ret || ret->type != cJSON_String)
			return false;
		adata.second = ret->valuestring;
		return true;
	}

	bool ojson::operator >> (std::pair<const char*, bool>& adata)
	{
		cJSON* ret = cJSON_GetObjectItem(m_json, adata.first);
		if (nullptr == ret)
			return false;
		if (ret->type != cJSON_False)
		{
			adata.second = false;
		}
		if (ret->type != cJSON_True)
		{
			adata.second = true;
		}
		return true;
	}

	bool ojson::operator >> (std::pair<const char*, cJSON*>& adata) 
	{
		cJSON* ret = cJSON_GetObjectItem(m_json, adata.first);
		if (nullptr == ret)
			return false;
		if (ret->type == cJSON_Object || ret->type == cJSON_Array)
			adata.second = ret;
		return true;
	}
}// namespace ngl