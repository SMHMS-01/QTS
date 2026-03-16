#include "market/normalization/normalization.hpp"
#include "market/normalization/sequence_guard.hpp"

namespace market::normalization {

class SimpleNormalizer final : public Normalizer {
public:
  explicit SimpleNormalizer(SequenceGuard* guard) : guard_(guard) {}

  NormalizedBookUpdate normalize_book_update(const market::feed::BookUpdate& msg) override {
    if (guard_) {
      guard_->accept(msg.header.sequence);
    }
    return {msg.header, msg.is_bid, msg.price, msg.quantity};
  }

  NormalizedTrade normalize_trade(const market::feed::Trade& msg) override {
    if (guard_) {
      guard_->accept(msg.header.sequence);
    }
    return {msg.header, msg.price, msg.quantity, msg.is_buyer_maker};
  }

private:
  SequenceGuard* guard_ = nullptr;
};

} // namespace market::normalization
