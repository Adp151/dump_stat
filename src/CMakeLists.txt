set(SRC 
    DumpWorker.c
    HashTable.c
    main.c
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(dump_stat ${SRC})