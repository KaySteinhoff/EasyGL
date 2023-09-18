#pragma once

#include <functional>
#include <list>
#include <algorithm>
#include <utility>
#include <atomic>
#include <mutex>
#include <future>
#include <any>

class EventArgs {};

class BaseEmmiter
{
public:
	virtual void Callback(EventArgs* args) = 0;

};

template<class T>
class Emmiter : public BaseEmmiter
{
public:
	
	T* m_obj;
	void(T::*m_func)(EventArgs*);
		
	Emmiter(T* obj, void(T::*func)(EventArgs*)) : m_obj(obj), m_func(func) {}
	
	void Callback(EventArgs* args) override
	{
		(m_obj->*m_func)(args);
	}
};

class EventHandler
{
private:
	int dummy = 0;
	std::vector<BaseEmmiter*> emmiters;
	
public:
	
	template<class T>
	void AddListener(T* obj, void(T::*func)(EventArgs*))
	{
		emmiters.push_back(new Emmiter(obj, func));
	}
	
	void RemoveListener(int index)
	{
		emmiters.erase(emmiters.begin()+index);
	}
	
	int& operator()(EventArgs* args)
	{
		for(int i = 0; i < emmiters.size(); ++i)
		{
			emmiters[i]->Callback(args);
		}
		return dummy;
	}
	
};
