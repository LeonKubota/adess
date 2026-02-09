#include <stdio.h>
#include <stdbool.h>
#include <dirent.h> // Probably OK on Linux/MacOS, need to test for W*ndows
#include <string.h>
#include <stdlib.h>

#include "utils.h"
#include "files/check.h"

bool checkFileExistsIn(char *path, const char *filename) {
	struct dirent *entry;
	DIR *dir = opendir(path);

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
		fclose(file);
		return true;
	}

	return false;
}

char *findProjectFile(char *path) {
	// If there is no file ending in '.adess'
	if (!checkFileExistsIn(path, "*.adess")) {
		e_fatal("project file not found in '%s'\n", path);
		free(path);
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
				closedir(dir);
				free(path);
				return NULL;
			}
		}
	}

	char *outputBuffer = (char *) malloc(1024 * sizeof(char));

	strcat(path, projectFilePath);

	strcpy(outputBuffer, path);
	free(path);

	closedir(dir); // Free it later, because it's still used in 'outputBuffer'

	return outputBuffer;
}

char *getCurDirectory(char *userpath) {
	char *currentWorkingDirectory = (char *) malloc(1024 * sizeof(char));
	char *outputDirectory = (char *) malloc(4096 * sizeof(char));

	// Check if allocated
	if (currentWorkingDirectory == NULL) return NULL;
	if (outputDirectory == NULL) return NULL;

	// Init
	strcpy(currentWorkingDirectory, "");
	strcpy(outputDirectory, "");
	
	if (getcwd(currentWorkingDirectory, 1024) != NULL) {
		if (userpath == NULL) {
			strcat(outputDirectory, currentWorkingDirectory);

			free(currentWorkingDirectory);

			strcat(outputDirectory, "/");

			return outputDirectory;
		} else {
			strcat(outputDirectory, currentWorkingDirectory);

			free(currentWorkingDirectory);

			strcat(outputDirectory, "/");
			strcat(outputDirectory, userpath);
			strcat(outputDirectory, "/");

			return outputDirectory;
		}
	}

	free(currentWorkingDirectory);

	e_fatal("directory not found\n");

	return NULL;
}

// 'true' if valid, 'false' if invalid
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
    uint32_t i = 0;
    uint32_t comOff = 0;
	
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

		strcpy(varname, line);

		if (varname[0] == '\n') {
			i++;
			continue;
		}

		// Get the type of the value
		typestring = strchr(varname, '=');
		if (typestring == NULL) {
			e_parse(path, i + 1, "variables have to be defined with '='\n");
			return false;
		}
		typestring++;

		// Skip leading whitespace
		while (typestring[0] == ' ' || typestring[0] == '\t') {
			typestring++;
		}

        // check for comment
        while (comOff < strlen(typestring)) {
            if (typestring[comOff] == '/' && typestring[comOff + 1] == '/') {
                typestring[comOff] = '\0';
                break;
            }
            comOff++;
        } comOff = 0;

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
			// Get the last char that isn't a comment
			while (typestring[n] != '\0') {
				if (typestring[n + 1] == '/' && typestring[n + 2] == '/') {
					break;
				}
				n++;
			}
			n--;
			// Float
			if (typestring[n] == 'f') {
				// Check if it really is a float
				type = 'f';
			} else if (!isDigit(typestring[n])) { // Check if it's a validatable int
				type = 'u';
			} else { // Int
				type = 'i';
			}
		// Boolean
		} else if (strcmp(typestring, "true") == 0 || strcmp(typestring, "false") == 0) {
			type = 'b';
		// Keyframe
		} else {
			strtok(varname, " =");
			if (strcmp(varname, "keyframes") == 0) {
				if (isKeyframeValid(path) == false) {
					return false;
				}
				type = 'k';
				break;
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
			    return false;
			}

			e_parse(path, i + 1, "incorrect type, %s expected\n", typename);

			return false;
		}

		i++;
	}

	fclose(file);
	
	return true;
}

