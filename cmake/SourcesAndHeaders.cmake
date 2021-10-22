set(sources
	src/chip8.cpp
	src/platform.cpp
)

set(exe_sources
	app/main.cpp
	${sources}
)

set(headers
    include/chip8.hpp
	include/platform.hpp
)
