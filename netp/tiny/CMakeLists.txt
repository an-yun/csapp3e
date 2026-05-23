add_executable(tiny)
target_sources(tiny
        PRIVATE
        tiny.c
)
target_link_libraries(tiny
        PRIVATE
        csapp
)

add_dependencies(tiny
        adder
        copy_resource_file
)

#复制资源依赖文件
set(SOURCE_HTTP_FILES "${CMAKE_CURRENT_SOURCE_DIR}/resource/home.html")
set(TARGET_HTTP_FILES "${CMAKE_CURRENT_BINARY_DIR}/home.html")
add_custom_target(copy_resource_file
        DEPENDS "${SOURCE_HTTP_FILES}"
        COMMAND ${CMAKE_COMMAND} -E copy "${SOURCE_HTTP_FILES}" "${TARGET_HTTP_FILES}"
        COMMENT "正在复制${CMAKE_CURRENT_SOURCE_DIR}/resource下所有内容到${CMAKE_CURRENT_BINARY_DIR}下"
        BYPRODUCTS "TARGET_HTTP_FILES"
)

add_subdirectory(cgi-bin)

