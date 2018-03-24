#include <iostream>
#include <unistd.h>
#include <pwd.h>
#include <errno.h>
#include <string>

using namespace std;

class Shell {
	private:  
		string path;
		string command;

		void getCurrentDir() {
			char buf[1024];
			if(getcwd(buf, sizeof(buf)) != NULL ) {
				path = buf;
			}
			else {
				perror("getcwd() error");
			}
		}

	public:
		Shell() {
			path = "~/";
			command = "";
		}

		~Shell() {}

		void read() {
			cout << "$_EBash "+path+ ": ";
			getline(cin, command); 
		}

		void eval(string command) {
			int p = 0; // flag for check if command is in list of supported commands
			if (command == "exit") { exit(1); p=1; } 
			if (command == "cd")  { cout << command << endl;  p=1; }

			
			if ( p == 0) { perror("Not found this command"); }
			
		}

		void start() {
			Shell::getCurrentDir();
			cout << "Welcome to _EBash terminal" << endl;
			while (1) {
				Shell::read();
				Shell::eval(command);
			}
		}
};


int main(int argc, char* argv[]) {

	//for (int i=0; i < argc; i++) { cout << argv[i] << endl; }

	Shell* s = new Shell;
	s->start();
	
	delete s;
	return 0;
}
