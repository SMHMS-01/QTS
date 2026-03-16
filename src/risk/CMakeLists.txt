add_library(risk STATIC risk.cpp limits.cpp)

target_compile_features(risk PUBLIC cxx_std_23)
target_include_directories(risk PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(risk PUBLIC core)
