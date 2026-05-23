# cgi-bin 的依赖 http://192.168.1.4:12080/cgi-bin/adder?15000&123
add_executable(adder)

target_sources(adder
        PRIVATE
        adder.c
)
target_link_libraries(adder
        PRIVATE
        csapp
)