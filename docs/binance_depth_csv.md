# Binance Depth CSV Format

This project expects one update per line in a simple CSV format.

Format
```
sequence,exchange_ts_ns,receive_ts_ns,symbol,side,price,qty
```

Notes
- `sequence` is monotonically increasing.
- `exchange_ts_ns` and `receive_ts_ns` are int64 nanoseconds.
- `side` accepts: `B|Bid|bid|BUY|buy|1` for bid and `A|Ask|ask|SELL|sell|0` for ask.
- `price` and `qty` are integer ticks/lots.
- If `receive_ts_ns` is omitted, it will default to `exchange_ts_ns`.

Example
```
1,1700000000000000000,1700000000000000100,BTCUSDT,B,65000,3
2,1700000000000000200,1700000000000000300,BTCUSDT,A,65010,2
```
