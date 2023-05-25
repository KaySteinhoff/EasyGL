#include <iostream>
#include <vector>
#include <initializer_list>

using namespace std;

template <typename F>class EventListener
{
	int dummy = 0;
public:
	F Func;

	template<typename... Args>
	int & operator()(Args... args)
	{	
		Func(args...);
		return dummy;
	}

};

template <typename T> class EventHandler
{
	vector<EventListener<T>> vec;
	
public:
	int length;

	int& operator +=(T func)
	{
		EventListener<T> listener;
		listener.Func = func;
		vec.push_back(listener);
		length++;
		return length;
	}
	
	int& operator -=(T func)
	{
		for(int i = 0;i < length; ++i)
		{
			if(vec[i].Func == func)
			{
				vec.erase(vec.begin()+i);
				length--;
				return length;
			}
		}
		return length;
	}

	template<typename... G>
	int& operator()(G... args)
	{
		for(int i = 0; i < length; ++i)
			vec[i](args...);
			
		return length;
	}

};