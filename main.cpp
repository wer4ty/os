#include <iostream>
#include <unistd.h>
#include <string>
#include <cstring>
//#include <cstdlib>
#include <errno.h>
#include <pwd.h>
#include <wait.h>
#include <sys/types.h>
#include <time.h>

#define SIZE 1024

using namespace std;

class Shell {
	private:  
		string path;
		string home;
		int chdir_status;
		int proc_status;
		bool deamon_flag;
		clock_t exec_time;

		char command[SIZE];
		char *args[SIZE];
		int args_len = 0;

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

		void args_length() {
			for(unsigned int i=0;args[i]!=NULL;i++)
  			args_len += strlen(args[i]);
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

		void parsing(char* c[]) {
			string arg;

			for (int k = 1; k < args_len; k++) {
			arg = string(c[k]);

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
				c[k] =  strdup(const_cast<char *>(arg.c_str()));
			}
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

		void printArgs(char* a[]) {
			cout << "[";
			for (int i =0; i< args_len; i++) {
				cout << a[i] << " ";
			}
			cout << "]" << endl;
		}

		void cleanMemory(char* arg[]) {
			// for (int i =0; i<args_len; i++)
			// 	delete [] arg[i];
			// delete [] arg;
			
			args_len = 0;
			cin.clear();
		}

		void tokenize(char *c, char *arg[]) {
			while (*c != '\0') {
				while (*c == ' ' || *c == '\t' || *c == '\n') {
					*c++ = '\0'; args_len++;
				}
				*arg++ = c;
				while (*c != '\0' && *c != ' ' && *c != '\t' && *c != '\n') {
					c++;
				}
			}
			args_len++;
			*arg = '\0';
		}


		void runProcess(char *arg[]) {
			pid_t pid;
			pid_t zombie_proc;

			// prepare to run external command
			if ( strcmp(arg[args_len-1], "&") == 0 ) {
					arg[args_len-1] = NULL;
					args_len--;
					deamon_flag = true;
			}
			else {
				deamon_flag = false;
			}

			switch(pid = fork()) {

				case -1: {
					perror("Fork Error:");
					exit(1);
				}

				// child process
				case 0: {									
					if(execvp(*arg, arg) < 0) {
						perror("Execvp");
						exit(1);
					}					

				break;
				}

				// parent process
				default: {

						// deamon
						if (deamon_flag) {
							cout << "[" << pid << "]" << endl;
							cout << endl;
						}

						// regular process
						else {
							wait(&proc_status);

							// zombie handle
							while ((zombie_proc = waitpid(-1, &proc_status, WNOHANG)) > 0) {
								cout << "exited, status=" << proc_status << endl;
							}

						}

				break;
				}

			}
		}

	public:
		Shell() { cout << "Welcome to _EBash terminal " << endl; }

		~Shell() {}

		void read() {
			cout << "$_EBash "+path+ ": ";
			cin.getline(command, SIZE);
		}

		void eval(char command[]) {
		
			Shell::tokenize(command, args);

			Shell::parsing(args);

			if ( (strcmp(args[0], "exit") == 0) || cin.eof()) 
				{ exit(0); } 
			

			if (strcmp(args[0], "cd") == 0)  { 
				Shell::changeDir(args[1]);  
			}

			// run external processes
			else {
				Shell::runProcess(args);
			}

		}

		void start() {
			// init
			Shell::getHomeDir();
			Shell::getCurrentDir();
			
			while (1) {
				Shell::read();

				exec_time = clock();
				Shell::eval(command);

				cout << "\n\t\t[[ ===--- Time: { " << (double)(clock() - exec_time) / CLOCKS_PER_SEC << "sec } Status: { " << proc_status << " } ---===]]" << endl; 

				Shell::cleanMemory(args);
			}
		}
};

int main() {
	Shell* s = new Shell;
	s->start();	
	delete s;


	return 0;
}
