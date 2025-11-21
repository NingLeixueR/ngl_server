#pragma once

#include <functional>
#include <string>

class Dumper 
{
	static std::function<void()> m_callback;
public:
	Dumper();

	static void setDumperHandler(const std::function<void()>& afun)
	{
		m_callback = afun;
	}

	static const std::function<void()>& getDumperHandler()
	{
		return m_callback;
	}

	static std::string m_excname;
};




