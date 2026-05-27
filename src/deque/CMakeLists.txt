if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    cmake_minimum_required(VERSION 3.10)
    project(STLiteDequeStdTests)
    include(CTest)
    enable_testing()
    set(CMAKE_CXX_STANDARD 20)
endif()

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/data)

function(add_deque_case case_name)
    string(REPLACE "." "_" case_suffix "${case_name}")
    set(target_name "deque_${case_suffix}")

    add_executable(
        ${target_name}
        ${CMAKE_CURRENT_SOURCE_DIR}/data/${case_name}/code.cpp
    )

    add_test(
        NAME ${target_name}
        COMMAND sh -c
                "${CMAKE_CURRENT_BINARY_DIR}/${target_name} >/tmp/${target_name}_out.txt && diff -u ${CMAKE_CURRENT_SOURCE_DIR}/data/${case_name}/answer.txt /tmp/${target_name}_out.txt >/tmp/${target_name}_diff.txt"
    )
endfunction()

add_deque_case(one)
add_deque_case(one.memcheck)
add_deque_case(two)
add_deque_case(two.memcheck)
add_deque_case(three)
add_deque_case(three.memcheck)
add_deque_case(four)
add_deque_case(four.memcheck)
add_deque_case(five)
add_deque_case(six)
