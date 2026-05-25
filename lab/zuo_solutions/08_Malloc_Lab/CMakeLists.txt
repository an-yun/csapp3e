add_executable(mdriver
        mdriver.c
        memlib.c
        mm.c
        fsecs.c
        fcyc.c
        ftimer.c
        clock.c
        )

add_dependencies(mdriver
        copy_test_file
)

#复制资源依赖文件
set(SOURCE_TEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.rep")
set(SOURCE_TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/traces")
set(BINARY_TEST_DIR "${CMAKE_CURRENT_BINARY_DIR}/traces")

add_custom_target(copy_test_file
        DEPENDS "${SOURCE_HTTP_FILES}"
        COMMAND ${CMAKE_COMMAND} -E copy "${SOURCE_TEST_FILES}" "${CMAKE_CURRENT_BINARY_DIR}/"
        COMMAND ${CMAKE_COMMAND} -E make_directory  "${BINARY_TEST_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${SOURCE_TEST_DIR}" "${BINARY_TEST_DIR}"
        COMMENT "正在复制${CMAKE_CURRENT_SOURCE_DIR}下所有rep文件到${CMAKE_CURRENT_BINARY_DIR}下"
        BYPRODUCTS "TARGET_HTTP_FILES"
)