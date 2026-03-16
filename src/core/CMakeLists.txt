add_library(core STATIC bus/event_bus.cpp)

target_compile_features(core PUBLIC cxx_std_23)
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
