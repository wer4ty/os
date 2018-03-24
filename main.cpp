#include <iostream>
#include <unistd.h>
#include <pwd.h>
#include <string>

using namespace std;

class Shell {
	private:  
		string path;
		string command;

	public:
		Shell() {
			path = "~/";
			command = "";
		}

		~Shell() {}

		void read() {
			cout << "$_EBash "+path+ ": ";
			getline(cin, command);
			cout <<  command << endl; 
		}

		void eval(string command) {
			if (command == "exit") { exit(0); }
		}

		void start() {
			while (1) {
				Shell::read();
				Shell::eval(command);
			}
		}
};


int main() {
	Shell* s = new Shell;
	s->start();

	//while(1) {}
	
	delete s;
	return 0;
}
