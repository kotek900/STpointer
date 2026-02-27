#include <string>
#include <iostream>

#include "STpointer.h"

class MyClass : public STnode {
public:
	std::string name;

	STpointer<MyClass> pointer1;
	STpointer<MyClass> pointer2;

	MyClass(std::string name) {
		pointer1.setParent(this);
		pointer2.setParent(this);
		this->name = name;
		std::cout << "Created: " << name << std::endl;
	}

	~MyClass() {
		std::cout << "Destroyed: " << name << std::endl;
	}
};

int main() {
	STpointer<MyClass> objectA = STpointer<MyClass>(new MyClass("A"));
	{
		STpointer<MyClass> objectB = STpointer<MyClass>(NEW, "B"); // fancy new way to create objects (can be disabled with #define DISABLE_NEW_IN_STPOINTER before #include "STpointer.h")
		{
			// circular reference
			STpointer<MyClass> objectC = STpointer<MyClass>(new MyClass("C"));
			objectB->pointer1 = objectC;
			objectC->pointer1 = objectB;
		}
		objectA->pointer1 = objectB;
		objectA->pointer2 = objectB;
	}
	objectA->pointer1 = objectA;
	objectA->pointer2.unset();
	(*objectA).pointer1 = objectA;
	objectA.unset();
	return 0;
}