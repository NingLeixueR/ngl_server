#pragma once

#include <string>

typedef void (*DumperHandler)();

class Dumper {
	static DumperHandler dumperHandler;
public:
	Dumper();

	static void setDumperHandler(DumperHandler val) {
		dumperHandler = val;
	}
	static DumperHandler getDumperHandler() {
		return dumperHandler;
	}

	static std::string m_excname;
};




