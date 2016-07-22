/* ZMathematics Kit - Matrix.cpp
	      __	   __
  _______ ___/ /______ ___/ /__
 / __/ -_) _  / __/ _ \ _  / -_)
/_/  \__/\_,_/\__/\___/_,_/\__/
Copyright © 2006-2016 Manuel Sainz de Baranda y Goñi.
Released under the terms of the GNU Lesser General Public License v3. */

#include "Matrix.hpp"
#include <Z/functions/base/value.hpp>
#include <stdlib.h>

using namespace Zeta;


#define SET_4X4(r, a, b, f,					\
	a_1_1, b_1_1, a_1_2, b_1_2, a_1_3, b_1_3, a_1_4, b_1_4,	\
	a_2_1, b_2_1, a_2_2, b_2_2, a_2_3, b_2_3, a_2_4, b_2_4,	\
	a_3_1, b_3_1, a_3_2, b_3_2, a_3_3, b_3_3, a_3_4, b_3_4,	\
	a_4_1, b_4_1, a_4_2, b_4_2, a_4_3, b_4_3, a_4_4, b_4_4	\
)								\
	r[ 0] a a_1_1 b b_1_1 f; r[ 1] a a_1_2 b b_1_2 f; r[ 2] a a_1_3 b b_1_3 f; r[ 3] a a_1_4 b b_1_4 f;	\
	r[ 4] a a_2_1 b b_2_1 f; r[ 5] a a_2_2 b b_2_2 f; r[ 6] a a_2_3 b b_2_3 f; r[ 7] a a_2_4 b b_2_4 f;	\
	r[ 8] a a_3_1 b b_3_1 f; r[ 9] a a_3_2 b b_3_2 f; r[10] a a_3_3 b b_3_3 f; r[11] a a_3_4 b b_3_4 f;	\
	r[12] a a_4_1 b b_4_1 f; r[13] a a_4_2 b b_4_2 f; r[14] a a_4_3 b b_4_3 f; r[15] a a_4_4 b b_4_4 f;


#define A m
#define B matrix.m


template <typename T>
Matrix4x4<T> Matrix4x4<T>::operator +(const Matrix4x4<T> &matrix) const
	{
	Matrix4x4<T> result;

	SET_4X4(result.m, = m, + matrix.m,,
		[ 0], [ 0], [ 1], [ 1], [ 2], [ 2], [ 3], [ 3],
		[ 4], [ 4], [ 5], [ 5], [ 6], [ 6], [ 7], [ 7],
		[ 8], [ 8], [ 9], [ 9], [10], [10], [11], [11],
		[12], [12], [13], [13], [14], [14], [15], [15])

	return matrix;
	}


template <typename T>
Matrix4x4<T> Matrix4x4<T>::operator -(const Matrix4x4<T> &matrix) const
	{
	Matrix4x4<T> result;

	SET_4X4(result.m, = m, - matrix.m,,
		[ 0], [ 0], [ 1], [ 1], [ 2], [ 2], [ 3], [ 3],
		[ 4], [ 4], [ 5], [ 5], [ 6], [ 6], [ 7], [ 7],
		[ 8], [ 8], [ 9], [ 9], [10], [10], [11], [11],
		[12], [12], [13], [13], [14], [14], [15], [15])

	return matrix;
	}


template <typename T>
Matrix4x4<T> Matrix4x4<T>::operator *(const Matrix4x4<T> &matrix) const
	{
	Matrix4x4<T> result;

	SET_4X4(result.m, =,,,
		A[ 0] * B[0] + A[ 1] * B[4] + A[ 2] * B[ 8] + A[ 3] * B[12],,
		A[ 0] * B[1] + A[ 1] * B[5] + A[ 2] * B[ 9] + A[ 3] * B[13],,
		A[ 0] * B[2] + A[ 1] * B[6] + A[ 2] * B[10] + A[ 3] * B[14],,
		A[ 0] * B[3] + A[ 1] * B[7] + A[ 2] * B[11] + A[ 3] * B[15],,

		A[ 4] * B[0] + A[ 5] * B[4] + A[ 6] * B[ 8] + A[ 7] * B[12],,
		A[ 4] * B[1] + A[ 5] * B[5] + A[ 6] * B[ 9] + A[ 7] * B[13],,
		A[ 4] * B[2] + A[ 5] * B[6] + A[ 6] * B[10] + A[ 7] * B[14],,
		A[ 4] * B[3] + A[ 5] * B[7] + A[ 6] * B[11] + A[ 7] * B[15],,

		A[ 8] * B[0] + A[ 9] * B[4] + A[10] * B[ 8] + A[11] * B[12],,
		A[ 8] * B[1] + A[ 9] * B[5] + A[10] * B[ 9] + A[11] * B[13],,
		A[ 8] * B[2] + A[ 9] * B[6] + A[10] * B[10] + A[11] * B[14],,
		A[ 8] * B[3] + A[ 9] * B[7] + A[10] * B[11] + A[11] * B[15],,

		A[12] * B[0] + A[13] * B[4] + A[14] * B[ 8] + A[15] * B[12],,
		A[12] * B[1] + A[13] * B[5] + A[14] * B[ 9] + A[15] * B[13],,
		A[12] * B[2] + A[13] * B[6] + A[14] * B[10] + A[15] * B[14],,
		A[12] * B[3] + A[13] * B[7] + A[14] * B[11] + A[15] * B[15],)

	return result;
	}


