#ifndef MATH_H
#define MATH_H

#include "Window.h" 

struct Vector2
{
	float X = 0.0, Y = 0.0;

	constexpr friend Vector2 operator + (const Vector2& a, const Vector2& other) 
	{
		return Vector2(a.X + other.X, a.Y + other.Y);
	}
	
	constexpr auto operator <=> (const Vector2& other) const = default;

	constexpr Vector2& operator += (const Vector2& other)
	{
		X += other.X;
		Y += other.Y;
		return *this;
	}

	constexpr friend bool operator == (const Vector2& a, const Vector2& b)
	{
		return a.X == b.X && b.Y == a.Y;
	}
};

inline bool IsColliding(const Rect& a, const Rect& b)
{
    if (a.x < b.x + b.w // is the left side before the right side of b? 
        && a.x + a.w > b.x 
        && a.y < b.y + b.h 
        && a.y + a.h > b.y// is the right side after the left side of b?
        )
    {
        return true;
    }
    return false;
}

inline bool operator == (const Rect& a, const Rect& b)
{
	return a.x == b.x && a.w == b.w && a.y == b.y && a.h == b.h;
}

#endif