//---------------------------------------------------------------
// Copyright 2022, Ed Keenan, all rights reserved.
//---------------------------------------------------------------

#include "Vect4D.h"


Vect4D::Vect4D()
:	x(0.0f),
	y(0.0f),
	z(0.0f),
	w(1.0f)
{

}


Vect4D& Vect4D::operator = (const Vect4D& tmp)
{
	this->x = tmp.x;
	this->y = tmp.y;
	this->z = tmp.z;
	this->w = tmp.w;

	return *this;
}// = default;

Vect4D::Vect4D(float tx, float ty, float tz, float tw)
	: x(tx),
	  y(ty),
	  z(tz),
	  w(tw)
{

}

Vect4D::~Vect4D()
{
	// nothing to delete
}

Vect4D Vect4D::norm(const Vect4D &v) const
{
	Vect4D temp;
	

	temp._m = _mm_mul_ps(v._m, v._m);
	temp._m = _mm_hadd_ps(temp._m, temp._m);
	temp._m = _mm_hadd_ps(temp._m, temp._m);
	temp._m = _mm_rsqrt_ps(temp._m); 
	temp._m = _mm_mul_ps(v._m, temp._m); 

	return temp;
}

Vect4D Vect4D::operator + (const Vect4D &t) const
{
	Vect4D ans;

	ans.x = this->x + t.x;
	ans.y = this->y + t.y;
	ans.z = this->z + t.z;

	return ans;
}


Vect4D Vect4D::operator* (const Vect4D& v) const
{
	Vect4D temp;
	temp._m = _mm_mul_ps(this->_m, v._m);
	return temp;
}

Vect4D Vect4D::operator *(float scale)
{
	Vect4D tmp;

	tmp.x = this->x * scale;
	tmp.y = this->y * scale;
	tmp.z = this->z * scale;

	return tmp;
}


Vect4D Vect4D::Cross(const Vect4D &R) const
{
	Vect4D vout;

	__m128 _A = _mm_shuffle_ps(this->_m, this->_m, _MM_SHUFFLE(3, 0, 2, 1));
	__m128 _B = _mm_shuffle_ps(R._m, R._m, _MM_SHUFFLE(3, 1, 0, 2));
	__m128 _C = _mm_shuffle_ps(R._m, R._m, _MM_SHUFFLE(3, 0, 2, 1));
	__m128 _D = _mm_shuffle_ps(this->_m, this->_m, _MM_SHUFFLE(3, 1, 0, 2));
	vout._m = _mm_sub_ps(_mm_mul_ps(_A, _B), _mm_mul_ps(_C, _D));

	return vout;
}



// --- End of File ---