template <typename T>
Matrix4x4<T> Matrix4x4<T>::operator /(const Matrix4x4<T> &matrix) const
	{
	Matrix4x4<T> result;
	return result;
	}


template <typename T>
Matrix4x4<T> &Matrix4x4<T>::operator +=(const Matrix4x4<T> &matrix)
	{
	SET_4X4(m, += matrix.m,,,
		[ 0],, [ 1],, [ 2],, [ 3],,
		[ 4],, [ 5],, [ 6],, [ 7],,
		[ 8],, [ 9],, [10],, [11],,
		[12],, [13],, [14],, [15],)

	return *this;
	}


template <typename T>
Matrix4x4<T> &Matrix4x4<T>::operator -=(const Matrix4x4<T> &matrix)
	{
	SET_4X4(m, -= matrix.m,,,
		[ 0],, [ 1],, [ 2],, [ 3],,
		[ 4],, [ 5],, [ 6],, [ 7],,
		[ 8],, [ 9],, [10],, [11],,
		[12],, [13],, [14],, [15],)

	return *this;
	}


template <typename T>
Matrix4x4<T> &Matrix4x4<T>::operator *=(const Matrix4x4<T> &matrix)
	{return *this = *this * matrix;}


template <typename T>
Matrix4x4<T> &Matrix4x4<T>::operator /=(const Matrix4x4<T> &matrix)
	{return *this = *this / matrix;}


template <typename T>
Matrix4x4<T> Matrix4x4<T>::operator +(T scalar) const
	{
	Matrix4x4<T> result;

	return result;
	}


template <typename T>
Matrix4x4<T> Matrix4x4<T>::operator -(T scalar) const
	{
	Matrix4x4<T> result;

	return result;
	}


template <typename T>
Matrix4x4<T> Matrix4x4<T>::operator *(T scalar) const
	{
	Matrix4x4<T> result;

	SET_4X4(result.m, = m,, * scalar,
		[ 0],, [ 1],, [ 2],, [ 3],,
		[ 4],, [ 5],, [ 6],, [ 7],,
		[ 8],, [ 9],, [10],, [11],,
		[12],, [13],, [14],, [15],)

	return result;
	}


template <typename T>
Matrix4x4<T> Matrix4x4<T>::operator /(T scalar) const
	{
	Matrix4x4<T> result;

	SET_4X4(result.m, = m,, / scalar,
		[ 0],, [ 1],, [ 2],, [ 3],,
		[ 4],, [ 5],, [ 6],, [ 7],,
		[ 8],, [ 9],, [10],, [11],,
		[12],, [13],, [14],, [15],)

	return result;
	}


template <typename T>
Matrix4x4<T> &Matrix4x4<T>::operator +=(T scalar)
	{
	return *this;
	}


template <typename T>
Matrix4x4<T> &Matrix4x4<T>::operator -=(T scalar)
	{
	return *this;
	}


template <typename T>
Matrix4x4<T> &Matrix4x4<T>::operator *=(T scalar)
	{
	SET_4X4(m, *=, ,scalar,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,)
	return *this;
	}


template <typename T>
Matrix4x4<T> &Matrix4x4<T>::operator /=(T scalar)
	{
	SET_4X4(m, /=, ,scalar,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,)
	return *this;
	}


template <typename T>
Matrix4x4<T> &Matrix4x4<T>::set_identity()
	{
	SET_4X4(m, =,,,
		1,, 0,, 0,, 0,,
		0,, 1,, 0,, 0,,
		0,, 0,, 1,, 0,,
		0,, 0,, 0,, 1,)

	return *this;
	}


template <typename T>
Matrix4x4<T> &Matrix4x4<T>::transpose()
	{
	swap<T>(m +  1, m +  4);
	swap<T>(m +  2, m +  8);
	swap<T>(m +  3, m + 12);
	swap<T>(m +  4, m +  1);
	swap<T>(m +  6, m +  9);
	swap<T>(m +  7, m + 13);
	swap<T>(m +  8, m +  2);
	swap<T>(m +  9, m +  6);
	swap<T>(m + 11, m + 14);
	swap<T>(m + 12, m +  3);
	swap<T>(m + 13, m +  7);
	swap<T>(m + 14, m + 11);

	return *this;
	}


