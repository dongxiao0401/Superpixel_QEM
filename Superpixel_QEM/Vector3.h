
#ifndef __VECTOR3_H
#define __VECTOR3_H

#include <iostream>
#include <cmath>
#include <cassert>

template <class _T> class Vector3;

// another VC 6.0 problem: would not accept this definition 
// after the class

template <class _T>
std::istream& operator >> (std::istream &is, Vector3<_T> &v) 
{
	is >> v.x >> v.y >> v.z;
	return is;
}

template <class _T>
std::ostream& operator << (std::ostream &os, const Vector3<_T> &v) 
{
	return os << v.x << ' ' << v.y << ' ' << v.z;
}

template <class _T>
class Vector3
{
public:
	Vector3(void) { x = y = z = (_T)0;}
	Vector3(const _T &, const _T &, const _T &);
	Vector3(const Vector3 &);
	Vector3(const _T[]);
	// conversion of different vectors 
/*
	template<class G> Vector3& operator= (const Vector3<G>& c) 
	{x = (_T)c.x; y = (_T)c.y; z = (_T)c.z; return *this;}
	template<class G> Vector3(const Vector3<G>& c) 
	{x = (_T)c.x; y = (_T)c.y; z = (_T)c.z; }
*/

	// No error hannling for c-style array acess by subscript
	_T& operator [] (int i) 
	{
		assert(i >= 0 && i < 3);
		if (i == 0) return x;
		else if (i == 1) return y;
		else return z;
	}

	bool operator > (const Vector3 &) const;
	bool operator < (const Vector3 &) const;
	bool operator == (const Vector3 &) const;
	bool operator != (const Vector3 &) const;

	Vector3 operator + (const Vector3 &) const;
	Vector3 operator - (const Vector3 &) const;
	Vector3 operator * (const _T &) const;
	Vector3 operator / (const _T &) const;
	_T operator * (const Vector3 &) const;
	Vector3& operator += (const Vector3 &);
	Vector3& operator -= (const Vector3 &);
	Vector3& operator *= (const _T &);
	Vector3& operator /= (const _T &);
	Vector3 operator ^(const Vector3 &) const;



	void zero(void) {x = y = z = (_T)0;}

	int compare(const Vector3 &);
	static int compare (const Vector3 &, const Vector3 &);

	_T norm2(void) const;
	static _T norm2(const Vector3 &); 

	_T norm(void) const;
	static _T norm(const Vector3 &);

	void normalize(void);
	static Vector3<_T> normalize(const Vector3<_T> &);

	_T dot(const Vector3 &) const;
	static _T dot(const Vector3 &, const Vector3 &);

	Vector3 cross(const Vector3 &) const;
	static Vector3 cross(const Vector3 &, const Vector3 &);

	Vector3 abs(void);
	static Vector3 abs(const Vector3 &);

public:
	_T x, y, z;
};


// implementation

template <class _T>
Vector3<_T>::Vector3(const _T &x, const _T &y, const _T &z)
{
	this->x = x; this->y = y; this->z = z;
}

template <class _T>
Vector3<_T>::Vector3(const Vector3 &v)
{
	x = v.x; y = v.y; z = v.z;
}

template <class _T>
Vector3<_T>::Vector3(const _T v[3])
{
	x = v[0];
	y = v[1];
	z = v[2];
}

template <class _T>
bool Vector3<_T>::operator < (const Vector3& v) const
{
	if (x < v.x) return true;
	else if (x > v.x) return false;

	if (y < v.y) return true;
	else if (y > v.y) return false;

	if (z < v.z) return true;
	else if (z > v.z) return false;

	return false;	// in this case the two are equal
}

template <class _T>
bool Vector3<_T>::operator > (const Vector3& v) const
{
	if (x > v.x) return true;
	else if (x < v.x) return false;

	if (y > v.y) return true;
	else if (y < v.y) return false;

	if (z > v.z) return true;
	else if (z < v.z) return false;

	return false;	// in this case the two are equal
}

template <class _T>
bool Vector3<_T>::operator == (const Vector3& v) const
{
	if (x != v.x) return false;
	if (y != v.y) return false;
	if (z != v.z) return false;

	return true;
}

template <class _T>
bool Vector3<_T>::operator != (const Vector3& v) const
{	
	return !(*this == v);
}

template <class _T>
Vector3<_T> Vector3<_T>::operator + (const Vector3& v) const
{
	Vector3<_T> t(x + v.x, y + v.y, z + v.z);
	return t;
}

template <class _T>
Vector3<_T> Vector3<_T>::operator - (const Vector3& v) const
{
	Vector3<_T> t(x - v.x, y - v.y, z - v.z);
	return t;
}

