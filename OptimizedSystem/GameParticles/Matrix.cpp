//---------------------------------------------------------------
// Copyright 2022, Ed Keenan, all rights reserved.
//---------------------------------------------------------------

#include <Math.h>
#include "Vect4d.h"
#include "Matrix.h"

Matrix::Matrix()
	:m0 (1.0f),
	m1 (0.0f),
	m2 (0.0f),
	m3 (0.0f),
	  		 
	m4 (0.0f),
	m5 (1.0f),
	m6 (0.0f),
	m7 (0.0f),
	   		 
	m8 (0.0f),
	m9 (0.0f),
	m10(1.0f),
	m11(0.0f),
	 
	m12(0.0f),
	m13(0.0f),
	m14(0.0f),
	m15(1.0f)
{	

}

Matrix::~Matrix()
{

}

Matrix& Matrix::operator = (const Matrix& tmp)
{
	this->m0 = tmp.m0;
	this->m1 = tmp.m1;
	this->m2 = tmp.m2;
	this->m3 = tmp.m3;

	this->m4 = tmp.m4;
	this->m5 = tmp.m5;
	this->m6 = tmp.m6;
	this->m7 = tmp.m7;

	this->m8 = tmp.m8;
	this->m9 = tmp.m9;
	this->m10 = tmp.m10;
	this->m11 = tmp.m11;

	this->m12 = tmp.m12;
	this->m13 = tmp.m13;
	this->m14 = tmp.m14;
	this->m15 = tmp.m15;

	return *this;
}


Vect4D Matrix::operator * (const Vect4D& v) const
{
	Vect4D tX;
	Vect4D tY;
	Vect4D tZ;
	Vect4D tW;

	Matrix m = *this;


	tX._m = _mm_shuffle_ps(m.v0._m, m.v2._m, _MM_SHUFFLE(1, 0, 1, 0));
	tZ._m = _mm_shuffle_ps(m.v0._m, m.v2._m, _MM_SHUFFLE(3, 2, 3, 2));
	tY._m = _mm_shuffle_ps(m.v1._m, m.v3._m, _MM_SHUFFLE(1, 0, 1, 0));
	tW._m = _mm_shuffle_ps(m.v1._m, m.v3._m, _MM_SHUFFLE(3, 2, 3, 2));


	Matrix temp;
	Matrix holder;

	holder.v0._m = _mm_set1_ps(v.x);
	holder.v1._m = _mm_set1_ps(v.y);
	holder.v2._m = _mm_set1_ps(v.z);
	holder.v3._m = _mm_set1_ps(v.w);

	Vect4D X;
	Vect4D Y;
	Vect4D Z;
	Vect4D W;

	X._m = _mm_shuffle_ps(tX._m, tY._m, _MM_SHUFFLE(2, 0, 2, 0));
	Y._m = _mm_shuffle_ps(tX._m, tY._m, _MM_SHUFFLE(3, 1, 3, 1));
	Z._m = _mm_shuffle_ps(tZ._m, tW._m, _MM_SHUFFLE(2, 0, 2, 0));
	W._m = _mm_shuffle_ps(tZ._m, tW._m, _MM_SHUFFLE(3, 1, 3, 1));

	X._m = _mm_shuffle_ps(X._m, X._m, _MM_SHUFFLE(3, 1, 2, 0));
	temp.v0._m = _mm_mul_ps(holder.v0._m, X._m);
	Y._m = _mm_shuffle_ps(Y._m, Y._m, _MM_SHUFFLE(3, 1, 2, 0));
	temp.v1._m = _mm_mul_ps(holder.v1._m, Y._m);


	Z._m = _mm_shuffle_ps(Z._m, Z._m, _MM_SHUFFLE(3, 1, 2, 0));
	temp.v2._m = _mm_mul_ps(holder.v2._m, Z._m);
	W._m = _mm_shuffle_ps(W._m, W._m, _MM_SHUFFLE(3, 1, 2, 0));
	temp.v3._m = _mm_mul_ps(holder.v3._m, W._m);

	Vect4D vb;
	vb._m = _mm_add_ps(temp.v0._m, _mm_add_ps(temp.v1._m, _mm_add_ps(temp.v2._m, temp.v3._m)));

	return vb;
}



void Matrix::setGLMatrix(const Vect4D &scale, const Vect4D& pos, const float &rot)
{ 


	float cosign = cosf(rot);
	float sign = sinf(rot);

	float x1 = cosign * scale.x;
	float y1 = -sign * scale.y;

	float x2 = sign * scale.x;
	float y2 = cosign * scale.y;

	m0 = x1 * scale.x;
	m1 = y1 * scale.x;
	m4 = x2 * scale.y;
	m5 = y2 * scale.y;

	m10 = scale.z * scale.z;

	m12 = (pos.x * x1) + ((-5.0f + pos.y) * (x2));
	m13 = (pos.x * y1) + ((-5.0f + pos.y) * (y2));
	m14 = (-40.0f + pos.z) * scale.z;


}


// --- End of File ---
