#include <iostream>
#include <unistd.h>
#include <string>
#include <errno.h>
#include <pwd.h>
#include <cstring>
#include <sys/types.h>
#include <vector>



using namespace std;

class Shell {
	private:  
		string path;
		string command;
		string home;

		vector<string> tokens;

		void getHomeDir() {
				home = getpwuid(getuid())->pw_dir;
		}

		void getCurrentDir() {
			char buf[1024];
			if(getcwd(buf, sizeof(buf)) != NULL ) {
				path = buf;
				
				if(path.length() > home.length()) {
					path.erase(0, home.size());
					path.insert(0, 1,'~');
				}
			}
			else {
				perror("getcwd() error");
			}
		}

		void changeDir(string newPath) {
			if (!newPath.empty()) {
				int chdir_res = chdir(newPath.c_str());
				if (chdir_res >= 0) {
					
					Shell::getCurrentDir();

				}
				else  perror("Error");
				
			}
			else {
					perror("Error");
				}

		}

		void clearTokens() {
				tokens.clear();
		}

		void tokenize(string str, vector<string> &v) {
			size_t start = str.find_first_not_of(" "), end=start;

			while (start != string::npos) {
				end = str.find(" ", start);
				v.push_back(str.substr(start, end-start));
				start = str.find_first_not_of(" ", end);
			}
		}

		void printTokens(vector<string> v) {
			for (unsigned int i=0; i<v.size(); i++) {
				cout << v[i] << endl;
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
			
			//string exec_command;
			//char* args[1024];

			Shell::tokenize(command, tokens);

			if (tokens[0] == "exit" || cin.eof()) 
				{ exit(0); } 
			

			if (tokens[0] == "cd")  { 
				Shell::changeDir(tokens[1]);  
				p=1; 
			}

			Shell::printTokens(tokens);

			if ( p == 0) { perror("Not found this command"); }
			
			Shell::clearTokens();
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
