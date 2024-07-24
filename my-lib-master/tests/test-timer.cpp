#include <iostream>
#include <vector>
#include <coroutine>
#include <list>

#include <cstdint>
#include <cassert>

#include <my-lib/timer.h>

uint32_t global_time = 0;

uint32_t get_time ()
{
	return global_time;
}

//using Timer = Mylib::Trigger::Timer<uint32_t, decltype(get_time)>;
//auto timer = Mylib::Trigger::Timer(get_time);

Mylib::Trigger::Timer timer(get_time);

using Timer = decltype(timer);

bool alive = true;

Timer::Coroutine coro_print_values ()
{
	while (alive) {
		std::cout << "\tcoro_print_values loop start time=" << global_time << std::endl;
		co_await timer.coroutine_wait(3);
		std::cout << "\tcoro_print_values loop end time=" << global_time << std::endl;
	}
}

void test_coroutine ()
{
	std::cout << "test_coroutine" << std::endl;

	global_time = 0;

	Timer::Coroutine timer_coroutine = coro_print_values();

	timer.register_coroutine(timer_coroutine);

	while (alive) {
		std::cout << "process events time=" << global_time << std::endl;
		timer.trigger_events();

		global_time++;

		if (global_time > 10)
			alive = false;
	}

//	test_timer.clear_events();
}

class test_t
{
public:

	int b;

	test_t ()
	{
		this->b = 5555555;
		std::cout << "test constructor" << std::endl;
	}

	test_t (const test_t& other)
	{
		this->b = other.b;
		std::cout << "test COPY constructor" << std::endl;
	}

	test_t (const test_t&& other)
	{
		this->b = other.b;
		std::cout << "test MOVE constructor" << std::endl;
	}

	void callback (Timer::Event& event)
	{
		std::cout << "oba " << b  << " time " << event.time << std::endl;
	}

	void callback_1 (const Timer::Event& event, int n)
	{
		std::cout << "uhu1 " << b << " time " << event.time << " n " << n << std::endl;
	}

	void callback_2 (int n, float c)
	{
		std::cout << "uhu2 " << b << " " << n << " - " << c << std::endl;
	}
};

test_t test;

int main ()
{
	std::cout << "scheduling object function without params" << std::endl;
	timer.schedule_event(10, Mylib::Trigger::make_callback_object_with_params< Timer::Event >(test, &test_t::callback_1, 10));

	timer.schedule_event(20, Mylib::Trigger::make_callback_object< Timer::Event >(test, &test_t::callback));

	timer.schedule_event(5, Mylib::Trigger::make_callback_object< Timer::Event >(test, &test_t::callback));

	timer.schedule_event(1, Mylib::Trigger::make_callback_object< Timer::Event >(test, &test_t::callback));

	timer.schedule_event(3, Mylib::Trigger::make_callback_object< Timer::Event >(test, &test_t::callback));

	auto d = timer.schedule_event(55, Mylib::Trigger::make_callback_object< Timer::Event >(test, &test_t::callback));

	std::cout << "created " << timer.get_n_scheduled_events() << " events" << std::endl;

	timer.unschedule_event(d);

	std::cout << "created " << timer.get_n_scheduled_events() << " events" << std::endl;

	// ensure that test object was not copy/moved

	test.b = 99;

	global_time = 100;

	timer.trigger_events();

	test_coroutine();

	return 0;
}