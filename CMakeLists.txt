cmake_minimum_required(VERSION 3.20)

set(PEONYSWORD "PeonySword")

project(${PEONYSWORD} LANGUAGES CXX)

add_executable(${PEONYSWORD} "")

file(GLOB PEONYSWORD_SRCS *.c *.cpp *.h)
target_sources(
        ${PEONYSWORD} PRIVATE
        ${PEONYSWORD_SRCS}
)