// Return 'g' if success, otherwise return reqired type
char checkVar(char type, char *variable) {
	// Ignore empty things
	if (strcmp(variable, "\n") == 0) {
		return 'g';
	}

	// list of available variables (keyframes and curve type is not included because they are special)
	char variables[32][64] = { // Increase the first value
		// "<type> <name>"
		// General
		"i sample_rate",
		"i bit_depth",
		"s engine_path",
		"s scene_path",
		"s output_path",
		"i seed",

		// Scene
		"s engine",
		"f length",
		"k keyframes",

		// Engine
		"i stroke",
		"i cylinder_count",
		"i idle_rpm",
		"i max_rpm",
		"f valvetrain_timing_offset",

		"i harmonics",

		"f brown_noise_strength",

		"f low_frequency_noise_frequency",
		"i low_frequency_noise_falloff",
		"f low_frequency_noise_strength",

		"f base_volume",
		"f valvetrain_volume",

		"f minimum_volume",
		"f load_volume_multiplier",
		"f rpm_volume_multiplier",

		"f minimum_noise",
		"f load_noise_multiplier",
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
	char digits[11] = "0123456789";
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
	FILE *fileKeys = fopen(scenePath, "r"); // No null check, should be fine (it's checked before)
	char lineKeys[1024] = "";
	char currentValue[1024];
	bool found = false;

	while (fgets(lineKeys, sizeof(lineKeys), fileKeys)) {
		strcpy(currentValue, lineKeys);
		strtok(currentValue, " =");
		if (strcmp(currentValue, "keyframes") == 0) {
			found = true;
			break;
		}
	}

	if (found == false) {
		e_parse(scenePath, 0, "'keyframes' not found\n");
		fclose(fileKeys);
		return -1;
	}

	int count = 0;

	// Count ';' until '}'
	while (fgets(lineKeys, sizeof(lineKeys), fileKeys)) {
		// Ignore comments
		if (strchr(lineKeys, '/') != NULL) {
			// Skip whitespace
			int offset = 0;
			while (lineKeys[offset] == ' ' || lineKeys[offset] == '\t') {
				offset++;
			}

			// Check if comment
			if (lineKeys[offset] == '/' && lineKeys[offset] == '/') {
				continue;
			}
		}
		if (strchr(lineKeys, ';') != NULL) {
			count++;
		}
		if (strchr(lineKeys, '{') != NULL) {
			break;
		}
	}

	fclose(fileKeys);

	return count;
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
		fclose(fileKey);
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
				fclose(fileKey);
				e_parse(scenePath, n + 1, "keyframe lists must begin with '{'\n");
				return false;
			}
		}
	}

	// Test if there is anything after '{'
	while (lineKey[offset] != '{') {
		offset++;
	}

	offset++;

	while (lineKey[offset] == ' ' || lineKey[offset] == '\t') {
		offset++;
	}

	if (lineKey[offset] != '\n' && lineKey[offset] != '\0') {
		if (lineKey[offset] != '/' || lineKey[offset + 1] != '/') {
			e_parse(scenePath, n + 1, "unexpected value\n");
			fclose(fileKey);
			return false;
		}
	}

	offset = 0;

	// Read the keyframes
	// Expect this: n numbers, dot, n numbers, 'f', ',', n numbers, ',', n numbers, dot, n numbers, 'f', ';'
	while (fgets(lineKey, sizeof(lineKey), fileKey)) {
		n++;
		// Go over the whitespace
		while (lineKey[offset] == ' ' || lineKey[offset] == '\t') {
			offset++;
		}

		// End loop if '}'
		if (lineKey[offset] == '}') {
			break;
		}

		// Lines don't have to have anything
		if (lineKey[offset] == '\n' || lineKey[offset] == '\0') {
			offset = 0;
			continue;
		}

		// Ignore comments
		if (lineKey[offset] == '/' && lineKey[offset + 1] == '/') {
			offset = 0;
			continue;
		}

		// Go over the numbers
		while (isDigit(lineKey[offset]) == true) {
			offset++;
		}

		// The dot
		if (lineKey[offset] == '.') {
			offset++;
		} else {
			e_parse(scenePath, n + 1, "incorrect type, float expected\n");
			fclose(fileKey);
			return false;
		}

		// Go over the numbers after decimal
		while (isDigit(lineKey[offset]) == true) {
			offset++;
		}

		// The f
		if (lineKey[offset] == 'f') {
			offset++;
		} else {
			e_parse(scenePath, n + 1, "incorrect type, float expected\n");
			fclose(fileKey);
			return false;
		}

		// Go over the whitespace
		while (lineKey[offset] == ' ' || lineKey[offset] == '\t') {
			offset++;
		}

		if (lineKey[offset] == ',') {
			offset++;
		} else {
			e_parse(scenePath, n + 1, "incorrect value separator\n");
			fclose(fileKey);
			return false;
		}

		// Go over the whitespace
		while (lineKey[offset] == ' ' || lineKey[offset] == '\t') {
			offset++;
		}

		// Normal number
		while (isDigit(lineKey[offset]) == true) {
			offset++;
		}

		// Go over the whitespace
		while (lineKey[offset] == ' ' || lineKey[offset] == '\t') {
			offset++;
		}

		if (lineKey[offset] == ',') {
			offset++;
		} else {
			e_parse(scenePath, n + 1, "incorrect value separator\n");
			fclose(fileKey);
			return false;
		}

		// Go over the whitespace
		while (lineKey[offset] == ' ' || lineKey[offset] == '\t') {
			offset++;
		}

		// Check final number (float)
		while (isDigit(lineKey[offset]) == true) {
			offset++;
		}

		if (lineKey[offset] == '.') {
			offset++;
		} else {
			e_parse(scenePath, n + 1, "incorrect type, float expected\n");
			fclose(fileKey);
			return false;
		}

		// Final numbers
		while (isDigit(lineKey[offset]) == true) {
			offset++;
		}

		// Check for 'f'
		if (lineKey[offset] == 'f') {
			offset++;
		} else {
			e_parse(scenePath, n + 1, "incorrect type, float expected\n");
			fclose(fileKey);
			return false;
		}

		// Go over the whitespace
		while (lineKey[offset] == ' ' || lineKey[offset] == '\t') {
			offset++;
		}

		if (lineKey[offset] == ';') {
			offset++;
		} else {
			e_parse(scenePath, n + 1, "incorrect keyframe terminator\n");
			fclose(fileKey);
			return false;
		}

		// Go over the whitespace
		while (lineKey[offset] == ' ' || lineKey[offset] == '\t') {
			offset++;
		}

		if (lineKey[offset] != '\n' && lineKey[offset] != '\0') {
			// If comment
			if (lineKey[offset] != '/' || lineKey[offset + 1] != '/') {
				e_parse(scenePath, n + 1, "incorrect keyframe terminator\n");
				fclose(fileKey);
				return false;
			}
		}
		offset = 0;
	}
	
	fclose(fileKey);

	return true;
}
