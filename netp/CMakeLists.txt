# 非本地跳转代码
add_executable(hostinfo)

target_sources(hostinfo
        PRIVATE
        hostinfo.c
)
target_link_libraries(hostinfo
        PRIVATE
        csapp
)

# cgi-bin 的依赖 http://192.168.1.4:12080/cgi-bin/adder?15000&123
add_executable(adder)

target_sources(adder
        PRIVATE
        tiny/cgi-bin/adder.c
)
target_link_libraries(adder
        PRIVATE
        csapp
)
set(CGI_BIN_DIR "${CMAKE_CURRENT_BINARY_DIR}/cgi-bin")
add_custom_command(TARGET adder POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CGI_BIN_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/adder" "${CGI_BIN_DIR}"
        COMMAND ${CMAKE_COMMAND} -E echo "成功复制${CMAKE_CURRENT_BINARY_DIR}/adder到${CGI_BIN_DIR}下"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/tiny/resource" "${CMAKE_CURRENT_BINARY_DIR}"
        COMMAND ${CMAKE_COMMAND} -E echo "成功复制${CMAKE_CURRENT_SOURCE_DIR}/tiny/resource下所有内容到${CMAKE_CURRENT_BINARY_DIR}下"
)

add_executable(tiny)
target_sources(tiny
        PRIVATE
        tiny/tiny.c
)
target_link_libraries(tiny
        PRIVATE
        csapp
)
add_dependencies(tiny adder)
