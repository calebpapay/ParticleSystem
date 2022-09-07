//---------------------------------------------------------------
// Copyright 2022, Ed Keenan, all rights reserved.
//---------------------------------------------------------------

#ifndef MATRIX_H
#define MATRIX_H

// includes
#include "Vect4D.h"

// class
class Matrix
{
public:

	Matrix();	
	Matrix(const Matrix& tmp) = default;
	Matrix& operator=(const Matrix& tmp);

	~Matrix();



	void setGLMatrix(const Vect4D& scale, const Vect4D& position, const float& rotation);
	
	Vect4D operator * (const Vect4D& v) const;

public:
	union
	{
		struct
		{
			Vect4D v0;
			Vect4D v1;
			Vect4D v2;
			Vect4D v3;
		};

		struct
		{
			float m0;
			float m1;
			float m2;
			float m3;
			float m4;
			float m5;
			float m6;
			float m7;
			float m8;
			float m9;
			float m10;
			float m11;
			float m12;
			float m13;
			float m14;
			float m15;
		};
	};
};

#endif  

// --- End of File ---
