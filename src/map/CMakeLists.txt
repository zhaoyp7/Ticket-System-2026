include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/data)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/corner_data)

find_program(MAP_VALGRIND_EXECUTABLE valgrind)

function(add_map_case case_name)
  string(REPLACE "." "_" case_suffix "${case_name}")
  set(target_name "map_${case_suffix}")
  set(run_cmd "${CMAKE_CURRENT_BINARY_DIR}/${target_name}")

  add_executable(
    ${target_name}
    ${CMAKE_CURRENT_SOURCE_DIR}/data/${case_name}/code.cpp
  )

  set_target_properties(${target_name} PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
  )

  if(case_name MATCHES "\\.memcheck$")

    target_compile_options(${target_name} PRIVATE
      -g
      -O1
      -fno-omit-frame-pointer
      ${MAP_MEMCHECK_SANITIZERS}
    )
    target_link_options(${target_name} PRIVATE
      ${MAP_MEMCHECK_SANITIZERS}
    )

    if(MAP_VALGRIND_EXECUTABLE)
      set(run_cmd
        "${MAP_VALGRIND_EXECUTABLE} --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all --error-exitcode=99 ${CMAKE_CURRENT_BINARY_DIR}/${target_name}"
      )
    endif()
  endif()

  add_test(
    NAME ${target_name}
    COMMAND sh -c
      "${run_cmd} >${CMAKE_CURRENT_BINARY_DIR}/${target_name}.out \
      && diff -u ${CMAKE_CURRENT_SOURCE_DIR}/data/${case_name}/answer.txt ${CMAKE_CURRENT_BINARY_DIR}/${target_name}.out >${CMAKE_CURRENT_BINARY_DIR}/${target_name}.diff"
  )
endfunction()

function(add_map_corner_case case_id)
  set(target_name "map_corner_${case_id}")

  add_executable(
    ${target_name}
    ${CMAKE_CURRENT_SOURCE_DIR}/corner_data/${case_id}.cpp
  )

  set_target_properties(${target_name} PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
  )

  add_test(
    NAME ${target_name}
    COMMAND sh -c
      "${CMAKE_CURRENT_BINARY_DIR}/${target_name} >${CMAKE_CURRENT_BINARY_DIR}/${target_name}.out \
      && diff -u ${CMAKE_CURRENT_SOURCE_DIR}/corner_data/${case_id}.ans ${CMAKE_CURRENT_BINARY_DIR}/${target_name}.out >${CMAKE_CURRENT_BINARY_DIR}/${target_name}.diff"
  )
endfunction()

add_map_case(one)
add_map_case(one.memcheck)
add_map_case(two)
add_map_case(two.memcheck)
add_map_case(three)
add_map_case(three.memcheck)
add_map_case(four)
add_map_case(four.memcheck)
add_map_case(five)
add_map_case(five.memcheck)

add_map_corner_case(1)
add_map_corner_case(2)
add_map_corner_case(3)
