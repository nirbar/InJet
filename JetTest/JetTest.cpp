// JetTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Jet.h"
#include "JetTypeDetector.h"

class other : public  JetTypeDetector<other>
{
public:

	other(const NamedArgs& args)
		: JetTypeDetector<other>(args)
		, i(3)
	{
	}

	int i;
};

class one: public JetTypeDetector<one>
{
public:

	one(const NamedArgs& args)
		: JetTypeDetector<one>(args)
		, i(1) 
	{
	}

	int i;
};

class two : public one, public JetTypeDetector<two, other>
{
public:

	using JetTypeDetector<two, other>::JetCtor;

	two(other* o, const NamedArgs& args)
		: JetTypeDetector<two, other>(o, args)
		, one(args)
	{
		i = o->i;
	}
};

int main()
{
	Jet<one>* _1 = Jet<one>::Scope("1");

	one* i = _1->Resolve();
	printf("%i\n", i->i);

	_1->BindTo<two>();
	i = _1->Resolve();
	printf("%i\n", i->i);

	Jet<one>* _2 = Jet<one>::Scope("2");
	i = _2->Resolve();
	printf("%i\n", i->i);

	i = _1->Resolve();
	printf("%i\n", i->i);
	
	return 0;
}

