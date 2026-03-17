#pragma once

#include <cctype>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>

#include "market/feed/replay.hpp"
#include "market/normalization/normalization.hpp"
#include "market/normalization/symbol_map.hpp"

namespace market::feed {

// Tardis incremental_book_L2 CSV format:
// exchange,symbol,timestamp,local_timestamp,is_snapshot,side,price,amount
// timestamp/local_timestamp: microseconds since epoch.
// side: bid/ask
// price/amount: decimal values (scaled to integer ticks in this reader).
class TardisIncrementalBookL2CsvReplay final : public ReplaySource {
public:
  static constexpr std::int64_t kPriceScale = 100;   // cents
  static constexpr std::int64_t kAmountScale = 1000; // milli-lots

  TardisIncrementalBookL2CsvReplay(std::string path,
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

  FeedMessageType payload_type() const override { return FeedMessageType::DepthUpdate; }

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
    std::int64_t sign = 1;
    std::size_t i = 0;
    if (text.empty()) {
      return false;
    }
    if (text[0] == '-') {
      sign = -1;
      i = 1;
    }
    std::int64_t value = 0;
    for (; i < text.size(); ++i) {
      if (!std::isdigit(static_cast<unsigned char>(text[i]))) {
        return false;
      }
      value = value * 10 + (text[i] - '0');
    }
    out = value * sign;
    return true;
  }

  static bool parse_decimal_scaled(std::string_view text, std::int64_t scale,
                                   std::int64_t& out) {
    if (text.empty()) {
      return false;
    }
    std::int64_t sign = 1;
    std::size_t i = 0;
    if (text[0] == '-') {
      sign = -1;
      i = 1;
    }
    std::int64_t integer_part = 0;
    std::int64_t frac_part = 0;
    std::int64_t frac_scale = scale;
    bool seen_dot = false;
    for (; i < text.size(); ++i) {
      const char c = text[i];
      if (c == '.') {
        if (seen_dot) {
          return false;
        }
        seen_dot = true;
        continue;
      }
      if (!std::isdigit(static_cast<unsigned char>(c))) {
        return false;
      }
      if (!seen_dot) {
        integer_part = integer_part * 10 + (c - '0');
      } else if (frac_scale > 1) {
        frac_part = frac_part * 10 + (c - '0');
        frac_scale /= 10;
      }
    }
    out = sign * (integer_part * scale + frac_part * frac_scale);
    return true;
  }

  bool parse_line(std::string_view line) {
    const auto fields = split_csv(line);
    if (fields.size() < 8) {
      last_error_ = "invalid field count at line " + std::to_string(line_number_);
      return false;
    }
    if (fields[0] == "exchange") {
      return false;
    }

    const auto symbol = fields[1];
    std::int64_t exchange_us = 0;
    std::int64_t local_us = 0;
    if (!parse_int64(fields[2], exchange_us) || !parse_int64(fields[3], local_us)) {
      last_error_ = "invalid timestamp at line " + std::to_string(line_number_);
      return false;
    }
    const auto is_snapshot_token = fields[4];
    const auto side = fields[5];
    const auto price = fields[6];
    const auto amount = fields[7];

    std::int64_t price_ticks = 0;
    std::int64_t qty_lots = 0;
    if (!parse_decimal_scaled(price, kPriceScale, price_ticks) ||
        !parse_decimal_scaled(amount, kAmountScale, qty_lots)) {
      last_error_ = "invalid price/amount at line " + std::to_string(line_number_);
      return false;
    }

    const bool is_bid = (side == "bid");
    const bool is_ask = (side == "ask");
    if (!is_bid && !is_ask) {
      last_error_ = "invalid side at line " + std::to_string(line_number_);
      return false;
    }

    const bool is_snapshot = (is_snapshot_token == "true");
    const core::types::SymbolId mapped =
        symbol_map_ ? symbol_map_->map(symbol) : core::types::SymbolId{0};

    current_.header.symbol = mapped;
    current_.header.sequence = ++sequence_;
    current_.header.exchange_time = {
        core::types::TimestampNs{exchange_us * 1000}, core::time::ClockType::Exchange};
    current_.header.receive_time = {
        core::types::TimestampNs{local_us * 1000}, core::time::ClockType::Receive};
    if (aligner_) {
      current_.header.receive_time = aligner_->align(current_.header.exchange_time);
    }

    current_.is_snapshot = is_snapshot;
    current_.is_bid = is_bid;
    current_.price = {price_ticks};
    current_.quantity = {qty_lots};
    return true;
  }

  std::string path_;
  std::ifstream file_;
  std::string last_error_;
  std::uint64_t line_number_ = 0;
  std::uint64_t sequence_ = 0;
  DepthUpdate current_{};
  market::normalization::StringSymbolMap* symbol_map_ = nullptr;
  market::normalization::TimeAligner* aligner_ = nullptr;
};

} // namespace market::feed
