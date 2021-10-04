cmake_minimum_required(VERSION 3.20)
project(Hue_Console)

set(CMAKE_CXX_STANDARD 14)

add_executable(Hue_Console main.cpp API_Adapter.h Json_Utilities.h Hue_API.h)
