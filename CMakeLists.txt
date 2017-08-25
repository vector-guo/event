set(CMAKE_C_COMPILER ${CROSS_COMPILE}gcc)


cmake_minimum_required (VERSION 2.6)

message(" <--- C ---> " ${CMAKE_C_COMPILER})


PROJECT(LORAGW)

# 编译显示报警信息
#add_definitions(-Wall)

# 添加调试信息(gdb)
add_definitions(-g)



ADD_SUBDIRECTORY(os)
ADD_SUBDIRECTORY(porting/${BOARD})
ADD_SUBDIRECTORY(modules)
ADD_SUBDIRECTORY(loragw)
ADD_SUBDIRECTORY(iottest)