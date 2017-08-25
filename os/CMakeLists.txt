INCLUDE_DIRECTORIES(.)


PROJECT(OSLIB)

SET(OSLIB_SRC  		os_log.c
			os_time.c
			os_thread.c
			os_event.c
			os_epoll.c
			os_socket.c
			os_modelA.c
			os_heap.c
			
				)


ADD_LIBRARY(os STATIC ${OSLIB_SRC})

ADD_EXECUTABLE(testos test_os.c)
TARGET_LINK_LIBRARIES(testos os
			     pthread
				)

				

