add_library(observability STATIC logging.cpp metrics.cpp tracing.cpp)

target_compile_features(observability PUBLIC cxx_std_23)
target_include_directories(observability PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/src)
