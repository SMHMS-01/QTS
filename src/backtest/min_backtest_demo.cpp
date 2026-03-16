#include <algorithm>
#include <iostream>
#include <vector>

namespace {

struct Bar {
  double price = 0.0;
};

struct Trade {
  int qty = 0; // +1 buy, -1 sell
  double price = 0.0;
};

class SimpleStrategy {
public:
  explicit SimpleStrategy(double threshold) : threshold_(threshold) {}

  int signal(double price) {
    if (!has_prev_) {
      prev_ = price;
      has_prev_ = true;
      return 0;
    }
    if (price > prev_ + threshold_) {
      prev_ = price;
      return 1; // buy
    }
    if (price < prev_ - threshold_) {
      prev_ = price;
      return -1; // sell
    }
    prev_ = price;
    return 0;
  }

private:
  double threshold_ = 0.0;
  double prev_ = 0.0;
  bool has_prev_ = false;
};

class Portfolio {
public:
  void on_trade(const Trade& t) {
    position_ += t.qty;
    cash_ -= static_cast<double>(t.qty) * t.price;
    trades_ += 1;
  }

  double equity(double mark) const { return cash_ + position_ * mark; }
  int trades() const { return trades_; }

private:
  int position_ = 0;
  double cash_ = 0.0;
  int trades_ = 0;
};

} // namespace

int main() {
  // Minimal in-memory data source
  const std::vector<Bar> data = {
      {100.0}, {101.0}, {102.0}, {101.5}, {103.0}, {104.0}, {103.0}, {102.0}};

  SimpleStrategy strat(0.8);
  Portfolio portfolio;

  for (const auto& bar : data) {
    const int s = strat.signal(bar.price);
    if (s == 0) {
      continue;
    }
    Trade t{.qty = s, .price = bar.price};
    portfolio.on_trade(t);
  }

  const double final_equity = portfolio.equity(data.back().price);
  std::cout << "Min backtest demo\n";
  std::cout << "Trades: " << portfolio.trades() << "\n";
  std::cout << "Final equity: " << final_equity << "\n";

  return 0;
}
