# Binance Delivery incremental_book_L2 (Tardis CSV)

Source
- Downloaded from Tardis datasets sample (binance-delivery).
- Local file: `data/raw/binance_delivery/BTCUSD_200925-incremental_book_L2-2020-07-01.csv.gz`

Columns
- exchange: exchange name
- symbol: instrument symbol
- timestamp: exchange timestamp in microseconds
- local_timestamp: local timestamp in microseconds
- is_snapshot: `true` for snapshot rows, `false` for diffs
- side: `bid` or `ask`
- price: decimal price
- amount: decimal quantity

Reader Mapping
- `timestamp`/`local_timestamp` are converted from microseconds to nanoseconds.
- `price` is scaled by 100 into `PriceTicks`.
- `amount` is scaled by 1000 into `QuantityLots`.
- Snapshot rows trigger a one-time `order_book.clear()` per snapshot timestamp.
