add_library(net STATIC
	src/htmlPage.cpp
	src/httpServerTSK.c
	src/modbusServerTSK.cpp
	src/radmonClient.cpp
	src/sntp.c
	)

target_include_directories(net PUBLIC
	include
	)

target_link_libraries(net lwip freemodbus)
