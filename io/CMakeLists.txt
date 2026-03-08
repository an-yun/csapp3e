# 获取文件状态
add_executable(statcheck)

target_sources(statcheck
        PRIVATE
        statcheck.c
)
target_link_libraries(statcheck
        PRIVATE
        csapp
)

# 读取目录
add_executable(readdir
        readdir.c)

target_link_libraries(readdir
        PRIVATE
        csapp
)

add_executable(forkfile
        forkfile.c)

target_link_libraries(forkfile
        PRIVATE
        csapp
)
set(IO_RESOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/resource/")
# 添加一个自定义命令，在构建主目标后执行复制操作
add_custom_command(TARGET forkfile POST_BUILD
        COMMAND cp -r "${IO_RESOURCE_DIR}/*" "${CMAKE_CURRENT_BINARY_DIR}/"
        WORKING_DIRECTORY ${IO_RESOURCE_DIR}
        COMMENT "正在复制${IO_RESOURCE_DIR}到可执行文${CMAKE_CURRENT_BINARY_DIR}/"
)

