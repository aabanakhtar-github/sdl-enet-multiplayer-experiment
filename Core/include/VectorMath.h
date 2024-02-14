#ifndef MATH_H
#define MATH_H

struct Vector2
{
	float X = 0.0, Y = 0.0;

	Vector2 operator + (Vector2& other) 
	{
		return Vector2(X + other.X, Y + other.Y);
	}
	
	Vector2& operator += (const Vector2& other)
	{
		X += other.X;
		Y += other.Y;
		return *this;
	}
};

#endif