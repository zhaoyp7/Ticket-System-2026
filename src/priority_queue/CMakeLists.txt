include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/test)

function(add_priority_queue_test test_id)
  set(target_name "priority_queue_${test_id}")
  set(test_name "priority_queue-test-${test_id}")

  add_executable(
    ${target_name}
    ${CMAKE_CURRENT_SOURCE_DIR}/test/${test_id}.cpp
  )

  add_test(
    NAME ${test_name}
    COMMAND sh -c
      "${CMAKE_CURRENT_BINARY_DIR}/${target_name} >${CMAKE_CURRENT_BINARY_DIR}/${test_id}.out \
      && diff -ubB ${CMAKE_CURRENT_SOURCE_DIR}/test/${test_id}.ans ${CMAKE_CURRENT_BINARY_DIR}/${test_id}.out >${CMAKE_CURRENT_BINARY_DIR}/${test_id}.diff"  )
endfunction()

add_priority_queue_test(1)
add_priority_queue_test(2)
add_priority_queue_test(3)
add_priority_queue_test(4)
add_priority_queue_test(5)
add_priority_queue_test(6)
add_priority_queue_test(7)