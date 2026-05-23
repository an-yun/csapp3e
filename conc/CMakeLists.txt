add_executable(badcnt)

target_sources(badcnt
  PRIVATE
        badcnt.c
)

target_link_libraries(badcnt
  PRIVATE
     csapp
)
# 正确的并发版本
add_executable(goodcnt)

target_sources(goodcnt
        PRIVATE
        goodcnt.c
)
target_link_libraries(goodcnt
        PRIVATE
        csapp
)
# IO多路复用的并发事件驱动服务器
add_executable(select)

target_sources(select
        PRIVATE
        select.c
)
target_link_libraries(select
        PRIVATE
        csapp
)

# 预线程化的并发服务器
add_executable(echoservert_pre)

target_sources(echoservert_pre
        PRIVATE
        echoservert_pre.c
        echo_cnt.c
)
target_link_libraries(echoservert_pre
        PRIVATE
        csapp
)
