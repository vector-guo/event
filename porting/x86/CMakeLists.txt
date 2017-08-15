INCLUDE_DIRECTORIES(${LORAGW_SOURCE_DIR}/porting/include)

PROJECT(porting)

SET(PORTING_SRC  	  spi_op.c

				)



ADD_LIBRARY(porting STATIC ${PORTING_SRC})

				

