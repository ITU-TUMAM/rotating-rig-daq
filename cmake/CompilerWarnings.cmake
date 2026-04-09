function(set_project_warnings target)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE
            -Wall -Wextra -Wpedantic -Wshadow -Wconversion
            -Wnon-virtual-dtor -Wold-style-cast -Wcast-align
            -Woverloaded-virtual -Wnull-dereference -Wdouble-promotion
        )
    elseif(MSVC)
        target_compile_options(${target} PRIVATE /W4 /permissive-)
    endif()
endfunction()
