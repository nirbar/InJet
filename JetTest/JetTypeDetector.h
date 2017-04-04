#pragma once
#include <type_traits>
#include <map>
#include <string>
#include "Jet.h"
#include "CircularDependencyException.h"

template<size_t a, typename ...ARGS>
static bool constexpr IsArgCount()
{
	return (a == sizeof...(ARGS));
}

template <typename C, typename ...ARGS>
class JetTypeDetector
{
private:
	static bool circularGuard_;

	static void CircularGuard()
	{
		if (circularGuard_)
		{
			throw CircularDependencyException(typeid(C).name());
		}
		circularGuard_ = true;
	}

public:
		
	static C* JetCtor(Jet<C>* jet, const NamedArgs& args)
	{
		CircularGuard();

		try
		{
			C* p = construct<ARGS...>(jet, args);
			circularGuard_ = false;
			return p;
		}
		catch (CircularDependencyException ex)
		{
			throw CircularDependencyException(ex, typeid(C).name());
		}
	}

#pragma region Constructor templates

	template <typename ...ARGSS>
	JetTypeDetector(typename std::enable_if<IsArgCount<0, ARGSS...>(), const NamedArgs&>::type)
	{
	}

	template <typename A1, typename ...ARGSS>
	JetTypeDetector(A1*, typename std::enable_if<IsArgCount<0, ARGSS...>(), const NamedArgs&>::type)
	{
	}

	template <typename A1, typename A2, typename ...ARGSS>
	JetTypeDetector(A1*, A2*, typename std::enable_if<IsArgCount<0, ARGSS...>(), const NamedArgs&>::type)
	{
	}

	template <typename A1, typename A2, typename A3, typename ...ARGSS>
	JetTypeDetector(A1*, A2*, A3*, typename std::enable_if<IsArgCount<0, ARGSS...>(), const NamedArgs&>::type)
	{
	}

	template <typename A1, typename A2, typename A3, typename A4, typename ...ARGSS>
	JetTypeDetector(A1*, A2*, A3*, A4*, typename std::enable_if<IsArgCount<0, ARGSS...>(), const NamedArgs&>::type)
	{
	}

#pragma endregion

private:
#pragma region construct function templates

	template <typename ...ARGS>
	static typename std::enable_if<IsArgCount<0, ARGS...>(), C*>::type construct(Jet<C>* jet, const NamedArgs& args)
	{
		return new C(args);
	}

	template <typename A1>
	static typename std::enable_if<IsArgCount<1, ARGS...>(), C*>::type construct(Jet<C>* jet, const NamedArgs& args)
	{
		Jet<A1>* a1 = Jet<A1>::Scope(jet->scope());

		return new C(a1->Resolve(), args);
	}

	template <typename A1, typename A2>
	static typename std::enable_if<IsArgCount<2, ARGS...>(), C*>::type construct(Jet<C>* jet, const NamedArgs& args)
	{
		Jet<A1>* a1 = Jet<A1>::Scope(jet->scope());
		Jet<A2>* a2 = Jet<A2>::Scope(jet->scope());

		return new C(a1->Resolve(), a2->Resolve(), args);
	}

	template <typename A1, typename A2, typename A3>
	static typename std::enable_if<IsArgCount<3, ARGS...>(), C*>::type construct(Jet<C>* jet, const NamedArgs& args)
	{
		Jet<A1>* a1 = Jet<A1>::Scope(jet->scope());
		Jet<A2>* a2 = Jet<A2>::Scope(jet->scope());
		Jet<A3>* a3 = Jet<A3>::Scope(jet->scope());

		return new C(a1->Resolve(), a2->Resolve(), a3->Resolve(), args);
	}

	template <typename A1, typename A2, typename A3, typename A4>
	static typename std::enable_if<IsArgCount<4, ARGS...>(), C*>::type construct(Jet<C>* jet, const NamedArgs& args)
	{
		Jet<A1>* a1 = Jet<A1>::Scope(jet->scope());
		Jet<A2>* a2 = Jet<A2>::Scope(jet->scope());
		Jet<A3>* a3 = Jet<A3>::Scope(jet->scope());
		Jet<A3>* a4 = Jet<A4>::Scope(jet->scope());

		return new C(a1->Resolve(), a2->Resolve(), a3->Resolve(), a4->Resolve(), args);
	}

#pragma endregion
};

template <typename C, typename ...ARGS>
bool JetTypeDetector<C, ARGS...>::circularGuard_ = false;