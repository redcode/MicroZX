/* ZMathematics Kit C++ API - Matrix.hpp
	      __	   __
  _______ ___/ /______ ___/ /__
 / __/ -_) _  / __/ _ \ _  / -_)
/_/  \__/\_,_/\__/\___/_,_/\__/
Copyright © 2006-2016 Manuel Sainz de Baranda y Goñi.
Released under the terms of the GNU Lesser General Public License v3. */

#ifndef __ZMathematics_Matrix_HPP__
#define __ZMathematics_Matrix_HPP__

#include <Z/classes/base/Value2D.hpp>
#include <Z/classes/base/Value3D.hpp>

namespace Zeta {
	template <typename T> struct Matrix2x2;
	template <typename T> struct Matrix3x3;
	template <typename T> struct Matrix4x4;
}

template <typename T> struct Zeta::Matrix4x4  {
	T m[4 * 4];

	// MARK: - Constructors

	static const Matrix4x4<T> identity;

	inline Matrix4x4<T>() {};

	// MARK: - Operators

	Matrix4x4<T> operator +(const Matrix4x4<T> &matrix) const;
	Matrix4x4<T> operator -(const Matrix4x4<T> &matrix) const;
	Matrix4x4<T> operator *(const Matrix4x4<T> &matrix) const;
	Matrix4x4<T> operator /(const Matrix4x4<T> &matrix) const;

	Matrix4x4<T> &operator +=(const Matrix4x4<T> &matrix);
	Matrix4x4<T> &operator -=(const Matrix4x4<T> &matrix);
	Matrix4x4<T> &operator *=(const Matrix4x4<T> &matrix);
	Matrix4x4<T> &operator /=(const Matrix4x4<T> &matrix);

	Matrix4x4<T> operator +(T scalar) const;
	Matrix4x4<T> operator -(T scalar) const;
	Matrix4x4<T> operator *(T scalar) const;
	Matrix4x4<T> operator /(T scalar) const;

	Matrix4x4<T> &operator +=(T scalar);
	Matrix4x4<T> &operator -=(T scalar);
	Matrix4x4<T> &operator *=(T scalar);
	Matrix4x4<T> &operator /=(T scalar);

	Matrix4x4<T> &set_identity();
	Matrix4x4<T> &transpose();
	T determinant() const;
	Matrix4x4<T> inverse() const;

	// MARK: - Operations for affine transformations

	static Matrix4x4<T> translation(T x, T y);
	static Matrix4x4<T> translation(T x, T y, T z);
	static Matrix4x4<T> scaling    (T x, T y);
	static Matrix4x4<T> scaling    (T x, T y, T z);
	static Matrix4x4<T> rotation   (T z);
	static Matrix4x4<T> rotation   (T x, T y, T z);


	//static inline Matrix4x4<T> translation(const Value2D<T> &translation)
	//	{return Matrix4x4<T>::translation(translation.x, translation.y);}


	//static inline Matrix4x4<T> translation(const Value3D<T> &translation)
	//	{return Matrix4x4<T>::translation(translation.x, translation.y, translation.z);}


	//static inline Matrix4x4<T> scaling(const Value2D<T> &scale)
	//	{return scaling(scale.x, scale.y);}


	//static inline Matrix<T, X, Y> scaling(const Value3D<T> &scale)
	//	{return scaling(scale.x, scale.y, scale.z);}


	//static inline Matrix4x4<T> rotation(const Value3D<T> &rotation)
	//	{return Matrix4x4<T>::rotation(rotation.x, rotation.y, rotation.z);}


	Matrix4x4<T> translated(T x, T y)      const;
	Matrix4x4<T> translated(T x, T y, T z) const;
	Matrix4x4<T> scaled    (T x, T y)      const;
	Matrix4x4<T> scaled    (T x, T y, T z) const;
	Matrix4x4<T> rotated   (T z)	       const;
	Matrix4x4<T> rotated   (T x, T y, T z) const;


	inline Matrix4x4<T> translated(const Value2D<T> &translation) const
		{return translated(translation.x, translation.y);}


	//inline Matrix4x4<T> translated(const Value3D<T> &distance) const
	//	{return translated(translation.x, translation.y, translation.z);}


	inline Matrix4x4<T> scaled(const Value2D<T> &scale) const
		{return scaled(scale.x, scale.y);}


	//inline Matrix4x4<T> scaled(const Value3D<T> &scale) const
	//	{return scaled(scale.x, scale.y, scale.z);}


	//inline Matrix4x4<T> rotated(const Value3D<T> &rotation) const
	//	{return scaled(scale.x, scale.y, scale.z);}


	Matrix4x4<T> &translate(T x, T y);
	Matrix4x4<T> &translate(T x, T y, T z);
	Matrix4x4<T> &scale    (T x, T y);
	Matrix4x4<T> &scale    (T x, T y, T z);
	Matrix4x4<T> &rotate   (T z);
	Matrix4x4<T> &rotate   (T x, T y, T z);
};


#endif // __ZMathematics_Matrix_HPP__
