#include <octree/math/math.h>

float degreeToRadian(float degree)
{
	return degree * PI / 180.f;
}

//https://www.geeksforgeeks.org/smallest-power-of-2-greater-than-or-equal-to-n/
unsigned int nextPowerOf2(unsigned int n)
{
	unsigned count = 0;

	// First n in the below condition
	// is for the case where n is 0
	if (n && !(n & (n - 1)))
		return n;

	while (n != 0)
	{
		n >>= 1;
		count++;
	}

	return 1 << count;
}

unsigned int nearestSqrt(unsigned int n)
{
	unsigned count = 0;

	while (n != 0)
	{
		n >>= 1;
		count++;
	}
	return count;
}

int max(int a, int b)
{
	return a > b ? a : b;
}
float maxf(float a, float b)
{
	return a > b ? a : b;
}