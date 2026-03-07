# 非本地跳转代码
add_executable(statcheck)

target_sources(statcheck
        PRIVATE
        statcheck.c
)
target_link_libraries(statcheck
        PRIVATE
        csapp
)