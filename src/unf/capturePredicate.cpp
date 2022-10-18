#include "unf/capturePredicate.h"

#include <typeinfo>
#include <functional>
#include <vector>
#include <string>
#include <algorithm>

namespace unf {

CapturePredicate::CapturePredicate(const CapturePredicateFunc& function)
    : _function(function)
{

}

bool CapturePredicate::operator()(
    const UnfNotice::StageNotice& notice) const
{
    if (!_function) return true;
    return _function(notice);
}

CapturePredicate CapturePredicate::Default()
{
    auto function = [](const UnfNotice::StageNotice&) { return true; };
    return CapturePredicate(function);
}

CapturePredicate CapturePredicate::BlockAll()
{
    auto function = [](const UnfNotice::StageNotice&) { return false; };
    return CapturePredicate(function);
}

}  // namespace unf