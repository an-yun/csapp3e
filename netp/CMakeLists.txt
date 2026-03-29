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