cmake_minimum_required(VERSION 3.5)

add_subdirectory(libnglcore)

IF (CMAKE_SYSTEM_NAME MATCHES "Windows")
	add_subdirectory(rebuild)
	add_subdirectory(makeproto)
	add_subdirectory(idl)
	
	#release 链接库生成pdb
	SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
	#lib的 release链接库生成pdb
	SET(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
	#exe的 release生成pdb
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Windows")

add_subdirectory(node)

