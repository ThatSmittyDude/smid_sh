#include <iostream>
#include <string>
#include <ctime>
#include <unistd.h>
#include <sys/syscall.h>
#include <dirent.h>
#include <cstring>
#include <vector>
#include <algorithm>
#include <sys/stat.h>
#include <sstream>


int pid;
int sh = 1;
const char* path;
std::string user_input;

std::string getTypeSuffix(const std::string& name) {
    struct stat sb;
    if (stat(name.c_str(), &sb) == -1) {
        return ""; // fallback if stat fails
    }

    if (S_ISDIR(sb.st_mode)) return "/";
    if (sb.st_mode & S_IXUSR) return "*"; // executable
    return "";
}


void sh_input(){
	pid = 0;
	std::cout << "smid_sh# ";
	std::cin >> user_input;
}

void clear(){
        std::cout << "\033[H\033[2J\033[3J";
}

void _error(){
        std::cout << "No command for: " << user_input << "\n";
}

void exit(){
        std::cout << "Exiting\n";
        sh = 0;
}

void test() {
        std::cout << "Shell input successful\n";
}

void whoami(){
        const char*  username = getenv("USER");
                if (username) {
                        std::cout << username << std::endl;
                } else {
        std::cerr << "Failed to get username." << std::endl;
	}
}

void date(){
 	struct timespec ts;
 	syscall(SYS_clock_gettime, CLOCK_REALTIME, &ts);

 	struct tm timeinfo;
 	localtime_r(&ts.tv_sec, &timeinfo);

 	char buffer[100];
 	strftime(buffer, sizeof(buffer), "%a %b %d %H:%M:%S %Z %Y", &timeinfo);

 	std::cout << buffer << std::endl;
}

void _unix(){
	std::time_t result = std::time(nullptr);
	std::cout << "UNIX Timestamp: " << result << "\n";
}

void ls(){
	DIR* dir;
	struct dirent* entry;
	std::vector<std::string> files;

	dir = opendir(".");
	if(dir == nullptr){
		std::cout << "Failed to open directory" << "\n";
	}

	while((entry = readdir(dir)) != nullptr){
		if(strcmp(entry->d_name, ".") !=0 && strcmp(entry->d_name, "..") !=0){
			files.push_back(entry->d_name);
		}
	}
	closedir(dir);

	std::sort(files.begin(), files.end());

 	for (const auto& name : files) {
  		std::cout << name << getTypeSuffix(name) << std::endl;
		usleep(10000);
 	}

}

void cd(int argc, char *argv[])
{
  if (argc == 1)
  {
    const char *home = getenv("HOME");
    if (home == NULL)
    {
      fprintf(stderr, "Could not find HOME environment variable.\n");
      return;
    }

    if (chdir(home) != 0)
    {
      perror("chdir");
      return;
    }
  }
  else if (argc > 2)
  {
    fprintf(stderr, "Usage: %s [directory]\n", argv[0]);
    return;
  }
  else
  {
    const char *dir = argv[1];

    if (strcmp(dir, "-") == 0)
    {
      const char *oldpwd = getenv("OLDPWD");
      if (oldpwd == NULL)
      {
        fprintf(stderr, "No previous directory found.\n");
        return;
      }

      if (chdir(oldpwd) != 0)
      {
        perror("chdir");
        return;
      }

      printf("%s\n", oldpwd);
    }
    else
    {
      if (chdir(dir) != 0)
      {
        perror("chdir");
        return;
      }
    }
  }
}


/*
PIDs:
	_error	1
	clear	2
	exit	3
	test	4
	whoami	5
	date	6
	unix	7
	ls	8
	cd	9
*/

int main() {
    while (sh == 1) {
        sh_input();

        // Tokenize input
        std::vector<std::string> tokens;
        std::istringstream iss(user_input);
        std::string token;
        while (iss >> token) {
            tokens.push_back(token);
        }

        // Convert to argc/argv
        std::vector<char*> argv;
        for (auto& t : tokens) {
            argv.push_back(const_cast<char*>(t.c_str()));
        }
        int argc = argv.size();

        // Dispatch commands
        if (tokens.empty()) {
            _error();
        } else if (tokens[0] == "clear") {
            clear();
        } else if (tokens[0] == "exit") {
            exit();
        } else if (tokens[0] == "test") {
            test();
        } else if (tokens[0] == "whoami") {
            whoami();
        } else if (tokens[0] == "date") {
            date();
        } else if (tokens[0] == "unix") {
            _unix();
        } else if (tokens[0] == "ls") {
            ls();
        } else if (tokens[0] == "cd") {
            cd(argc, argv.data());
        } else {
            _error();
        }
    }
    return 0;
}


