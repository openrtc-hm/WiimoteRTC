cmake_minimum_required(VERSION 2.8)

set(comp_srcs "./src/wiimote.cpp")

include_directories("C:\\Program Files (x86)\\Microsoft Visual Studio 10.0\\VC\\include" ${WDK_INCLUDE_DIRS} "src")

link_directories( "C:\\Program Files (x86)\\Microsoft Visual Studio 10.0\\VC\\lib" ${WDK_LIBRARIES})

add_library(wiimote STATIC ${comp_srcs})