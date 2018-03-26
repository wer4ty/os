#include <iostream>
#include <unistd.h>
#include <string>
#include <errno.h>
#include <pwd.h>
#include <cstring>
#include <sys/types.h>

//#include <algorithm>
//#include <stdlib.h>


using namespace std;

class Shell {
	private:  
		string path;
		string command;
		string home;

		int tokens_counter;
		char* tokens;

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

		/*void clearTokens() {
			for (int i=0; i < tokens_counter; i++) { tokens[i] = NULL ; }
				tokens_counter = 0
		}*/

		void tokenize(string command_line) {
			const char* str = command_line.c_str();

			char * duplicate = strdup(str);

    		char* chars_array = strtok(duplicate, " ");
    		while(chars_array) {
    			tokens_counter++;
        		cout << chars_array << '\n';
        		chars_array = strtok(NULL, "#:");
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

			Shell::tokenize(command);
			
			if (command == "exit" || cin.eof()) { exit(1); } 
			if (command == "cd")  { 
				Shell::changeDir(command);  
				p=1; 
			}

			if ( p == 0) { perror("Not found this command"); }
			
			//Shell::clearTokens();
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
