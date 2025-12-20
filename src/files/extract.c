#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utils.h"
#include "files/check.h"
#include "files/extract.h"

char *parseLineValueS(char *variable, char *path) {
	char *string = parseLineValue(variable, path);

	// Check for empty strings
	if (string == NULL) {
		return NULL;
	}
	
	// Remove the first character if it's "
	if (string[0] == '\"') {
		string++;
	} else {
		e_parse(path, getVariableLineNumber(variable, path) + 1, "string expected\n");
		return NULL;
	}

	if ((string[strlen(string) - 1]) == '\"') {
		string[strlen(string) - 1] = '\0';
	} else {
		e_parse(path, getVariableLineNumber(variable, path) + 1, "string expected\n");
		return NULL;
	}
	
	return string;
}

int parseLineValueI(char *variable, char *path) {
	char *value = parseLineValue(variable, path);

	// atoi returns 0 if input isn't an integer string
	if (atoi(value) == 0) {
		// If it really is a zero
		if (value[0] == 0) {
			return INT_FAIL;
		} else {
			e_parse(path, getVariableLineNumber(variable, path) + 1, "integer expected\n");
			return INT_FAIL;
		}
	}

	return atoi(value);
}

float parseLineValueF(char *variable, char *path) {
	char *string = parseLineValue(variable, path);

	if (string == NULL) {
		return 0;
	}

	// Floats must have a '.'
	unsigned long i = 0;
	bool detected = false;
	while (i < strlen(string)) {
		if (string[i] == '.') {
			if (detected == true) {
				e_parse(path, getVariableLineNumber(variable, path) + 1, "float expected\n");
				return FLOAT_FAIL;
			}
			detected = true;
		}
		i++;
	}
	if (detected == false) {
		e_parse(path, getVariableLineNumber(variable, path) + 1, "float expected\n");
		return FLOAT_FAIL;
	}

	// Floats must end with 'f'
	if (string[strlen(string) - 1] != 'f') {
		e_parse(path, getVariableLineNumber(variable, path) + 1, "float expected\n");
		return FLOAT_FAIL;
	}

	return strtod(string, NULL);
}

bool parseLineValueB(char *variable, char *path) {
	char *string = parseLineValue(variable, path);

	if ((strcmp(string, "true") == 0) || (string[0] == '1' && string[1] == '\0')) {
		return true;
	} else if ((strcmp(string, "false") == 0) || (string[0] == '0' && string[1] == '\0')) {
		return false;
	} else {
		e_parse(path, getVariableLineNumber(variable, path) + 1, "boolean expected\n");
		return NULL;
	}
}

char *parseLineValue(char *variable, char *path) {
	FILE *file = fopen(path, "r");

	// If the file doesn't exist
	if (file == NULL) {
		e_fatal("file '%s' could not be read\n");
		return NULL;
	}

	char line[1024];
	char output[1024] = "";

	while (fgets(line, sizeof(line), file)) {
		// Skip the comments
		if (line[0] == '/' && line[1] == '/') {
			continue;
		}

		if (compare(line, variable) == true) {
			strncpy(output, line, sizeof(output) - 1);
			output[sizeof(output) - 1] = '\0';
		}
	}

	// Check if variable was found
	if (strcmp(output, "") == 0) {
		e_fatal("variable '%s' not found in '%s'\n", variable, path);
		return NULL;
	}

	fclose(file);

	char *value = strchr(output, '='); // Skip to "="
	if (value == NULL) {
		return NULL;
	}
	value++; // Skip "="
	//value = strtok(value, "//"); // before comment ("//")

	// Skip leading whitespace
	while (*value == ' ' || *value == '\t') {
		value++;
	}

	// Skip trailing whitespace
	char *end = value + strlen(value) - 1;
	while (end > value && (*end == '\t' || *end == ' ' || *end == '\n')) {
		*end-- = '\0';
	}

	// Delete comment
	char *curchar = value;
	if (strstr(value, "//") != NULL) {
		// WTF pointer things
		int i = 0;
		bool instring = false;
		while (curchar < (value + strlen(value) - 1)) {
			// Flip 'instring' if " detected
			if (curchar[0] == '\"') {
				instring = !instring;
			}
			
			if (instring == false && curchar[0] == '/' && curchar[1] == '/') {
				value[i - 1] = '\0';
			}
			i++;
			curchar++;
		}
	}

	return value;
}

