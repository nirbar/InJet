#pragma once
#include <type_traits>
#include "Jet.h"

template<size_t a, typename ...ARGS>
static bool constexpr IsArgCount()
{
	return (a == sizeof...(ARGS));
}

template <typename C, typename ...ARGS>
class JetTypeDetector
{
public:
		
	C* construct(Jet<C>* jet)
	{
		return construct<ARGS...>(jet);
	}

	template <typename ...ARGS>
	typename std::enable_if<IsArgCount<0, ARGS...>(), C*>::type construct(Jet<C>* jet)
	{
		return new C();
	}

	template <typename A1>
	typename std::enable_if<IsArgCount<1, ARGS...>(), C*>::type construct(Jet<C>* jet)
	{
		Jet<A1>* a1 = Jet<A1>::Scope(jet->scope());

		return new C(a1->Resolve());
	}

	template <typename A1, typename A2>
	typename std::enable_if<IsArgCount<2, ARGS...>(), C*>::type construct(Jet<C>* jet)
	{
		Jet<A1>* a1 = Jet<A1>::Scope(jet->scope());
		Jet<A2>* a2 = Jet<A2>::Scope(jet->scope());

		return new C(a1->Resolve(), a2->Resolve());
	}

	template <typename A1, typename A2, typename A3>
	typename std::enable_if<IsArgCount<3, ARGS...>(), C*>::type construct(Jet<C>* jet)
	{
		Jet<A1>* a1 = Jet<A1>::Scope(jet->scope());
		Jet<A2>* a2 = Jet<A2>::Scope(jet->scope());
		Jet<A3>* a3 = Jet<A3>::Scope(jet->scope());

		return new C(a1->Resolve(), a2->Resolve(), a3->Resolve());
	}

	template <typename A1, typename A2, typename A3, typename A4>
	typename std::enable_if<IsArgCount<4, ARGS...>(), C*>::type construct(Jet<C>* jet)
	{
		Jet<A1>* a1 = Jet<A1>::Scope(jet->scope());
		Jet<A2>* a2 = Jet<A2>::Scope(jet->scope());
		Jet<A3>* a3 = Jet<A3>::Scope(jet->scope());
		Jet<A3>* a4 = Jet<A4>::Scope(jet->scope());

		return new C(a1->Resolve(), a2->Resolve(), a3->Resolve(), a4->Resolve());
	}
};