template <typename T>
T Matrix4x4<T>::determinant() const
	{
	return	m[12] * (m[ 9] * (m[3] * m[6] - m[2] * m[7])  +
			 m[10] * (m[1] * m[7] - m[3] * m[5])  +
			 m[11] * (m[2] * m[5] - m[1] * m[6])) +
		m[13] * (m[ 8] * (m[2] * m[7] - m[3] * m[6])  +
			 m[10] * (m[3] * m[4] - m[0] * m[7])  +
			 m[11] * (m[0] * m[6] - m[2] * m[4])) +
		m[14] * (m[ 8] * (m[3] * m[5] - m[1] * m[7])  +
			 m[ 9] * (m[0] * m[7] - m[3] * m[4])  +
			 m[11] * (m[1] * m[4] - m[0] * m[5])) +
		m[15] * (m[ 8] * (m[1] * m[6] - m[2] * m[5])  +
			 m[ 9] * (m[2] * m[4] - m[0] * m[6])  +
			 m[10] * (m[0] * m[5] - m[1] * m[4]));
	}


template <typename T>
Matrix4x4<T> Matrix4x4<T>::inverse() const
	{
	Matrix4x4<T> result;

	T fraction = T(1.0) / determinant();

	SET_4X4(result.m, = fraction *,,,
		(m[13] * (m[6] * m[11] - m[7] * m[10]) + m[14] * (m[7] * m[ 9] - m[5] * m[11]) + m[15] * (m[5] * m[10] - m[6] * m[ 9])),,
		(m[13] * (m[3] * m[10] - m[2] * m[11]) + m[14] * (m[1] * m[11] - m[3] * m[ 9]) + m[15] * (m[2] * m[ 9] - m[1] * m[10])),,
		(m[13] * (m[2] * m[ 7] - m[3] * m[ 6]) + m[14] * (m[3] * m[ 5] - m[1] * m[ 7]) + m[15] * (m[1] * m[ 6] - m[2] * m[ 5])),,
		(m[ 9] * (m[3] * m[ 6] - m[2] * m[ 7]) + m[10] * (m[1] * m[ 7] - m[3] * m[ 5]) + m[11] * (m[2] * m[ 5] - m[1] * m[ 6])),,

		(m[12] * (m[7] * m[10] - m[6] * m[11]) + m[14] * (m[4] * m[11] - m[7] * m[ 8]) + m[15] * (m[6] * m[ 8] - m[4] * m[10])),,
		(m[12] * (m[2] * m[11] - m[3] * m[10]) + m[14] * (m[3] * m[ 8] - m[0] * m[11]) + m[15] * (m[0] * m[10] - m[2] * m[ 8])),,
		(m[12] * (m[3] * m[ 6] - m[2] * m[ 7]) + m[14] * (m[0] * m[ 7] - m[3] * m[ 4]) + m[15] * (m[2] * m[ 4] - m[0] * m[ 6])),,
		(m[ 8] * (m[2] * m[ 7] - m[3] * m[ 6]) + m[10] * (m[3] * m[ 4] - m[0] * m[ 7]) + m[11] * (m[0] * m[ 6] - m[2] * m[ 4])),,

		(m[12] * (m[5] * m[11] - m[7] * m[ 9]) + m[13] * (m[7] * m[ 8] - m[4] * m[11]) + m[15] * (m[4] * m[ 9] - m[5] * m[ 8])),,
		(m[12] * (m[3] * m[ 9] - m[1] * m[11]) + m[13] * (m[0] * m[11] - m[3] * m[ 8]) + m[15] * (m[1] * m[ 8] - m[0] * m[ 9])),,
		(m[12] * (m[1] * m[ 7] - m[3] * m[ 5]) + m[13] * (m[3] * m[ 4] - m[0] * m[ 7]) + m[15] * (m[0] * m[ 5] - m[1] * m[ 4])),,
		(m[ 8] * (m[3] * m[ 5] - m[1] * m[ 7]) + m[ 9] * (m[0] * m[ 7] - m[3] * m[ 4]) + m[11] * (m[1] * m[ 4] - m[0] * m[ 5])),,

		(m[12] * (m[6] * m[ 9] - m[5] * m[10]) + m[13] * (m[4] * m[10] - m[6] * m[ 8]) + m[14] * (m[5] * m[ 8] - m[4] * m[ 9])),,
		(m[12] * (m[1] * m[10] - m[2] * m[ 9]) + m[13] * (m[2] * m[ 8] - m[0] * m[10]) + m[14] * (m[0] * m[ 9] - m[1] * m[ 8])),,
		(m[12] * (m[2] * m[ 5] - m[1] * m[ 6]) + m[13] * (m[0] * m[ 6] - m[2] * m[ 4]) + m[14] * (m[1] * m[ 4] - m[0] * m[ 5])),,
		(m[ 8] * (m[1] * m[ 6] - m[2] * m[ 5]) + m[ 9] * (m[2] * m[ 4] - m[0] * m[ 6]) + m[10] * (m[0] * m[ 5] - m[1] * m[ 4])),)

	return result;
	}


