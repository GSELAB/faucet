SET(sources
    main.cpp)

SET(CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} -w")

add_executable(faucet ${sources})

target_link_libraries(faucet
    config hub server ${GSE_BOOST_LIBS})