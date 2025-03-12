#include <iostream>
#include <sstream>
#include <vector>
#include <string>

#include <cstdint>
#include <cassert>

#include <my-lib/matrix.h>

int main ()
{
	std::cout << "---------------------- Static" << std::endl;

	Mylib::StaticMatrix<int, 2, 3> m1;
	
	m1[{0, 0}] = 1;
	m1[{0, 1}] = 2;

	std::cout << m1[{0, 0}] << std::endl;
	std::cout << m1[{0, 1}] << std::endl;
	std::cout << m1[{0, 0}] << std::endl;

	std::cout << "---------------------- Dynamic" << std::endl;

	auto m2 = Mylib::Matrix<int>(2, 3);
	
	m2[{0, 0}] = 1;
	m2[{0, 1}] = 2;

	std::cout << m2[{0, 0}] << std::endl;
	std::cout << m2[{0, 1}] << std::endl;
	std::cout << m2[{0, 0}] << std::endl;

	return 0;
}