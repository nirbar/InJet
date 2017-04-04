#pragma once

#include <exception>
#include <string>

class CircularDependencyException : public std::exception
{
public:
	CircularDependencyException(const char* type)
		: std::exception(type)
		, list_(type)
	{
	}

	CircularDependencyException(const CircularDependencyException& prev, const char* type)
		: std::exception(type)
		, list_(type)
	{
		list_ += "\n" + prev.list_;
	}

	const std::string& DependencyList() const
	{
		return list_;
	}

private:
	std::string list_;
};
