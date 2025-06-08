#include <chronovyan/chronovyan.hpp>

namespace chronovyan {

class Chronovyan::Impl {
public:
    Impl() = default;
    ~Impl() = default;

    bool is_ready_{true};
};

Chronovyan::Chronovyan() : pimpl_(std::make_unique<Impl>()) {}
Chronovyan::~Chronovyan() = default;

std::string Chronovyan::version() noexcept { return "0.1.0"; }

bool Chronovyan::is_ready() const noexcept { return pimpl_ ? pimpl_->is_ready_ : false; }

}  // namespace chronovyan
