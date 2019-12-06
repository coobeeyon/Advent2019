#include <cassert>
#include <iostream>

#if 1
const int rangeMin = 183564;
const int rangeMax = 657474;
#else
const int rangeMin = 10;
const int rangeMax = 99;
#endif

bool
isValid(int candidate)
{
	int lastDigit{ candidate % 10 };
	candidate = candidate / 10;

	int  repeatCount{ 1 };
	bool groupOfTwo{false};
	do {
		int digit = candidate % 10;
		if ( digit > lastDigit )
		{
			return false;
		}
		else if ( digit == lastDigit )
		{
			repeatCount++;
		}
		else
		{
			if ( repeatCount == 2 )
			{
				groupOfTwo = true;
			}
			repeatCount = 1;
		}
		candidate = candidate / 10;
		lastDigit = digit;
	} while (candidate > 0);
	if ( repeatCount == 2 )
	{
		groupOfTwo = true;
	}
	return groupOfTwo;
}

int
main()
{
	int numFound{};
	for (int candidate = rangeMin ; candidate <= rangeMax ; ++candidate)
	{
		if ( isValid(candidate) )
		{
			numFound++;
		}
	}
	std::cout << numFound << std::endl;
}
