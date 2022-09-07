//---------------------------------------------------------------
// Copyright 2022, Ed Keenan, all rights reserved.
//---------------------------------------------------------------

#ifndef Vect4D_H
#define Vect4D_H

// includes
#include <xmmintrin.h>
#include <smmintrin.h> 


// class
class Vect4D : public Align16
{
public:


	Vect4D();
	Vect4D(const Vect4D& t) = default;

	Vect4D& operator = (const Vect4D& tmp);
	Vect4D(float tx, float ty, float tz, float tw = 1.0f);
	~Vect4D();

	Vect4D operator * (const Vect4D& v) const;

	Vect4D Cross(const Vect4D &vin) const;
	Vect4D norm(const Vect4D& v) const;
	

	Vect4D operator * (float scale);
	Vect4D operator + (const Vect4D &t) const;

public:

	union
	{
		__m128	_m;

		struct
		{
			float x;
			float y;
			float z;
			float w;
		};
	};
};

#endif

// --- End of File ---
