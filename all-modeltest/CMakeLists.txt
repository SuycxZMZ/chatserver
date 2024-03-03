cmake_minimum_required (VERSION 2.8) #要求cmake最低的版本号
project (test) # 定义当前工程名字
# set(CMAKE_BUILD_TYPE "Debug")#设置debug模式，如果没有这一行将不能调试设断点 set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)
# 设置可执行文件路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
add_executable(jsontest jsontest.cpp)
add_executable(muduotest muduotest.cpp)
target_link_libraries(muduotest muduo_net muduo_base pthread)
#进入子目录下执行 CMakeLists.txt文件 这里的lib和tests里面都有可编译的代码文件
#add_subdirectory(lib)
#add_subdirectory(tests)