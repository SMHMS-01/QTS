add_library(backtest STATIC replay_engine.cpp replay_controller.cpp)

target_compile_features(backtest PUBLIC cxx_std_23)
target_include_directories(backtest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/src)

target_link_libraries(backtest PUBLIC core)

add_executable(backtest_demo demo_main.cpp)
target_link_libraries(backtest_demo PRIVATE backtest core)
