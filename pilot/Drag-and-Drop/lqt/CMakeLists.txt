#  Copyright (c) 2008 Peter Kuemmel 
#  Copyright (c) 2008 Mauro Iazzi
#  Copyright (c) 2011 Michal Kottman
#  
#  Permission is hereby granted, free of charge, to any person
#  obtaining a copy of this software and associated documentation
#  files (the "Software"), to deal in the Software without
#  restriction, including without limitation the rights to use,
#  copy, modify, merge, publish, distribute, sublicense, and/or sell
#  copies of the Software, and to permit persons to whom the
#  Software is furnished to do so, subject to the following
#  conditions:
#  
#  The above copyright notice and this permission notice shall be
#  included in all copies or substantial portions of the Software.
#  
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
#  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
#  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
#  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
#  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
#  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
#  OTHER DEALINGS IN THE SOFTWARE.
#
# TODO:
#	[ ] C++ rref parse (Type&& value)
#	[ ] verbose print support
#	[ ] shell class template constructor support
#	[ ] lqtL_toboolref value ret

cmake_minimum_required(VERSION 3.1.0)

project(lqt5)

set(REUSE_FILES 1)
set(CMAKE_BUILD_TYPE Debug)

set(Qt5Script_DIR /home/test/projects/alben/build-qt5/qtbase/lib/cmake/Qt5Script)
set(Qt5ScriptTools_DIR /home/test/projects/alben/build-qt5/qtbase/lib/cmake/Qt5ScriptTools)
set(Qt5OpenGL_DIR /home/test/projects/alben/build-qt5/qtbase/lib/cmake/Qt5OpenGL)
set(Qt5UiTools_DIR /home/test/projects/alben/build-qt5/qtbase/lib/cmake/Qt5UiTools)
set(Qt5WebSockets_DIR /home/test/projects/alben/build-qt5/qtbase/lib/cmake/Qt5WebSockets)


set(CMAKE_SOURCE_DIR /home/test/projects/alben/lqt)
set(CMAKE_BINARY_DIR /home/test/projects/alben/build-lqt)



set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/bin) 
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/lib) 
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/lib) 

set(LUA_INCLUDE_DIR ${CMAKE_BINARY_DIR}/../lqt/modules/src/LuaJIT/src)
set(CMAKE_PROGRAM_PATH ${LUA_INCLUDE_DIR})


set(CMAKE_PREFIX_PATH /home/test/projects/alben/build-qt5/qtbase/lib/cmake)
set(LUA_LIBRARIES ${LUA_INCLUDE_DIR}/libluajit.so)


set(QT_SCHEMA_DIR ${CMAKE_BINARY_DIR}/../lqt/generator/schema)

find_package(Qt5 REQUIRED
	Core 
	Sql
	Positioning
	Script
	ScriptTools
	Gui
	Widgets
	OpenGL
	PrintSupport
	UiTools
)


set(VERBOSE_BUILD 1)

set(EXTRA_LUA_OPTIONS )
option(VERBOSE_BUILD "Create a verbose build which logs every binding call" ON)
if(VERBOSE_BUILD)
	add_definitions(-DVERBOSE_BUILD)
	set(EXTRA_LUA_OPTIONS -v)
endif()



find_package(LuaJiT REQUIRED)

find_program(LUA_CMD NAMES luajit)
message("-- Lua program is : ${LUA_CMD}")



set(CMAKE_SHARED_MODULE_CREATE_C_FLAGS "${CMAKE_SHARED_MODULE_CREATE_C_FLAGS} -flat_namespace -undefined dynamic_lookup")
set(CMAKE_SHARED_MODULE_CREATE_CXX_FLAGS "${CMAKE_SHARED_MODULE_CREATE_CXX_FLAGS} -flat_namespace -undefined dynamic_lookup")

add_subdirectory(cpptoxml)


	
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -std=c++11 -Wno-deprecated-declarations")


