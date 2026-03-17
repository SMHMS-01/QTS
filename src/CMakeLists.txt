add_subdirectory(core)
add_subdirectory(market)
add_subdirectory(backtest)
add_subdirectory(execution)
add_subdirectory(risk)
add_subdirectory(observability)
add_subdirectory(storage)

add_executable(system_demo app/system_demo.cpp)
target_link_libraries(system_demo PRIVATE backtest core order_book execution risk)

add_executable(e2e_demo app/e2e_demo.cpp)
target_link_libraries(e2e_demo PRIVATE backtest core order_book observability normalization)

add_executable(binance_replay_demo app/binance_replay_demo.cpp)
target_link_libraries(binance_replay_demo PRIVATE backtest core order_book observability normalization)

add_executable(imbalance_backtest_demo app/imbalance_backtest_demo.cpp)
target_link_libraries(imbalance_backtest_demo PRIVATE backtest core order_book normalization)
