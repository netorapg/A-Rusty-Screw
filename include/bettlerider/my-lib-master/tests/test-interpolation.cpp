#include <iostream>

#include <my-lib/math-vector.h>
#include <my-lib/interpolation.h>

using InterpolationManager = Mylib::InterpolationManager<float>;
using Vector = Mylib::Math::Vector<float, 2>;

InterpolationManager interpolation_manager;
bool alive = true;
float y;
float global_time;
Vector v;

void callback (InterpolationManager::Event& event)
{
	std::cout << "interpolation of " << event.interpolator->get_target()  << " finished" << std::endl;
}

Mylib::Coroutine coro_print_values ()
{
	while (global_time < 10) {
		std::cout << "\tcoro_print_values loop start time=" << global_time << std::endl;
		co_await interpolation_manager.coroutine_wait_interpolate_linear(14.0f, &y, 100.0f, 200.0f);
		std::cout << "\tcoro_print_values loop end time=" << global_time << std::endl;
	}

	std::cout << "\t\t\tcoro_print_values done" << std::endl;
}


int main ()
{
	float x;

	interpolation_manager.interpolate_linear(10.0f, &x, 2.0f, 10.0f, Mylib::Event::make_callback_function< InterpolationManager::Event >(&callback));
	interpolation_manager.interpolate_linear(5.0f, &v, Vector(0.0, 0.0), Vector(-10.0, 10.0), Mylib::Event::make_callback_function< InterpolationManager::Event >(&callback));
	
	Mylib::Coroutine coroutine = coro_print_values();
	Mylib::initialize_coroutine(coroutine);

	std::cout << "&x = " << &x << " value = " << x << std::endl;

	const float delta_x = 1.0f;

	for (global_time = 0; global_time < 20; global_time += delta_x) {
		interpolation_manager.process_interpolation(delta_x);

		std::cout << "x = " << x << std::endl;
		std::cout << "y = " << y << std::endl;
		std::cout << "v = " << v << std::endl;
	}

	alive = false;

	return 0;
}