function(build_qt_lib_binding BINDING_HEADER)

	string(TOLOWER ${BINDING_HEADER} BINDING_NAME)

	# hardcoded in generator.lua
	set (LQT_GEN_DIR ${CMAKE_BINARY_DIR}/${BINDING_NAME}_src)
	file(MAKE_DIRECTORY ${LQT_GEN_DIR})

	set (BINDING_SRC ${LQT_GEN_DIR}/${BINDING_NAME}_merged_build.cpp)
	set_source_files_properties(${BINDING_SRC} PROPERTIES  GENERATED TRUE)

	set (PP_CONFIG ${QT_SCHEMA_DIR}/lqt5_macos.lqt)
	

	set(REUSE_FILES 0)

	if(NOT REUSE_FILES)
		set (GEN_XML ${LQT_GEN_DIR}/${BINDING_NAME}.xml)
		add_custom_target(generate_${BINDING_NAME}_xml
							COMMAND cpptoxml
								${BINDING_HEADER}
								-C ${PP_CONFIG}
								-Q ${QT_SCHEMA_DIR} 
								-o ${GEN_XML}
							COMMENT "Generating XML: running cpptoxml on ${BINDING_NAME} "
							VERBATIM)

		math(EXPR MAX ${ARGC}-1)
		if(MAX)
			foreach(I RANGE 1 ${MAX})
				string(TOLOWER ${ARGV${I}} DEPNAME)
				set(${BINDING_NAME}_TYPES ${${BINDING_NAME}_TYPES} -t ${DEPNAME}_src/${DEPNAME}_types.lua)
				set(${BINDING_NAME}_DEPENDS ${${BINDING_NAME}_DEPENDS} generate_${DEPNAME}_cpp)
			endforeach()
		endif()

		add_custom_target(generate_${BINDING_NAME}_cpp
							COMMAND ${LUA_CMD}
								"${CMAKE_SOURCE_DIR}/generator/generator.lua" 
								${GEN_XML}
								${EXTRA_LUA_OPTIONS}
								-i ${BINDING_HEADER} -i lqt_qt.hpp 
								-i "${QT_LIBRARIES}"
								-n ${BINDING_NAME} 
								-t ${CMAKE_SOURCE_DIR}/generator/qtypes.lua
								${${BINDING_NAME}_TYPES}
								-f ${CMAKE_SOURCE_DIR}/generator/qt_internal.lua
							COMMENT "Generating binding code with Lua"
							VERBATIM)
	else()
		add_custom_target(generate_${BINDING_NAME}_xml 	COMMAND
							COMMENT "Reusing old XML on ${BINDING_NAME} "
							VERBATIM)
		add_custom_target(generate_${BINDING_NAME}_cpp
							COMMENT "Reusing old binding code with Lua"
							VERBATIM)
	endif()

	set(COMMON COMMON_${BINDING_NAME})
	file(GLOB ${COMMON}_SRC ${CMAKE_SOURCE_DIR}/common/*.cpp)
	file(GLOB ${COMMON}_HPP ${CMAKE_SOURCE_DIR}/common/*.hpp)
	file(GLOB ${COMMON}_HPP ${CMAKE_SOURCE_DIR}/common/*.inl)

	set(${BINDING_NAME}_GENERATED ${LQT_GEN_DIR}/${BINDING_NAME}_enum.cpp
									${LQT_GEN_DIR}/${BINDING_NAME}_meta.cpp
									${LQT_GEN_DIR}/${BINDING_NAME}_globals.cpp
									${LQT_GEN_DIR}/${BINDING_NAME}_slot.cpp)
	set_source_files_properties(${${BINDING_NAME}_GENERATED} PROPERTIES GENERATED TRUE)

	if(nomerge)
		#TODO fix this build process:
		#	1. cmake ../lqt -Dnomerge=1
		#	2. make -> will generate cpp files but they are not part of the Makefile (GLOB was at 1.)
		# 	3. cmake ../lqt -Dnomerge=1 -> GLOB now finds the generated cpp files
		#	4. make
		file(GLOB BINDING_SRC ${LQT_GEN_DIR}/*.cpp) 
		set(${COMMON}_SRC)
		list(REMOVE_ITEM BINDING_SRC ${LQT_GEN_DIR}/${BINDING_NAME}_merged_build.cpp)
	else()				
		set(${COMMON}_SRC ${${COMMON}_SRC}	${${BINDING_NAME}_GENERATED})
	endif()

	qt5_wrap_cpp(${COMMON}_MOC ${LQT_GEN_DIR}/${BINDING_NAME}_slot.hpp)

	include_directories(${CMAKE_SOURCE_DIR}/common ${LUA_INCLUDE_DIR} ${QT_INCLUDES})

	add_library(${BINDING_NAME} MODULE ${BINDING_SRC}
											${${COMMON}_SRC}
											${${COMMON}_HPP} 
											${${COMMON}_MOC})

	qt5_use_modules(${BINDING_NAME} ${QT_LIBRARIES})
											
	target_link_libraries(${BINDING_NAME} ${LUA_LIBS_FOR_MODULE})
	set_target_properties(${BINDING_NAME} PROPERTIES PREFIX "" COMPILE_DEFINITIONS "MODULE_${BINDING_NAME}")

	if(LQT_BUILDIN_CPPTOXML)
		add_dependencies(generate_${BINDING_NAME}_xml  cpptoxml)
	endif()

	add_dependencies(generate_${BINDING_NAME}_cpp  generate_${BINDING_NAME}_xml ${${BINDING_NAME}_DEPENDS})
	add_dependencies(${BINDING_NAME} generate_${BINDING_NAME}_cpp)
endfunction()


set(REUSE_FILES 1)
set(QT_LIBRARIES Core)
build_qt_lib_binding(QtCore)

set(REUSE_FILES 1)
set(QT_LIBRARIES Sql Core)
build_qt_lib_binding(QtSql QtCore)

set(REUSE_FILES 1)
set(QT_LIBRARIES Positioning Core)
build_qt_lib_binding(QtPositioning QtCore)

set(REUSE_FILES 1)
set(QT_LIBRARIES Script Core)
build_qt_lib_binding(QtScript QtCore)

set(REUSE_FILES 1)
set(QT_LIBRARIES ScriptTools Widgets Gui Script Core)
build_qt_lib_binding(QtScriptTools QtWidgets QtGui QtScript QtCore)

set(REUSE_FILES 1)
set(QT_LIBRARIES Gui Core)
build_qt_lib_binding(QtGui QtCore)

set(REUSE_FILES 1)
set(QT_LIBRARIES Widgets Gui Core)
build_qt_lib_binding(QtWidgets QtGui QtCore)

set(REUSE_FILES 1)
set(QT_LIBRARIES OpenGL Widgets Gui Core)
build_qt_lib_binding(QtOpenGL QtWidgets QtGui QtCore)

set(REUSE_FILES 1)
set(QT_LIBRARIES PrintSupport Widgets Gui Core)
build_qt_lib_binding(QtPrintSupport QtWidgets QtGui QtCore)

set(REUSE_FILES 1)
set(QT_LIBRARIES UiTools Widgets Gui Core)
build_qt_lib_binding(QtUiTools QtWidgets QtGui QtCore)



add_subdirectory(modules/lrdb)
add_subdirectory(modules/lpeglabel)
add_subdirectory(modules/lfs)
add_subdirectory(modules/lpath)
add_subdirectory(modules/lua-miniz)
add_subdirectory(modules/luautf8)
add_subdirectory(modules/luagbk)
add_subdirectory(modules/luasocket)
add_subdirectory(modules/lanes)
