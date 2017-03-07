#include <iostream>
#include "Motor.h"


using namespace std;

int main(){

	unsigned char Address = 0;
	unsigned char Status = 0;
	int Value = 0;

	Motor *Mot = new Motor(Address,Status,Value);

	return 0;
}
