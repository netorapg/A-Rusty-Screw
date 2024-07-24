#include <iostream>
#include <vector>

#include <cstdint>
#include <cassert>

#include <my-lib/trigger.h>

struct MyFilter {
	inline bool operator() (const int& event)
	{
		return (event > 1000);
	}
};

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

	void callback ()
	{
		std::cout << "oba " << b << std::endl;
	}

	void callback_1 (const int& event_data, int n)
	{
		std::cout << "uhu1 " << b << " event_data " << event_data << " n " << n << std::endl;
	}

	void callback_2 (int n, float c)
	{
		std::cout << "uhu2 " << b << " " << n << " - " << c << std::endl;
	}

	void callback_3 (const int& event_data)
	{
		std::cout << "uhu3 " << b << " event_data " << event_data << std::endl;
	}

	void callback_with_filter (const int& event_data, const MyFilter& filter)
	{
		std::cout << "uhu FILTER " << b << " event_data " << event_data << std::endl;
	}

	void callback_with_filter_and_param (const int& event_data, const MyFilter& filter, float v)
	{
		std::cout << "uhu FILTER " << b << " event_data " << event_data << " v " << v << std::endl;
	}
};

test_t test;

int main ()
{
	auto callback1 = Mylib::Trigger::make_callback_object_with_params<int>(test, &test_t::callback_1, 10);

	Mylib::Trigger::EventHandler<int> event_handler;

	auto d1 = event_handler.subscribe(callback1);

	auto d3 = event_handler.subscribe( Mylib::Trigger::make_callback_object<int>(test, &test_t::callback_3) );

	MyFilter filter;
	auto d4 = event_handler.subscribe( Mylib::Trigger::make_filter_callback_object<int>(filter, test, &test_t::callback_with_filter) );

	auto d5 = event_handler.subscribe( Mylib::Trigger::make_filter_callback_object_with_params<int>(filter, test, &test_t::callback_with_filter_and_param, 8.7) );

	test.b = 99;

	std::cout << "----------------------" << std::endl;
	event_handler.publish(50);

	event_handler.unsubscribe(d1);

	std::cout << "----------------------" << std::endl;
	event_handler.publish(67);

	std::cout << "----------------------" << std::endl;
	event_handler.publish(2000);

	//auto callback3 = Mylib::Trigger::make_callback_object<int>(test, &test_t::callback_3);

	return 0;
}