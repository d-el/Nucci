add_library(dev STATIC
	src/24AAxx.cpp
	src/display.c
	src/graphics.c
	src/rt9466.cpp
	src/st7735.c
	)

target_include_directories(dev PUBLIC
	include
	)

target_link_libraries(dev freertos drivers utils)