// Compare the first word of string 1 to string 2
bool compare(char *char1, char *char2) {
	int i = 0;
	
	// While the first word isn't " "
	while (char1[i] != ' ') {
		if (char1[i] != char2[i]) {
			return false;
		}
		i++;
	}
	
	return true;
}

int getVariableLineNumber(char *variable, char *path) {
	FILE *file = fopen(path, "r");

	// If the file doesn't exist
	if (file == NULL) {
		e_fatal("file '%s' could not be read\n");
		return -1;
	}

	char line[1024];
	int i = 0;

	while (fgets(line, sizeof(line), file)) {
		if (compare(line, variable) == true) {
			fclose(file);
			return i;
		}
		i++;
	}

	fclose(file);
	return -1;
}

void loadKeyframes(char *scenePath, struct Keyframe *keyframes, int keyCount) {
	FILE *file = fopen(scenePath, "r"); // No need to check NULL
	char line[1024];
	char currentValue[1024];

	// TEMP
	if (false)
		printf("%f\n", keyframes[0].keytime);

	while (fgets(line, sizeof(line), file)) {
		strcpy(currentValue, line);
		strtok(currentValue, " =");
		if (strcmp(currentValue, "keyframes") == 0) {
			break;
		}
	}

	int offset = 0;

	// Find where keyframes start
	if (strchr(line, '{') == NULL) {
		while (fgets(line, sizeof(line), file)) {
			// Skip leading whitespace
			while (line[offset] == ' ' || line[offset] == '\t') {
				offset++;
			}

			// Break if found a '{'
			if (line[offset] == '{') {
				break;
			} else if (line[offset] == '\n') {
				continue;
			} else if (line[offset] == '/' && line[offset + 1] == '/') {
				continue;
			}
		}
	}

	int i = 0;
	char keytime[32];
	char rpm[32];
	char load[32];

	while (fgets(line, sizeof(line), file) && i < keyCount) {
		// Go over whitespace
		while (line[offset] == ' ' || line[offset] == '\t') {
			offset++;
		}

		// Get keytime (ends with 'f')
		strcpy(keytime, line);

		bool secondF = false;
		while (strlen(keytime) > 1) {
			if (keytime[strlen(keytime) - 1] == 'f') {
				if (secondF == true) {
					break;
				}
				secondF = true;
			}
			keytime[strlen(keytime) - 1] = '\0';
		}

		keyframes[i].keytime = strtod(keytime + offset, NULL);

		// Go to ','
		while (line[offset] != ',') {
			offset++;
		}

		// Skip it
		offset++;

		// Skip whitespace
		while (line[offset] == ' ' || line[offset] == '\t') {
			offset++;
		}

		// Get rpm (whole number)
		int n = 0;
		while (isDigit(line[offset]) == true) {
			rpm[n] = line[offset];
			offset++;
			n++;
		}

		keyframes[i].rpm = atoi(rpm);

		// Skip ','
		offset++;

		// Go over whitespace
		while (line[offset] == ' ' || line[offset] == '\t') {
			offset++;
		}

		// Get number (ends with 'f')
		strcpy(load, line);

		while (strlen(load) > 1) {
			if (load[strlen(load) - 1] == 'f') {
				break;
			}
			load[strlen(load) - 1] = '\0';
		}

		keyframes[i].load = strtod(load + offset, NULL);
		// Go over ';'
		offset = 0;
		i++;
	}
}
