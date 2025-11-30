add_executable(waitpid2)

target_sources(waitpid2
  PRIVATE
    waitpid2.c
)

target_link_libraries(waitpid2
  PRIVATE
     csapp
)
# ctrl c不安全版本
add_executable(sigint)

target_sources(sigint
        PRIVATE
        sigint.c
)
target_link_libraries(sigint
        PRIVATE
        csapp
)

# ctrl c不安全版本
add_executable(sigintsafe)

target_sources(sigintsafe
        PRIVATE
        sigintsafe.c
)
target_link_libraries(sigintsafe
        PRIVATE
        csapp
)


# 多信号处理代码
add_executable(signal1)

target_sources(signal1
        PRIVATE
        signal1.c
)
target_link_libraries(signal1
        PRIVATE
        csapp
)

# 安全的多信号处理代码
add_executable(signal2)

target_sources(signal2
        PRIVATE
        signal2.c
)
target_link_libraries(signal2
        PRIVATE
        csapp
)