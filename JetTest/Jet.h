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
		bindConstant_ = false;
		delegate_ = std::bind<T*>(&Jet<T1>::ResolveWithArgs, Jet<T1>::Scope(scope_), std::placeholders::_1);
	}

	void BindTo(T* constant)
	{
		bindConstant_ = true;
		constant_ = constant;
	}

	void BindToSelf()
	{
		bindConstant_ = false;
		delegate_ = NULL;
	}

	void AsSingleton(bool asSingleton)
	{
		asSingleton_ = asSingleton;
	}

	T* Resolve()
	{
		static NamedArgs empty;
		return ResolveWithArgs(empty);
	}

	T* ResolveWithArgs(const NamedArgs& args)
	{
		if (bindConstant_)
		{
			return constant_;
		}

		if (asSingleton_ && (constant_ != NULL))
		{
			return constant_;
		}

		T* ptr = NULL;
		try 
		{
			if (!!delegate_)
			{
				ptr = delegate_(args);
			}
			else if (!!ctor_)
			{
				ptr = ctor_(args);
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

		if (asSingleton_)
		{
			constant_ = ptr;
		}
		return ptr;
	}

	const std::string& scope() const
	{
		return scope_;
	}

private:

	// Construct on scope only
	Jet(const std::string& scope)
		: scope_(scope)
		, bindConstant_(false)
		, asSingleton_(false)
	{
		ctor_ = std::bind<T*, T* (Jet<T>*, const NamedArgs&)>(T::JetCtor, this, std::placeholders::_1);
	}

	bool bindConstant_;
	bool asSingleton_;
	T* constant_;
	std::function<T*(const NamedArgs&)> ctor_;
	std::function<T*(const NamedArgs&)> delegate_;
	std::string scope_;

	typedef std::map<std::string, std::unique_ptr<Jet<T>>> ScopeMap;
	static ScopeMap scopeInstances_;
};

template<typename T>
typename Jet<T>::ScopeMap Jet<T>::scopeInstances_;
