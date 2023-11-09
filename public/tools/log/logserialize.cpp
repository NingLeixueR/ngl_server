#include "logserialize.h"

namespace ngl
{
	logserialize::logserialize(char* abuff, int abufflen) :
		m_buff(abuff),
		m_bufflen(abufflen)
	{}

	char* logserialize::buff() 
	{
		return m_buff; 
	}

	int logserialize::len() 
	{ 
		return m_bufflen; 
	}

	bool logserialize::operator()(int64_t avalue)
	{
		return number("%lld", avalue);
	}

	bool logserialize::operator()(int32_t avalue)
	{
		return number("%d", avalue);
	}

	bool logserialize::operator()(int16_t avalue)
	{
		return number<int32_t>("%d", avalue);
	}

	bool logserialize::operator()(int8_t avalue)
	{
		return number<int32_t>("%d", avalue);
	}

	bool logserialize::operator()(uint64_t avalue)
	{
		return number("%lld", avalue);
	}

	bool logserialize::operator()(uint32_t avalue)
	{
		return number("%d", avalue);
	}

	bool logserialize::operator()(uint16_t avalue)
	{
		return number("%d", avalue);
	}

	bool logserialize::operator()(uint8_t avalue)
	{
		return number<int32_t>("%d", avalue);
	}

	bool logserialize::operator()(char avalue)
	{
		return number("%c", avalue);
	}

	bool logserialize::operator()(float avalue)
	{
		return number("%f", avalue);
	}

	bool logserialize::operator()(double avalue)
	{
		return number("%lf", avalue);
	}

	bool logserialize::operator()(char* avalue)
	{
		return (*this)((const char*)avalue);
	}

	bool logserialize::operator()(const std::string& avalue)
	{
		return (*this)(avalue.c_str());
	}

	bool logserialize::operator()(bool avalue)
	{
		int llen = (avalue ? sizeof("true") : sizeof("false")) - 1;
		const char* lp = avalue ? "true" : "false";
		if (m_bufflen <= llen) return false;
		memcpy(m_buff, lp, llen);
		m_buff += llen;
		m_bufflen -= llen;
		return true;
	}

	bool logserialize::operator()(const char* avalue)
	{
		int llen = (int)strlen(avalue);
		if (llen >= m_bufflen) return false;
		memcpy(m_buff, avalue, llen);
		m_buff += llen;
		m_bufflen -= llen;
		return true;
	}

	
}