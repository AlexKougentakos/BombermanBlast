#pragma once

template <typename T>
class Observer
{
public:
    virtual ~Observer() {}
    virtual void OnNotify(T* source, const std::string& field) = 0;  // pure virtual function
};
