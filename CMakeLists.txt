cmake_minimum_required (VERSION 3.8)

project ("CMakeProject1")


set(TARGETNAME "target")

add_executable(${TARGETNAME} "")

file(GLOB TARGET_SRCS *.h *.cpp *.c)
target_sources(
	${TARGETNAME} PRIVATE
	${TARGET_SRCS}
)
