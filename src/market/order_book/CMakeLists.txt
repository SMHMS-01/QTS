add_library(order_book INTERFACE)

target_compile_features(order_book INTERFACE cxx_std_23)
target_include_directories(order_book INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
