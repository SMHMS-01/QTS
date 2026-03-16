add_subdirectory(order_book)
add_subdirectory(normalization)

add_library(market INTERFACE)

target_compile_features(market INTERFACE cxx_std_23)
target_link_libraries(market INTERFACE order_book normalization)
