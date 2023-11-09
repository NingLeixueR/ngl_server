#include "ijson.h"

namespace ngl
{
	ijson::ijson() :
		json(cJSON_CreateObject()),
		m_nonformatstr(nullptr),
		m_str(nullptr),
		m_isnonformatstr(false),
		m_free(true)
	{
	}

	ijson::ijson(bool _):
		json(cJSON_CreateObject()),
		m_nonformatstr(nullptr),
		m_str(nullptr),
		m_isnonformatstr(false),
		m_free(false)
	{
	}


	void free_json(const char* m_str)
	{
		free((void*)m_str);
	}

	ijson::~ijson()
	{
		free_nonformatstr();
		free_str();

		if (m_json != nullptr && m_free)
		{
			cJSON_Delete(m_json);
			m_json = nullptr;
		}

	}

	cJSON* ijson::get()
	{
		return m_json;
	}

	ijson& ijson::operator << (const std::pair<const char*, int8_t>& adata)
	{
		cJSON_AddItemToObject(m_json, adata.first, cJSON_CreateNumber(adata.second));  
		return *this;
	}

	ijson& ijson::operator << (const std::pair<const char*, int16_t>& adata)
	{
		cJSON_AddItemToObject(m_json, adata.first, cJSON_CreateNumber(adata.second));  
		return *this;
	}

	ijson& ijson::operator << (const std::pair<const char*, int32_t>& adata)
	{
		cJSON_AddItemToObject(m_json, adata.first, cJSON_CreateNumber(adata.second));  
		return *this;
	}

	ijson& ijson::operator << (const std::pair<const char*, int64_t>& adata)
	{
		cJSON_AddItemToObject(m_json, adata.first, cJSON_CreateNumber(adata.second));
		return *this;
	}

	ijson& ijson::operator << (const std::pair<const char*, uint8_t>& adata)
	{ 
		cJSON_AddItemToObject(m_json, adata.first, cJSON_CreateNumber(adata.second));  
		return *this; 
	}

	ijson& ijson::operator << (const std::pair<const char*, uint16_t>& adata)
	{ 
		cJSON_AddItemToObject(m_json, adata.first, cJSON_CreateNumber(adata.second));  
		return *this; 
	}

	ijson& ijson::operator << (const std::pair<const char*, uint32_t>& adata)
	{ 
		cJSON_AddItemToObject(m_json, adata.first, cJSON_CreateNumber(adata.second));  
		return *this; 
	}

	ijson& ijson::operator << (const std::pair<const char*, uint64_t>& adata)
	{
		cJSON_AddItemToObject(m_json, adata.first, cJSON_CreateNumber(adata.second));
		return *this;
	}

	ijson& ijson::operator << (const std::pair<const char*, float>& adata)
	{ 
		cJSON_AddItemToObject(m_json, adata.first, cJSON_CreateNumber(adata.second));  
		return *this; 
	}

	ijson& ijson::operator << (const std::pair<const char*, double>& adata)
	{ 
		cJSON_AddItemToObject(m_json, adata.first, cJSON_CreateNumber(adata.second));  
		return *this; 
	}

	ijson& ijson::operator << (const std::pair<const char*, const char*>& adata)
	{ 
		cJSON_AddItemToObject(m_json, adata.first, cJSON_CreateString(adata.second)); 
		return *this; 
	}

	ijson& ijson::operator << (const std::pair<const char*, bool>& adata)
	{ 
		cJSON_AddItemToObject(m_json, adata.first, adata.second ? cJSON_CreateTrue() : cJSON_CreateFalse()); 
		return *this;
	}

	ijson& ijson::operator << (const std::pair<const char*, json*>& adata)
	{ 
		std::pair<const char*, cJSON*> lpair(adata.first, adata.second->get());
		this->operator<<(lpair);
		return *this; 
	}

	ijson& ijson::operator << (const std::pair<const char*, cJSON*>& adata)
	{
		cJSON_AddItemToObject(m_json, adata.first, adata.second);
		return *this;
	}

	ijson& ijson::operator << (const std::pair<const char*, std::string>& adata)
	{
		std::pair<const char*, const char*> lpair(adata.first, adata.second.c_str());
		*this << lpair;
		return *this;
	}

	ijson& ijson::operator >> (std::string& adata)
	{
		if (m_isnonformatstr)
		{
			free_nonformatstr();
			m_nonformatstr = cJSON_PrintUnformatted(m_json);
			adata = m_nonformatstr;
		}
		else
		{
			free_str();
			m_str = cJSON_Print(m_json);
			adata = m_str;
		}
		return *this;
	}

	void ijson::set_nonformatstr(bool abool)
	{
		m_isnonformatstr = abool;
	}

}//namespace ngl