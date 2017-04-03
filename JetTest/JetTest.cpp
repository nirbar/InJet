// JetTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Jet.h"
#include "JetTypeDetector.h"

class other 
{
public:

	static other* JetCtor(Jet<other>* jet)
	{
		JetTypeDetector<other> t;
		return t.construct(jet);
	}

	other()
		: i(3)
	{
	}

	int i;
};

class one
{
public:

	static one* JetCtor(Jet<one>* jet)
	{
		JetTypeDetector<one> t;
		return t.construct(jet);
	}

	one() 
		:i(1) 
	{
	}

	int i;
};

class two : public one
{
private:
	other *other_;

public:

	static two* JetCtor(Jet<two>* jet)
	{
		JetTypeDetector<two, other> t;
		return t.construct(jet);
	}

	two(other* o)
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

