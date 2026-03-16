add_library(storage STATIC snapshot_store.cpp)

target_compile_features(storage PUBLIC cxx_std_23)
target_include_directories(storage PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(storage PUBLIC core)
