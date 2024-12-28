set(LIB_TINYBVH_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(LIB_TINYBVH_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(LIB_TINYBVH_SOURCE "${LIB_TINYBVH_SOURCE_DIR}/tiny_bvh_impl.cpp")

add_library(tinybvh STATIC ${LIB_TINYBVH_SOURCE})
target_compile_options(tinybvh PRIVATE -w) # supress all warnings
target_include_directories(tinybvh
            SYSTEM INTERFACE ${LIB_TINYBVH_INCLUDE_DIR}
            PRIVATE ${LIB_TINYBVH_INCLUDE_DIR})
