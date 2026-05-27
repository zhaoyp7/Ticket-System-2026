cmake_minimum_required(VERSION 3.22.1)
project(Ticket-System-2026)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(-Wall)
add_compile_options(-Ofast)

add_executable(code 
    src/vector/src/exceptions.hpp
    src/vector/src/utility.hpp
    src/vector/src/vector.hpp
    src/map/src/exceptions.hpp
    src/map/src/utility.hpp
    src/map/src/map.hpp
    src/priority_queue/include/exceptions.hpp
    src/priority_queue/include/utility.hpp
    src/priority_queue/include/priority_queue.hpp
    src/deque/exceptions.hpp
    src/deque/utility.hpp
    src/deque/deque.hpp
    
    src/MemoryRiver.hpp
    src/command.hpp
    src/my_sort.hpp
    src/BPT.hpp
    src/utils.hpp
    src/User.hpp
    src/Train.hpp
    src/Ticket.hpp

    main.cpp
)