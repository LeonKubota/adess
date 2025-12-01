#include <stdio.h>
#include <stdbool.h>
#include <dirent.h> // Probably OK on Linux/MacOS, need to test for W*ndows
#include <string.h>
#include <ctype.h> // for 'isdigit()'

#ifdef _WIN32
#include <windows.h>
#include <direct.h> // for _getcwd
#define PATH_SEPARATOR "\\" // Windows' weird things (idiot Bill Gates)
#define getcwd _getcwd // so that its the same later
#else
#include <unistd.h> // for gecwd
#define PATH_SEPARATOR "/" // for sane people
#endif

#include "utils.h"
#include "files/check.h"

bool checkFileExistsIn(char *path, char *filename) {
	struct dirent *entry;
	DIR *dir = opendir(path);

	while ((entry = readdir(dir)) != NULL) {
		if (strstr(entry -> d_name, filename)) {
			return true;
		}
	}

	return false;
}

bool checkFileExists(char *path) {
	FILE *file = fopen(path, "r");
	
	if (file != NULL) {
		return true;
	}

	return false;
}

char *findProjectFile(char *path) {
	// If there is no file ending in '.adess'
	if (!checkFileExistsIn(path, ".adess")) {
		e_fatal("project file not found in '%s'\n", path);
		return NULL;
	}

	struct dirent *entry;
	DIR *dir = opendir(path);
	char *projectFilePath = NULL;
	
	while ((entry = readdir(dir)) != NULL) {
		if (strstr(entry -> d_name, ".adess")) {
			if (projectFilePath == NULL) {
				projectFilePath = entry -> d_name;  
			} else {
				e_fatal("multiple projects file found in '%s'\n", path); 
				return NULL;
			}
		}
	}

	return strcat(path, projectFilePath);
}

char *getCurDirectory(char *userpath) {
	char cwd[4096]; // Should be enough (even overkill)
	
	if (getcwd(cwd, sizeof(cwd)) != NULL) {
		if (userpath == NULL) {
			return strcat(cwd, PATH_SEPARATOR);
		} else {
			return strcat(strcat(strcat(cwd, PATH_SEPARATOR), userpath), PATH_SEPARATOR);
		}
	} else {
		e_fatal("directory not found\n");
	}
	return NULL;
}

bool checkValidity(char *path) {
	FILE *file = fopen(path, "r");
	
	if (file == NULL) {
		e_fatal("file '%s' not found\n");
		return false;
	}

	char line[1024];
	char varname[1024] = "";
	char type = ' '; // a = any; s = string; i = int; f = float; b = boolean; m = multi (int or bool, 0 or 1)
	char *typestring;
	int i = 0;
	
	while (fgets(line, sizeof(line), file)) {
		// Skip comments
		if (line[0] == '/' && line[1] == '/') {
			i++;
			continue;
		}

		strncpy(varname, line, sizeof(varname) - 1);
	
		if (varname[0] == '\n') {
			i++;
			continue;
		}
		
		// Get the type of the value
		typestring = strchr(varname, '=');
		typestring++;

		// Skip leading whitespace
		while (typestring[0] == ' ' || typestring[0] == '\t') {
			typestring++;
		}
	
		// Skip trailing whitespace
		char *end = typestring + strlen(typestring) - 1;
		while (end > typestring &&  (*end == '\t' || *end == ' ' || *end == '\n')) {
			*end-- = '\0';
		}
		
		// String
		if (typestring[0] == '\"') {
			type = 's';
		// Int or float
		} else if (isdigit(typestring[0])) {
			int n = 0;
			while (isdigit(typestring[n])) {
				n++;
			}
			// Float
			if (typestring[n] == 'f') {
				type = 'f';
			// Int
			} else {
				type = 'i';
			}
		// Boolean
		} else {
			char *truestring = "true";
			int p = 0;
			// True
			if (typestring[0] == 't') {
				while (typestring[p] != '\0' && p <3) {
					if (typestring[p] != truestring[p]) {
						e_parse(path, i, "invalid type '%s'\n", varname);
						return 1;
					}
					p++;
				}
				type = 'b';
			}
			char *falsestring = "false";
			// False
			p = 0;
			if (typestring[0] == 'f') {
				while (typestring[p] != '\0' && p < 4) {
					if (typestring[p] != falsestring[p]) {
						e_parse(path, i, "invalid type '%s'\n", varname);
						return 1;
					}
					p++;
				}
				type = 'b';
			}
			// If unknown
			if (type == ' ') {
				e_parse(path, i, "invalid type '%s'\n", varname);
			}
		}

		// Prepare varname for checking variable
		strtok(varname, " =");

		// 'a' is for any
		if (checkVar('a', varname) == false) {
			e_parse(path, i, "unknown variable '%s'\n", varname);
			return false;
		}

		char *typename;
		// Check if the value has the correct type
		if (checkVar(type, varname) == false) {
			// Set typename base on type char
			if (type == 's') {
				typename = "string";
			} else if (type == 'i') {
				typename = "integer";
			} else if (type == 'f') {
				typename = "float";
			} else {
				typename = "boolean";
			}

			e_parse(path, i, "incorrent type, %s expected\n", typename);
			return false;
		}
		i++;
	}
	
	fclose(file);

	d_print("file %s is a valid DST file\n", path);
	
	return true;
}

bool checkVar(char type, char *variable) {
	// Ignore empty things
	if (strcmp(variable, "\n") == 0) {
		return true;
	}

	// list of available variables
	char variables[8][64] = {
		// "<type> <name>"
		"i sample_rate",
		"i bit_depth",
		"b debug",
		"i max_buffer_size",
		"b amplitude_clip",
		"s engine_path",
		"s scene_path",
		"s output_path"
	};

	int i = 0;
	char *curvar;
	char curtype;

	while (i < 32) {
		curvar = variables[i];
		curvar += 2;
		curtype = variables[i][0];
		
		if (strcmp(variable, curvar) == 0) {
			if (type == curtype) {
				return true;
			} else if (type == 'a') {
				return true;
			}
			return false;
		}
		i++;
	}
	
	return false;
}
