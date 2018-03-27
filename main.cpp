#include <iostream>
#include <unistd.h>
#include <string>
#include <cstring>
#include <cstdlib>
#include <errno.h>
#include <pwd.h>
#include <vector>
#include <sys/types.h>


using namespace std;

class Shell {
	private:  
		string path;
		string command;
		string home;
		int chdir_status;

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

		bool isCapital(char a) {
			if (a >= 'A' && a <= 'X') return true;
			return false;
		}

		bool isLower(char a) {
			if (a >= 'a' && a <= 'z') return true;
			return false;
		}

		bool isNum(char a) {
			if (a >= '0' && a <= '9') return true;
			return false;
		}

		string parsing(string arg) {
			for (unsigned int i=0; i<arg.length(); i++) {
				if (arg[i] == '~') {
					arg.erase(i,1); // remove ~
					arg.insert(0, home); // add from start home directory
				}

				if (arg[i] == '$' && arg[i+1] == '?') {
					arg.erase(i, 2); // remove $?
					string tmp = to_string(chdir_status);
					arg.insert(i, tmp);
				}

				// variables $VAR
				if (arg[i] == '$' && isCapital(arg[i+1])) {
					string var_name;
					unsigned int j=i+1;
					while (  isCapital(arg[j]) || isLower(arg[j]) || isNum(arg[j]) ) {
						var_name.push_back(arg[j]);
						j++;
					}
					
					char* var_value = getenv(var_name.c_str());
					if (var_value != NULL) {
						arg.erase(i, i+var_name.size()+1); // +1 because of $ at the beginning
						arg.insert(i, var_value);
					}
				}
			}
			cout << arg << endl;
			return arg;
		}

		void changeDir(string newPath) {
			if (!newPath.empty()) {
				
				chdir_status = chdir(newPath.c_str());
				if (chdir_status >= 0) {
					
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

		void runProcess(vector<string> v) {
			int status;
			int pid;

			switch(pid = fork()) {
				
				case -1:
					cout << "Error creating process" << endl;
					exit(-1);


				// child process
				case 0:
					char ** args = new char*[tokens.size()];
					for (i=0; i< tokens.size(); i++) {
						args[i] = new char[tokens.size() + 1];
						strcpy(args[i], tokens[i].c_str());
					}

					execvp(tokens[0].c_str(), args); // run external process

					// clean up memory
					for (i=0; i<tokens.size(); i++) 
						delete [] args[i];
					
					delete [] args;

				break;

				// parent process
				default:

					if(v[v.size()-1] == "&") { // deamon process (deamon runs parallel with parent process)
						 cout << "[" << pid << "]" << endl;
						}
						
					 else { // regular process (wait while child process finish)  
						//wait(&status);
						
						}

				break;
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
		
			Shell::tokenize(command, tokens);

			// parsing
			for (unsigned int k=1; k < tokens.size(); k++) 
				tokens[k] = Shell::parsing(tokens[k]);

			if (tokens[0] == "exit" || cin.eof()) 
				{ exit(0); } 
			

			if (tokens[0] == "cd")  { 
				Shell::changeDir(tokens[1]);  
			}

			// run external processes
			else {
				Shell::runProcess(tokens);
			}

			
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

	//for (int i=0; i < argc; i++) { cout << env[i] << endl; }

	Shell* s = new Shell;
	s->start();
	
	delete s;
	return 0;
}
