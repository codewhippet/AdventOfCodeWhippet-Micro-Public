#pragma once

#include "Vec3.h"

template <typename COORDINATE_TYPE>
struct Mat43
{
	COORDINATE_TYPE m00, m01, m02;
	COORDINATE_TYPE m10, m11, m12;
	COORDINATE_TYPE m20, m21, m22;
	COORDINATE_TYPE m30, m31, m32;
};

template <typename COORDINATE_TYPE>
bool operator==(const Mat43<COORDINATE_TYPE>& a, const Mat43<COORDINATE_TYPE>& b);

template <typename COORDINATE_TYPE>
bool operator!=(const Mat43<COORDINATE_TYPE>& a, const Mat43<COORDINATE_TYPE>& b);

template <typename COORDINATE_TYPE>
bool operator<(const Mat43<COORDINATE_TYPE>& a, const Mat43<COORDINATE_TYPE>& b);

template <typename COORDINATE_TYPE>
Vec3<COORDINATE_TYPE> operator*(const Vec3<COORDINATE_TYPE>& v, const Mat43<COORDINATE_TYPE>& m);

template <typename COORDINATE_TYPE>
Mat43<COORDINATE_TYPE> operator*(const Mat43<COORDINATE_TYPE>& a, const Mat43<COORDINATE_TYPE>& b);

namespace Matrix
{
	template <typename COORDINATE_TYPE>
	Mat43<COORDINATE_TYPE> MakeIdentity();

	template <typename COORDINATE_TYPE>
	Mat43<COORDINATE_TYPE> MakeRotation(const Vec3<COORDINATE_TYPE>& i, const Vec3<COORDINATE_TYPE>& j, const Vec3<COORDINATE_TYPE>& k);

	template <typename COORDINATE_TYPE>
	Mat43<COORDINATE_TYPE> MakeTranslation(const Vec3<COORDINATE_TYPE>& t);

	template <typename COORDINATE_TYPE>
	Mat43<COORDINATE_TYPE> InvertOrthonormal(const Mat43<COORDINATE_TYPE>& m);
}

extern template struct Mat43<int32_t>;

using Mat43Int = Mat43<int32_t>;