template <class _T>
Vector3<_T> Vector3<_T>::operator * (const _T &factor) const
{
	Vector3<_T> t(x * factor, y * factor, z * factor);
	return t;
}

template <class _T>
_T Vector3<_T>::operator * (const Vector3<_T> &v) const
{
	_T temp;
	temp = dot(v);
	return temp;
}


template <class _T>
Vector3<_T> Vector3<_T>::operator / (const _T &factor) const
{
	Vector3<_T> t(x / factor, y / factor, z / factor);
	return t;
}

template <class _T>
Vector3<_T>& Vector3<_T>::operator += (const Vector3 &v)
{
	x += v.x; y += v.y; z += v.z;
	return *this;
}

template <class _T>
Vector3<_T>& Vector3<_T>::operator -= (const Vector3 &v)
{
	x -= v.x; y -= v.y; z -= v.z;
	return *this;
}

template <class _T>
Vector3<_T>& Vector3<_T>::operator *= (const _T &factor)
{
	x *= factor; y *= factor; z *= factor;
	return *this;
}

template <class _T>
Vector3<_T>& Vector3<_T>::operator /= (const _T &factor)
{
	x /= factor; y /= factor; z /= factor;
	return *this;
}

template <class _T>
Vector3<_T> Vector3<_T>::operator ^ (const Vector3<_T> &v) const
{
	Vector3<_T> t;
	t = cross(v);
	return t;
}


template <class _T>
int Vector3<_T>::compare(const Vector3& v)
{
	if (*this > v)
		return 1;
	else if (*this < v)
		return -1;
	else return 0;
}


template <class _T>
int Vector3<_T>::compare(const Vector3 &v1, const Vector3 &v2)
{
	if (v1 > v2)
		return 1;
	else if (v1 < v2)
		return -1;
	else return 0;
}

template <class _T>
_T Vector3<_T>::norm2(void) const
{
	return (_T)(x*x + y*y + z*z);
}

template <class _T>
_T Vector3<_T>::norm2(const Vector3 &v)
{
	return (_T)(v.x*v.x + v.y*v.y + v.z*v.z);
}


template <class _T>
_T Vector3<_T>::norm(void) const
{
	_T len;
	len = (_T)sqrt(x*x + y*y + z*z);
	return len;
}


template <class _T>
_T Vector3<_T>::norm(const Vector3 &v)
{
	_T len;
	len = (_T)sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
	return len;
}

template <class _T>
void Vector3<_T>::normalize(void)
{
	_T len;
	len = (_T)sqrt(x*x + y*y + z*z);
	if (len == 0) len = (_T)1.0;
	x /= len; y /= len; z /= len;
}


template <class _T>
Vector3<_T> Vector3<_T>::normalize(const Vector3 &v)
{
	_T len;
	len = (_T)sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
	if (len == 0) len = (_T)1.0;
	return Vector3<_T>(v.x/len, v.y/len, v.z/len);
}


template <class _T>
_T Vector3<_T>::dot(const Vector3 &v) const
{
	_T temp;
	temp = x*v.x + y*v.y + z*v.z;
	return temp;
}


template <class _T>
_T Vector3<_T>::dot(const Vector3 &v1, const Vector3 &v2)
{
	_T temp;
	temp = v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;

	return temp;
}

template <class _T>
Vector3<_T> Vector3<_T>::cross(const Vector3 &v) const
{
	Vector3<_T> t;
	t.x = y * v.z - z * v.y;
	t.y = z * v.x - x * v.z;
	t.z = x * v.y - y * v.x;
	return t;
}

template <class _T>
Vector3<_T> Vector3<_T>::cross(const Vector3 &v1, const Vector3 &v2)
{
	Vector3<_T> t;
	t.x = v1.y * v2.z - v1.z * v2.y;
	t.y = v1.z * v2.x - v1.x * v2.z;
	t.z = v1.x * v2.y - v1.y * v2.x;
	return t;
}


template <class _T>
Vector3<_T> Vector3<_T>::abs(void)
{
	Vector3<_T> t;
	t.x = ::fabs(x);	t.y = ::fabs(y);	t.z = ::fabs(z);
	return t;
}


template <class _T>
Vector3<_T> Vector3<_T>::abs(const Vector3 &v)
{
	Vector3<_T> t;
	t.x = ::fabs(v.x);	t.y = ::fabs(v.y);	t.z = ::fabs(v.z);
	return t;
}

///////////////////////////////////

typedef Vector3<double> vec3;




#endif // __VECTOR3_H