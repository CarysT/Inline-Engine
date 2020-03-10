#pragma once

#include "QuaternionImpl.hpp"

namespace mathter {


/// <summary> Check exact equality of coefficients. </summary>
template <class T, bool Packed>
bool operator==(const Quaternion<T, Packed>& lhs, const Quaternion<T, Packed>& rhs) {
	return lhs.vec == rhs.vec;
}
/// <summary> Check exact unequality of coefficients. </summary>
template <class T, bool Packed>
bool operator!=(const Quaternion<T, Packed>& lhs, const Quaternion<T, Packed>& rhs) {
	return !(lhs == rhs);
}

} // namespace mathter