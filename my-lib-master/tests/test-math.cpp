#include <iostream>
#include <sstream>
#include <vector>
#include <string>

#include <cstdint>
#include <cassert>

#include <my-lib/math.h>
#include <my-lib/math-vector.h>
#include <my-lib/math-matrix.h>

using namespace Mylib::Math;

int main ()
{
	std::cout << "----------------------" << std::endl;
	Vector2f v1(2.0f, 3.0f);
	std::cout << v1 << std::endl;

	Matrix4f m;

	std::cout << "----------------------" << std::endl;
	m.set_identity();
	std::cout << m << std::endl;

	std::cout << "----------------------" << std::endl;
	m.set_scale(v1);
	std::cout << m << std::endl;

	std::cout << "----------------------" << std::endl;
	m.set_translate(v1);
	std::cout << m << std::endl;

	return 0;
}