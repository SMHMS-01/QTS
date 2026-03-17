#pragma once

#include <charconv>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>

#include "market/feed/replay.hpp"
#include "market/normalization/normalization.hpp"
#include "market/normalization/symbol_map.hpp"

namespace market::feed {

// CSV format (one update per line):
// sequence,exchange_ts_ns,receive_ts_ns,symbol,side,price,qty
// side: B|Bid|bid|BUY|buy|1 for bid, A|Ask|ask|SELL|sell|0 for ask
class BinanceDepthCsvReplay final : public ReplaySource {
public:
  BinanceDepthCsvReplay(std::string path,
                        market::normalization::StringSymbolMap* symbol_map = nullptr,
                        market::normalization::TimeAligner* aligner = nullptr)
      : path_(std::move(path)), symbol_map_(symbol_map), aligner_(aligner) {
    file_.open(path_);
  }

  const FeedHeader* peek_header() const override { return &current_.header; }

  bool next() override {
    if (!file_.is_open()) {
      return false;
    }
    std::string line;
    while (std::getline(file_, line)) {
      ++line_number_;
      if (line.empty() || line[0] == '#') {
        continue;
      }
      if (parse_line(line)) {
        return true;
      }
    }
    return false;
  }

  const void* payload() const override { return &current_; }

  FeedMessageType payload_type() const override { return FeedMessageType::BookUpdate; }

  const std::string& last_error() const { return last_error_; }

private:
  static std::vector<std::string_view> split_csv(std::string_view line) {
    std::vector<std::string_view> out;
    std::size_t start = 0;
    while (start <= line.size()) {
      const auto comma = line.find(',', start);
      if (comma == std::string_view::npos) {
        out.emplace_back(line.substr(start));
        break;
      }
      out.emplace_back(line.substr(start, comma - start));
      start = comma + 1;
    }
    return out;
  }

  static bool parse_int64(std::string_view text, std::int64_t& out) {
    const auto* begin = text.data();
    const auto* end = text.data() + text.size();
    auto result = std::from_chars(begin, end, out);
    return result.ec == std::errc{} && result.ptr == end;
  }

  static bool parse_uint64(std::string_view text, std::uint64_t& out) {
    const auto* begin = text.data();
    const auto* end = text.data() + text.size();
    auto result = std::from_chars(begin, end, out);
    return result.ec == std::errc{} && result.ptr == end;
  }

  static bool is_bid_token(std::string_view token) {
    return token == "B" || token == "b" || token == "Bid" || token == "bid" ||
           token == "BUY" || token == "buy" || token == "1";
  }

  static bool is_ask_token(std::string_view token) {
    return token == "A" || token == "a" || token == "Ask" || token == "ask" ||
           token == "SELL" || token == "sell" || token == "0";
  }

  bool parse_line(std::string_view line) {
    const auto fields = split_csv(line);
    if (fields.size() < 6) {
      last_error_ = "invalid field count at line " + std::to_string(line_number_);
      return false;
    }

    std::uint64_t sequence = 0;
    std::int64_t exchange_ns = 0;
    std::int64_t receive_ns = 0;
    if (!parse_uint64(fields[0], sequence)) {
      last_error_ = "invalid sequence at line " + std::to_string(line_number_);
      return false;
    }
    if (!parse_int64(fields[1], exchange_ns)) {
      last_error_ = "invalid exchange_ts at line " + std::to_string(line_number_);
      return false;
    }
    std::size_t symbol_index = 2;
    if (fields.size() >= 7) {
      if (!parse_int64(fields[2], receive_ns)) {
        last_error_ = "invalid receive_ts at line " + std::to_string(line_number_);
        return false;
      }
      symbol_index = 3;
    } else {
      receive_ns = exchange_ns;
    }

    const auto symbol = fields[symbol_index];
    const auto side = fields[symbol_index + 1];
    const auto price = fields[symbol_index + 2];
    const auto qty = fields[symbol_index + 3];

    std::int64_t price_ticks = 0;
    std::int64_t qty_lots = 0;
    if (!parse_int64(price, price_ticks) || !parse_int64(qty, qty_lots)) {
      last_error_ = "invalid price/qty at line " + std::to_string(line_number_);
      return false;
    }

    if (!is_bid_token(side) && !is_ask_token(side)) {
      last_error_ = "invalid side at line " + std::to_string(line_number_);
      return false;
    }

    const core::types::SymbolId mapped = symbol_map_ ? symbol_map_->map(symbol)
                                                     : core::types::SymbolId{0};

    current_.header.symbol = mapped;
    current_.header.sequence = sequence;
    current_.header.exchange_time = {
        core::types::TimestampNs{exchange_ns}, core::time::ClockType::Exchange};
    current_.header.receive_time = {
        core::types::TimestampNs{receive_ns}, core::time::ClockType::Receive};
    if (aligner_) {
      current_.header.receive_time = aligner_->align(current_.header.exchange_time);
    }

    current_.is_bid = is_bid_token(side);
    current_.price = {price_ticks};
    current_.quantity = {qty_lots};
    return true;
  }

  std::string path_;
  std::ifstream file_;
  std::string last_error_;
  std::uint64_t line_number_ = 0;
  BookUpdate current_{};
  market::normalization::StringSymbolMap* symbol_map_ = nullptr;
  market::normalization::TimeAligner* aligner_ = nullptr;
};

} // namespace market::feed
