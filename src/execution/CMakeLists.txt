add_library(execution STATIC order_manager.cpp broker_adaptor.cpp)

target_compile_features(execution PUBLIC cxx_std_23)
target_include_directories(execution PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(execution PUBLIC core)
