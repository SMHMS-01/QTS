add_library(order_book STATIC order_book.cpp)

target_compile_features(order_book PUBLIC cxx_std_23)
target_include_directories(order_book PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/src)
