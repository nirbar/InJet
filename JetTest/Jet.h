#pragma once
#include <functional>
#include <map>
#include <string>
#include <memory>
#include "CircularDependencyException.h"

typedef std::map<std::string, void*> NamedArgs;

template<typename T>
class Jet
{
public:

	static Jet<T>* Scope(const std::string& scope)
	{
		ScopeMap::const_iterator it = scopeInstances_.find(scope);
		if (it == scopeInstances_.end())
		{
			scopeInstances_[scope].reset(new Jet<T>(scope));
			it = scopeInstances_.find(scope);
		}

		return it->second.get();
	}

	template<typename T1>
	void BindTo()
	{
		bindMode_ = BindMode::Normal;
		delegate_ = std::bind<T*>(&Jet<T1>::ResolveWithArgs, Jet<T1>::Scope(scope_), std::placeholders::_1);
	}

	void BindTo(T* constant)
	{
		bindMode_ = BindMode::Constant;
		constant_ = constant;
	}

	void BindToFactory(std::function<T*(const NamedArgs&)> func)
	{
		bindMode_ = BindMode::Factory;
		factory_ = func;
	}

	void BindToSelf()
	{
		bindMode_ = BindMode::Normal;
		delegate_ = NULL;
	}

	void AsSingleton(bool asSingleton)
	{
		bindMode_ = BindMode::Singleton;
	}

	T* Resolve()
	{
		static NamedArgs empty;
		return ResolveWithArgs(empty);
	}

	T* ResolveWithArgs(const NamedArgs& args)
	{
		T* ptr = NULL;

		try
		{
			switch (bindMode_)
			{
			case BindMode::Constant:
				ptr = constant_;
				break;

			case BindMode::Singleton:
				if (constant_ != NULL)
				{
					ptr = constant_;
					break;
				}

				// Fallthrough
			case BindMode::Normal:
				if (!!delegate_)
				{
					ptr = delegate_(args);
				}
				else if (!!ctor_)
				{
					ptr = ctor_(args);
				}
				break;

			case BindMode::Factory:
				ptr = factory_(args);
				break;

			default:
				break;
			}
		}
		catch (const CircularDependencyException& ex)
		{
			printf("Circular dependency detected:\n%s", ex.DependencyList().c_str());
		}

		if (ptr == NULL)
		{
			throw std::exception("Don't know how to create object");
		}
		return ptr;
	}

	const std::string& scope() const
	{
		return scope_;
	}

private:

	enum BindMode
	{
		Normal,
		Factory,
		Constant,
		Singleton
	};

	// Construct on scope only
	Jet(const std::string& scope)
		: scope_(scope)
		, bindMode_(BindMode::Normal)
	{
		ctor_ = std::bind<T*, T* (Jet<T>*, const NamedArgs&)>(T::JetCtor, this, std::placeholders::_1);
	}

	BindMode bindMode_;
	T* constant_;
	std::function<T*(const NamedArgs&)> ctor_;
	std::function<T*(const NamedArgs&)> delegate_;
	std::function<T*(const NamedArgs&)> factory_;
	std::string scope_;

	typedef std::map<std::string, std::unique_ptr<Jet<T>>> ScopeMap;
	static ScopeMap scopeInstances_;
};

template<typename T>
typename Jet<T>::ScopeMap Jet<T>::scopeInstances_;

// Specialization to lazy:

template<typename T>
class Jet<Jet<T>>
{
public:
	Jet<T>* ResolveWithArgs(const NamedArgs& args)
	{
		return Jet<T>::Scope(this->scope());
	}
};