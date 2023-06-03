#pragma once
#include "Observer.h"

template <typename T>
class Subject
{
public:
    void registerObserver(Observer<T>* observer)
	{
        observers.push_back(observer);
    }

    void unregisterObserver(Observer<T>* observer)
	{
        observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
    }

    void notifyObservers(const std::string& field)
	{
        for (Observer<T>* observer : observers) 
        {
            observer->OnNotify(static_cast<T*>(this), field);
        }
    }

private:
    std::vector<Observer<T>*> observers;
};