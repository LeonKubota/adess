#include <stdio.h>
#include <stdbool.h>
#include <dirent.h> // Probably OK on Linux/MacOS, need to test for W*ndows
#include <string.h>
#include <stdlib.h>

#include "utils.h"
#include "files/check.h"

bool checkFileExistsIn(char *path, const char *filename) {
	struct dirent *entry;
	DIR *dir = opendir(path); // WTF this erases char *filename some of the time (when it doesn't exist???

	if (dir == NULL) {
		e_fatal("directory '%s' could not be read\n", path);
		return false;
	}
	
	// Handle non-exact search
	if (filename[0] == '*') {
		filename++;
		while ((entry = readdir(dir)) != NULL) {
			if (strstr(entry -> d_name, filename)) {
				closedir(dir);
				return true;
			}
		}
		return false;
	}

	while ((entry = readdir(dir)) != NULL) {
		if (strcmp(entry -> d_name, filename) == 0) {
			closedir(dir);
			return true;
		}
	}

	closedir(dir);
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
	if (!checkFileExistsIn(path, "*.adess")) {
		e_fatal("project file not found in '%s'\n", path);
		return NULL;
	}

	struct dirent *entry;
	DIR *dir = opendir(path);
	char *projectFilePath = NULL;
	
	while ((entry = readdir(dir)) != NULL) {

		if (strstr(entry -> d_name, ".adess")) {
			// Ignore hidden files
			if (entry -> d_name[0] == '.') {
				continue;
			}

			if (projectFilePath == NULL) {
				projectFilePath = entry -> d_name;  
			} else {
				e_fatal("multiple project files found in '%s'\n", path); 
				return NULL;
			}
		}
	}

	return strcat(path, projectFilePath);
}

char *getCurDirectory(char *userpath) {
	char cwd[4096];
	
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
	char type = ' '; // a = any; s = string; i = int; f = float; b = boolean; 
	char *typestring;
	int i = 0;
	
	while (fgets(line, sizeof(line), file)) {

		// Reset 'type'
		type = ' ';

		// Skip comments
		if (line[0] == '/' && line[1] == '/') {
			i++;
			continue;
		}

		// Skip empty
		if (line[0] == '\n') {
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
		
		// Picking 'type'
		// String
		if (typestring[0] == '\"') {
			type = 's';
		// Int or float
		} else if (isDigit(typestring[0]) == true) {
			int n = 0;
			// Get the last char
			while (typestring[n] != '\0') {
				n++;
			}
			n--;
			// Float
			if (typestring[n] == 'f') {
				type = 'f';
			}
			// Check if it's a validatable int
		   	else if (!isDigit(typestring[n - 1])) {
				type = 'u';
			}
			// Int
		   	else {
				type = 'i';
			}
		// Boolean
		} else if (strcmp(typestring, "true") == 0 || strcmp(typestring, "false") == 0) {
			type = 'b';
		// Keyframe or curve
		} else {
			strtok(varname, " =");
			if (strcmp(varname, "keyframes") == 0) {
				if (isKeyframeValid(path) == false) {
					return 1;
				}
			} else {
				printf("its a curve");
				return 1;
			}
		}

		// Prepare varname for checking variable
		strtok(varname, " =");

		// 'a' is for any
		if (checkVar('a', varname) == false) {
			e_parse(path, i + 1, "unknown variable '%s'\n", varname);
			return false;
		}

		char *typename;
		// Check if the value has the correct type
		char requiredType = checkVar(type, varname);
		if (requiredType != 'g') {
			// Set typename base on type char
			if (requiredType == 's') {
				typename = "string";
			} else if (requiredType == 'i') {
				typename = "integer";
			} else if (requiredType == 'f') {
				typename = "float";
			} else if (requiredType == 'b') {
				typename = "boolean";
			} else if (requiredType == 'k') {
				typename = "keyframe";
			} else {
				typename = NULL;
				printf("Something is horribly broken (this is not your fault)\n");
			}

			e_parse(path, i + 1, "incorrent type, %s expected\n", typename);
			return false;
		}

		i++;
	}
	
	fclose(file);

	d_print("file %s is a valid DST file\n", path);
	
	return true;
}

// Return 'g' if success, otherwise return reqired type
char checkVar(char type, char *variable) {
	// Ignore empty things
	if (strcmp(variable, "\n") == 0) {
		return 'g';
	}

	// list of available variables (keyframes and curve type is not included because they are special)
	char variables[16][64] = {
		// "<type> <name>"
		"i sample_rate",
		"i bit_depth",
		"b debug",
		"i max_buffer_size",
		"b amplitude_clip",
		"s engine_path",
		"s scene_path",
		"s output_path",
		"s engine",
		"f length",
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
				return 'g';
			} else if (type == 'a') {
				return 'g';
			}
			return curtype;
		}
		i++;
	}
	
	return curtype;
}

