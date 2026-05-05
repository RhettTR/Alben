cmake_minimum_required(VERSION 3.10)
project(Alben)

set(BUILD_SHARED_LIBS ON)


file(GLOB SOURCES *.cpp *.h)

include_directories(/home/me/Qt/6.8.1/gcc_64/include/QtWidgets)
include_directories(/home/me/Qt/6.8.1/gcc_64/include/QtNetwork)



add_executable(main ${SOURCES})


target_include_directories(main PRIVATE /home/me/luau/VM/include) 
target_include_directories(main PRIVATE /home/me/luau/Compiler/include)
target_include_directories(main PRIVATE /home/me/Qt/6.8.1/gcc_64/include)
target_include_directories(main PRIVATE /home/me/Qt/6.8.1/gcc_64/include/QtSvg)


target_link_libraries(main -Wl,--copy-dt-needed-entries)
target_link_libraries(main -L/home/me/Qt/6.8.1/gcc_64/lib -lQt6Core -lQt6Widgets -lQt6Network)
target_link_libraries(main -L/home/me/Qt/6.8.1/gcc_64/plugins/imageformats -lQt6Svg)
target_link_libraries(main -L/home/me/luau -lLuau.VM -lLuau.Compiler -lLuau.Ast -lisocline)

