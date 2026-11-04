#pragma once

#include "Mat43.h"

template <typename COORDINATE_TYPE>
bool operator==(const Mat43<COORDINATE_TYPE>& a, const Mat43<COORDINATE_TYPE>& b)
{
	return
		(a.m00 == b.m00) && (a.m01 == b.m01) && (a.m02 == b.m02) &&
		(a.m10 == b.m10) && (a.m11 == b.m11) && (a.m12 == b.m12) &&
		(a.m20 == b.m20) && (a.m21 == b.m21) && (a.m22 == b.m22) &&
		(a.m30 == b.m30) && (a.m31 == b.m31) && (a.m32 == b.m32);
}

template <typename COORDINATE_TYPE>
bool operator!=(const Mat43<COORDINATE_TYPE>& a, const Mat43<COORDINATE_TYPE>& b)
{
	return !(a == b);
}

template <typename COORDINATE_TYPE>
bool operator<(const Mat43<COORDINATE_TYPE>& a, const Mat43<COORDINATE_TYPE>& b)
{
	if (a.m00 != b.m00) return a.m00 < b.m00;
	if (a.m01 != b.m01) return a.m01 < b.m01;
	if (a.m02 != b.m02) return a.m02 < b.m02;

	if (a.m10 != b.m10) return a.m10 < b.m10;
	if (a.m11 != b.m11) return a.m11 < b.m11;
	if (a.m12 != b.m12) return a.m12 < b.m12;

	if (a.m20 != b.m20) return a.m20 < b.m20;
	if (a.m21 != b.m21) return a.m21 < b.m21;
	if (a.m22 != b.m22) return a.m22 < b.m22;

	if (a.m30 != b.m30) return a.m30 < b.m30;
	if (a.m31 != b.m31) return a.m31 < b.m31;
	return a.m32 < b.m32;
}

template <typename COORDINATE_TYPE>
Vec3<COORDINATE_TYPE> operator*(const Vec3<COORDINATE_TYPE>& v, const Mat43<COORDINATE_TYPE>& m)
{
	return Vec3<COORDINATE_TYPE>{
		Dot(v, Vec3<COORDINATE_TYPE>{ m.m00, m.m10, m.m20 }) + m.m30,
		Dot(v, Vec3<COORDINATE_TYPE>{ m.m01, m.m11, m.m21 }) + m.m31,
		Dot(v, Vec3<COORDINATE_TYPE>{ m.m02, m.m12, m.m22 }) + m.m32,
		};
}

template <typename COORDINATE_TYPE>
Mat43<COORDINATE_TYPE> operator*(const Mat43<COORDINATE_TYPE>& a, const Mat43<COORDINATE_TYPE>& b)
{
	Mat43<COORDINATE_TYPE> m;

	m.m00 = (a.m00 * b.m00) + (a.m01 * b.m10) + (a.m02 * b.m20);
	m.m01 = (a.m00 * b.m01) + (a.m01 * b.m11) + (a.m02 * b.m21);
	m.m02 = (a.m00 * b.m02) + (a.m01 * b.m12) + (a.m02 * b.m22);

	m.m10 = (a.m10 * b.m00) + (a.m11 * b.m10) + (a.m12 * b.m20);
	m.m11 = (a.m10 * b.m01) + (a.m11 * b.m11) + (a.m12 * b.m21);
	m.m12 = (a.m10 * b.m02) + (a.m11 * b.m12) + (a.m12 * b.m22);

	m.m20 = (a.m20 * b.m00) + (a.m21 * b.m10) + (a.m22 * b.m20);
	m.m21 = (a.m20 * b.m01) + (a.m21 * b.m11) + (a.m22 * b.m21);
	m.m22 = (a.m20 * b.m02) + (a.m21 * b.m12) + (a.m22 * b.m22);

	m.m30 = (a.m30 * b.m00) + (a.m31 * b.m10) + (a.m32 * b.m20) + b.m30;
	m.m31 = (a.m30 * b.m01) + (a.m31 * b.m11) + (a.m32 * b.m21) + b.m31;
	m.m32 = (a.m30 * b.m02) + (a.m31 * b.m12) + (a.m32 * b.m22) + b.m32;

	return m;
}

namespace Matrix
{
	template <typename COORDINATE_TYPE>
	Mat43<COORDINATE_TYPE> MakeIdentity()
	{
		return Mat43{
			1, 0, 0,
			0, 1, 0,
			0, 0, 1,
			0, 0, 0
		};
	}

	template <typename COORDINATE_TYPE>
	Mat43<COORDINATE_TYPE> MakeRotation(const Vec3<COORDINATE_TYPE>& i, const Vec3<COORDINATE_TYPE>& j, const Vec3<COORDINATE_TYPE>& k)
	{
		return Mat43{
		i.X, i.Y, i.Z,
		j.X, j.Y, j.Z,
		k.X, k.Y, k.Z,
		0, 0, 0
		};
	}

	template <typename COORDINATE_TYPE>
	Mat43<COORDINATE_TYPE> MakeTranslation(const Vec3<COORDINATE_TYPE>& t)
	{
		return Mat43{
			1, 0, 0,
			0, 1, 0,
			0, 0, 1,
			t.X, t.Y, t.Z,
		};
	}

	template <typename COORDINATE_TYPE>
	Mat43<COORDINATE_TYPE> InvertOrthonormal(const Mat43<COORDINATE_TYPE>& m)
	{
		Mat43<COORDINATE_TYPE> inverse;

		inverse.m00 = m.m00;
		inverse.m01 = m.m10;
		inverse.m02 = m.m20;

		inverse.m10 = m.m01;
		inverse.m11 = m.m11;
		inverse.m12 = m.m21;

		inverse.m20 = m.m02;
		inverse.m21 = m.m12;
		inverse.m22 = m.m22;

		inverse.m30 = 0;
		inverse.m31 = 0;
		inverse.m32 = 0;

		Vec3<COORDINATE_TYPE> v = Vec3<COORDINATE_TYPE>{ m.m30, m.m31, m.m32 } * inverse;

		inverse.m30 = -v.X;
		inverse.m31 = -v.Y;
		inverse.m32 = -v.Z;

		return inverse;
	}
}

template struct Mat43<int32_t>;

template Vec3<int32_t> operator*(const Vec3<int32_t>& v, const Mat43<int32_t>& m);

template Mat43<int32_t> operator*(const Mat43<int32_t>& a, const Mat43<int32_t>& b);

template Mat43<int32_t> Matrix::MakeRotation(const Vec3<int32_t>& i, const Vec3<int32_t>& j, const Vec3<int32_t>& k);