bool isDigit(char input) {
	char digits[11] = "0123456789\0";
	int i = 0;
	while (digits[i] != '\0') {
		if (digits[i] == input) {
			return true;
		}
		i++;
	}
	return false;
}

int countKeyframes(char *scenePath) {
	printf("%s\n", scenePath);
	return -1;
}

// Keyframes can be: valid or invalid (unended, empty, wrong data (type, ',' or ';', ect.))
bool isKeyframeValid(char *scenePath) {
	// Setup file read
	FILE *fileKey = fopen(scenePath, "r"); // no error handling because we already know it exists
	char lineKey[1024];
	char currentValue[1024];
	int n = 0;
	bool found = false;

	// Set n to line with keyframe
	while (fgets(lineKey, sizeof(lineKey), fileKey)) {
		strcpy(currentValue, lineKey);
		strtok(currentValue, " =");
		if (strcmp(currentValue, "keyframes") == 0) {
			found = true;
			break;
		}
		n++;
	}

	if (found == false) {
		return false;
	}

	int offset = 0;

	// Ensure that '{' is somewhere immidietly after 'keyframes'
	if (strchr(lineKey, '{') == NULL) {
		while (fgets(lineKey, sizeof(lineKey), fileKey)) {
			n++;
			// Skip leading whitespace
			while (lineKey[offset] == ' ' || lineKey[offset] == '\t') {
				offset++;
			}

			// Break if found a '{'
			if (lineKey[offset] == '{') {
				break;
			} else if (lineKey[offset] == '\n') {
				continue;
			} else if (lineKey[offset] == '/' && lineKey[offset + 1] == '/') {
				continue;
			} else {
				e_parse(scenePath, n + 1, "keyframe lists must begin with '{'\n");
				return false;
			}
		}
	}

	bool wasDot = false;

	// Read the keyframes
	while (fgets(lineKey, sizeof(lineKey), fileKey)) {
		n++;
		offset = 0;
		// Skip leading whitespace
		while (lineKey[offset] == ' ' || lineKey[offset] == '\t') {
			offset++;
		}

		printf("%s\n", lineKey + offset);

		// Check the first part (float keytime)
		wasDot = false;
		while (lineKey[offset + 1] != ',' || lineKey[offset] != '\n' || lineKey[offset] != '\0') {
			if (isDigit(lineKey[offset]) == true) {
				printf("d");
				offset++;
				continue;
			} else if (lineKey[offset] == '.') {
				printf(".");
				if (wasDot == true) {
					e_parse(scenePath, n + 1, "incorrect type, float expected\n");
					return false;
				}
				wasDot = true;
				offset++;
				continue;
			} else if (lineKey[offset] == 'f') {
				printf("f");
				offset++;
				break;
			} else {
				e_parse(scenePath, n + 1, "incorrect type, float expected\n");
				return false;
			}
			offset++;
		}

		// Skip some more whitespace
		while (lineKey[offset] == ' ' || lineKey[offset] == '\t') {
			offset++;
		}

		printf("after: %s\n", lineKey + offset);
		
		if (lineKey[offset] != ',') {
			e_parse(scenePath, n + 1, "incorrect values divider\n");
			return false;
		}

		printf("\n");

		// Do the second value (int rpm)
		break;
	}

	printf("first is correct\n");

	return true;
}
