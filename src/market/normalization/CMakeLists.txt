add_library(normalization STATIC normalization.cpp symbol_map.cpp)

target_compile_features(normalization PUBLIC cxx_std_23)
target_include_directories(normalization PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/src)
