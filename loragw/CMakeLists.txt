INCLUDE_DIRECTORIES(.)
INCLUDE_DIRECTORIES(${LORAGW_SOURCE_DIR}/modules/include)
INCLUDE_DIRECTORIES(${LORAGW_SOURCE_DIR}/porting/include)

PROJECT(LORA)

SET(LORA_SRC  		loragw.c

				)



ADD_EXECUTABLE(lora loragw.c)
TARGET_LINK_LIBRARIES(lora   
			     modules
			     porting
			     os
			     pthread
				)

				

