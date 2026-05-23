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