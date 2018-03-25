#include <iostream>
#include <unistd.h>
#include <string>
#include <errno.h>
#include <pwd.h>
#include <sys/types.h>
#include <algorithm>
#include <list>

//#include <stdlib.h>


using namespace std;

class Shell {
	private:  
		string path;
		string command;
		string home;
		list<string> tokens;

		void getHomeDir() {
				home = getpwuid(getuid())->pw_dir;
		}

		void getCurrentDir() {
			char buf[1024];
			if(getcwd(buf, sizeof(buf)) != NULL ) {
				path = buf;
				path.replace(path.begin(), path.end()-home.length(), "~/");
			}
			else {
				perror("getcwd() error");
			}
		}

		void changeDir(string newPath) {
			int chdir_res = chdir(newPath.c_str());
			cout << chdir_res << endl;
		}

		list<string> tokenize(string line) {
			string buf;
			stringstream ss(line);

			while (ss >> buf) tokens.push_back(buf);
			return tokens;
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
			if (command == "exit" || cin.eof()) { exit(1); } 
			if (command == "cd")  { Shell::changeDir(command);  p=1; }

			if ( p == 0) { perror("Not found this command"); }
			
		}

		void start() {
			// init
			Shell::getHomeDir();
			Shell::getCurrentDir();
			

			cout << "Welcome to _EBash terminal " << endl;
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