template <typename T>
Matrix4x4<T> Matrix4x4<T>::translation(T x, T y)
	{
	Matrix4x4<T> result;

	SET_4X4(result.m, =,,,
		1,, 0,, 0,, x,,
		0,, 1,, 0,, y,,
		0,, 0,, 1,, 0,,
		0,, 0,, 0,, 1,)

	return result;
	}


template <typename T>
Matrix4x4<T> Matrix4x4<T>::translation(T x, T y, T z)
	{
	Matrix4x4<T> result;

	SET_4X4(result.m, =,,,
		1,, 0,, 0,, x,,
		0,, 1,, 0,, y,,
		0,, 0,, 1,, z,,
		0,, 0,, 0,, 1,)

	return result;
	}


template <typename T>
Matrix4x4<T> Matrix4x4<T>::scaling(T x, T y)
	{
	Matrix4x4<T> result;

	SET_4X4(result.m, =,,,
		x,, 0,, 0,, 0,,
		0,, y,, 0,, 0,,
		0,, 0,, 1,, 0,,
		0,, 0,, 0,, 1,)

	return result;
	}


template <typename T>
Matrix4x4<T> Matrix4x4<T>::scaling(T x, T y, T z)
	{
	Matrix4x4<T> result;

	SET_4X4(result.m, =,,,
		x,, 0,, 0,, 0,,
		0,, y,, 0,, 0,,
		0,, 0,, z,, 0,,
		0,, 0,, 0,, 1,)

	return result;
	}


template <typename T>
Matrix4x4<T> Matrix4x4<T>::translated(T x, T y) const
	{
	Matrix4x4<T> result;

	SET_4X4(result.m, =,,,
		m[ 0],, m[ 1],, m[ 2],, m[ 0] * x + m[ 1] * y + m[ 3],,
		m[ 4],, m[ 5],, m[ 6],, m[ 4] * x + m[ 5] * y + m[ 7],,
		m[ 8],, m[ 9],, m[10],, m[ 8] * x + m[ 9] * y + m[11],,
		m[12],, m[13],, m[14],, m[12] * x + m[13] * y + m[15],)

	return result;
	}


template <typename T>
Matrix4x4<T> Matrix4x4<T>::translated(T x, T y, T z) const
	{
	Matrix4x4<T> result;

	SET_4X4(result.m, =,,,
		m[ 0],, m[ 1],, m[ 2],, m[ 0] * x + m[ 1] * y + m[ 2] * z + m[ 3],,
		m[ 4],, m[ 5],, m[ 6],, m[ 4] * x + m[ 5] * y + m[ 6] * z + m[ 7],,
		m[ 8],, m[ 9],, m[10],, m[ 8] * x + m[ 9] * y + m[10] * z + m[11],,
		m[12],, m[13],, m[14],, m[12] * x + m[13] * y + m[14] * z + m[15],)

	return result * *this;
	}


template <typename T>
Matrix4x4<T> Matrix4x4<T>::scaled(T x, T y) const
	{
	Matrix4x4<T> result;

	SET_4X4(result.m, =,,,
		m[ 0] * x,, m[ 1] * y,, m[ 2],, m[ 3],,
		m[ 4] * x,, m[ 5] * y,,	m[ 6],,	m[ 7],,
		m[ 8] * x,, m[ 9] * y,, m[10],, m[11],,
		m[12] * x,, m[13] * y,, m[14],, m[15],)

	return result;
	}


template <typename T>
Matrix4x4<T> Matrix4x4<T>::scaled(T x, T y, T z) const
	{
	Matrix4x4<T> result;

	SET_4X4(result.m, =,,,
		m[ 0] * x,, m[ 1] * y,, m[ 2] * z,, m[ 3],,
		m[ 4] * x,, m[ 5] * y,,	m[ 6] * z,, m[ 7],,
		m[ 8] * x,, m[ 9] * y,, m[10] * z,, m[11],,
		m[12] * x,, m[13] * y,, m[14] * z,, m[15],)

	return result;
	}


template struct Zeta::Matrix4x4<float>;
template struct Zeta::Matrix4x4<double>;

#if Z_LANGUAGE_HAS_TYPE(C, LDOUBLE)
	template struct Zeta::Matrix4x4<long double>;
#endif


// Matrix.cpp EOF
