#pragma once

#include <stdint.h>
#include <stdarg.h>

void randomSeed();
int getRandom(int min, int max);
int randChoose(int count, ...);
int sign(int x);
float sign(float x);

//
// Overview: A class of 4 color bytes.
//
class Color
{
public:
	inline Color()
	{
		this->r = 0.0f;
		this->g = 0.0f;
		this->b = 0.0f;
		this->a = 0.0f;
	}
	inline Color(float r, float g, float b, float a = 1.0f)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	float r;
	float g;
	float b;
	float a;
};

//
// Overivew: A vector of 2 floats.
//
class Vector2f
{
public:
	inline Vector2f()
	{
		this->x = 0.0f;
		this->y = 0.0f;
	}
	inline Vector2f(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	float x;
	float y;
};

//
// Overivew: A vector of 2, 32 bit ints.
//
class Vector2i
{
public:
	inline Vector2i()
	{
		this->x = 0;
		this->y = 0;
	}
	inline Vector2i(int32_t x, int32_t y)
	{
		this->x = x;
		this->y = y;
	}

	int32_t x;
	int32_t y;
};

//
// Overivew: A vector of 2, unsigned 32 bit ints.
//
class Vector2ui
{
public:
	inline Vector2ui()
	{
		this->x = 0u;
		this->y = 0u;
	}
	inline Vector2ui(uint32_t x, uint32_t y)
	{
		this->x = x;
		this->y = y;
	}

	uint32_t x;
	uint32_t y;
};
