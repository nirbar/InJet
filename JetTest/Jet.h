#pragma once
#include <functional>
#include <map>
#include <string>
#include <memory>

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
		delegate_ = std::bind<T*>(&Jet<T1>::Resolve, Jet<T1>::Scope(scope_));
	}

	void BindTo(T* constant)
	{
		bindConstant_ = true;
		constant_ = constant;
	}

	void ConstructWith(std::function<T*(void)>& ctor)
	{
		bindConstant_ = false;
		ctor_ = ctor;
	}

	void ConstructWith(T* (*func)(void))
	{
		bindConstant_ = false;
		ctor_ = func;
	}

	void BindToSelf()
	{
		bindConstant_ = false;
		delegate_ = NULL;
	}

	T* Resolve()
	{
		if (bindConstant_)
		{
			return constant_;
		}

		if (!!delegate_)
		{
			return delegate_();
		}

		if (!!ctor_)
		{
			return ctor_();
		}

		throw std::exception("Don't know how to create object");
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
	{
		ctor_ = std::bind(T::JetCtor, this);
	}

	bool bindConstant_;
	T* constant_;
	std::function<T*(void)> ctor_;
	std::function<T*(void)> delegate_;
	std::string scope_;

	typedef T* (*JetBuilder)(Jet<T>*);
	typedef std::map<std::string, std::unique_ptr<Jet<T>>> ScopeMap;
	static ScopeMap scopeInstances_;
};

template<typename T>
typename Jet<T>::ScopeMap Jet<T>::scopeInstances_;
