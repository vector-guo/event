INCLUDE_DIRECTORIES(.)
INCLUDE_DIRECTORIES(${LORAGW_SOURCE_DIR}/modules/include)
INCLUDE_DIRECTORIES(${LORAGW_SOURCE_DIR}/porting/include)
INCLUDE_DIRECTORIES(${LORAGW_SOURCE_DIR}/os)

PROJECT(IOTTEST)

SET(IOT_SRC  		iot.c

				)



ADD_EXECUTABLE(iot iot.c)
TARGET_LINK_LIBRARIES(iot   
			     modules
			     porting
			     os
			     pthread
				)

				

