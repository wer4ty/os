#include <iostream>
#include <unistd.h>
#include <pwd.h>
#include <string>

using namespace std;

int main() {

string command;

	while(1) {
	cout << "$_EBash ~/ex2:";
	getline(cin, command);
	cout <<  command << endl;

	if (command == "exit") { break; }
	}
	return 0;
}
