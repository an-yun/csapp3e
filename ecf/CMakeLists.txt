add_executable(waitpid2)

target_sources(waitpid2
  PRIVATE
    waitpid2.c
)

target_link_libraries(waitpid2
  PRIVATE
     csapp
)

