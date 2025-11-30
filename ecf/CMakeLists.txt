add_executable(waitpid2)

target_sources(waitpid2
  PRIVATE
    waitpid2.c
)

target_link_libraries(waitpid2
  PRIVATE
     csapp
)

add_executable(sigint)

target_sources(sigint
        PRIVATE
        sigint.c
)
target_link_libraries(sigint
        PRIVATE
        csapp
)
