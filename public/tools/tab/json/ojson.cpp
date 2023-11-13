#include "ojson.h"


namespace ngl
{

	ojson::ojson(const char* astr) :
		json(cJSON_Parse(astr)),
		m_free(true)
	{}

	ojson::ojson() :
		json(),
		m_free(false)
	{}

	ojson::~ojson()
	{
		if (m_json != nullptr && m_free)
			cJSON_Delete(m_json);
	}


	ojson& ojson::operator >> (std::pair<const char*, int8_t>& adata)
	{
		return _fun_number(adata);
	}

	ojson& ojson::operator >> (std::pair<const char*, int16_t>& adata)
	{
		return _fun_number(adata);
	}

	ojson& ojson::operator >> (std::pair<const char*, int32_t>& adata)
	{
		return _fun_number(adata);
	}

	ojson& ojson::operator >> (std::pair<const char*, int64_t>& adata)
	{
		return _fun_number(adata);
	}

	ojson& ojson::operator >> (std::pair<const char*, uint8_t>& adata) 
	{
		return _fun_number(adata);
	}

	ojson& ojson::operator >> (std::pair<const char*, uint16_t>& adata) 
	{
		return _fun_number(adata);
	}

	ojson& ojson::operator >> (std::pair<const char*, uint32_t>& adata) 
	{
		return _fun_number(adata);
	}

	ojson& ojson::operator >> (std::pair<const char*, uint64_t>& adata)
	{
		return _fun_number(adata);
	}

	ojson& ojson::operator >> (std::pair<const char*, float>& adata)
	{
		cJSON* ret = cJSON_GetObjectItem(m_json, adata.first);
		if (nullptr == ret || ret->type != cJSON_Number)
			return *this;
		adata.second = (float)ret->valuedouble;
		return *this;
	}

	ojson& ojson::operator >> (std::pair<const char*, double>& adata)
	{
		cJSON* ret = cJSON_GetObjectItem(m_json, adata.first);
		if (nullptr == ret || ret->type != cJSON_Number)
			return *this;
		adata.second = ret->valuedouble;
		return *this;
	}

	ojson& ojson::operator >> (std::pair<const char*, const char*>& adata)
	{
		cJSON* ret = cJSON_GetObjectItem(m_json, adata.first);
		if (nullptr == ret || ret->type != cJSON_String)
			return *this;
		adata.second = ret->valuestring;
		return *this;
	}

	ojson& ojson::operator >> (std::pair<const char*, bool>& adata) 
	{
		cJSON* ret = cJSON_GetObjectItem(m_json, adata.first);
		if (nullptr == ret)
			return *this;
		if (ret->type != cJSON_False)
		{
			adata.second = false;
		}
		if (ret->type != cJSON_True)
		{
			adata.second = true;
		}
		return *this;
	}

	ojson& ojson::operator >> (std::pair<const char*, json>& adata) 
	{
		cJSON* ret = cJSON_GetObjectItem(m_json, adata.first);
		if (nullptr == ret || ret->type != cJSON_Object)
			return *this;
		adata.second.set(ret);
		return *this;
	}

	ojson& ojson::operator >> (std::pair<const char*, cJSON*>& adata) 
	{
		cJSON* ret = cJSON_GetObjectItem(m_json, adata.first);
		if (nullptr == ret)
			return *this;
		if (ret->type == cJSON_Object || ret->type == cJSON_Array)
			adata.second = ret;
		return *this;
	}



}//namespace ngl