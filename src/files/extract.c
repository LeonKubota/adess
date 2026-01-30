#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <inttypes.h>

#include "utils.h"
#include "files/check.h"
#include "files/extract.h"

char *parseLineValueS(char *variable, char *path) {
	char *string = parseLineValue(variable, path);

	uint16_t offset = 0;

	// Check for empty strings
	if (string == NULL) {
		return NULL;
	}

	// Remove the first character if it's "
	if (string[0] == '\"') {
		string++;
		offset++;
	} else {
		e_parse(path, getVariableLineNumber(variable, path) + 1, "incorrect type, string expected\n");
		free(string);
		return NULL;
	}

    printf("%s\n", string);
    printf("%d\n", string[strlen(string) - 1]);
    printf("%d\n", 'A');

    // Skip trailing whitespace
    while ((string[strlen(string) - 1]) == ' ' || string[strlen(string) - 1] == '\t') {
        string[strlen(string) - 1] = '\0';
    }

    printf("%s\n", string);
    printf("%d\n", string[strlen(string) - 1]);
    printf("%d\n", 'A');

	if ((string[strlen(string) - 1]) == '\"') {
		string[strlen(string) - 1] = '\0';
	} else {
		e_parse(path, getVariableLineNumber(variable, path) + 1, "incorrect type, string expected\n");
		string -= offset;
		free(string);
		return NULL;
	}
    printf("here\n");

	char *output = (char *) malloc(1024 * sizeof(char));
	strcpy(output, string);

	// Free string
	string -= offset;
	free(string);

	return output;
}

int64_t parseLineValueI(char *variable, char *path) {
	char *value = parseLineValue(variable, path);

	if (value == NULL) {
		return INT_FAIL;
	}

	int64_t output = atoll(value);

	// atoll returns 0 if input isn't an integer string (64 bit int)
	if (output == 0) {
		// If it really is a zero
		if (value[0] == '0') {
			return 0;
		} else {
			e_parse(path, getVariableLineNumber(variable, path) + 1, "incorrect type, integer expected\n");
			return INT_FAIL;
		}
	}

	free(value);
	
	return output;
}

float parseLineValueF(char *variable, char *path) {
	char *string = parseLineValue(variable, path);

	if (string == NULL) {
		return FLOAT_FAIL;
	}

	// Prepare the stringNumber
	char stringNumber[16] = "";
	bool wasDot = false;
	int i = 0;


	while (string[i] != 'f') {
		if (isDigit(string[i]) == true) {
			stringNumber[i] = string[i];
		} else if (string[i] == '.') {
			if (wasDot) {
				e_parse(path, getVariableLineNumber(variable, path) + 1, "incorrect type, float expected\n");
				free(string);
				return FLOAT_FAIL;
			}
			wasDot = true;
			stringNumber[i] = string[i];
		} else {
			e_parse(path, getVariableLineNumber(variable, path) + 1, "incorrect type, float expected\n");
			free(string);
			return FLOAT_FAIL;
		}
		i++;
	}

	free(string);

	return atof(stringNumber);
}

bool parseLineValueB(char *variable, char *path) {
	char *string = parseLineValue(variable, path);

	if ((strcmp(string, "true") == 0) || (string[0] == '1' && string[1] == '\0')) {
		return true;
	} else if ((strcmp(string, "false") == 0) || (string[0] == '0' && string[1] == '\0')) {
		return false;
	} else {
		e_parse(path, getVariableLineNumber(variable, path) + 1, "incorrect type, boolean expected\n");
		return NULL;
	}

	free(string);
}

char *parseLineValue(char *variable, char *path) { // FIXME
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
			strcpy(output, line);
			output[sizeof(output) - 1] = '\0';
		}
	}

	// Check if variable was found
	if (strcmp(output, "") == 0) {
		e_fatal("variable '%s' not found in '%s'\n", variable, path);
		return NULL;
	}

	fclose(file);

	char *cursor = strchr(output, '='); // Skip to "="

	if (cursor == NULL) {
		return NULL;
	}

	cursor++; // Skip "="

	// Skip leading whitespace
	while (*cursor == ' ' || *cursor == '\t') {
		cursor++;
	}

	// Skip trailing whitespace
	char *end = cursor + strlen(cursor) - 1;
	while (end > cursor && (*end == '\t' || *end == ' ' || *end == '\n')) {
		*end-- = '\0';
	}

	// Delete comment
	char *curchar = cursor;
	if (strstr(cursor, "//") != NULL) {
		// WTF pointer things
		int i = 0;
		bool instring = false;
		while (curchar < (cursor + strlen(cursor) - 1)) {
			// Flip 'instring' if " detected
			if (curchar[0] == '\"') {
				instring = !instring;
			}
			
			if (instring == false && curchar[0] == '/' && curchar[1] == '/') {
				cursor[i - 1] = '\0';
			}
			i++;
			curchar++;
		}
	}

	char *value = (char *) malloc(1024 * sizeof(char));
	if (value) strcpy(value, cursor);

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

void loadKeyframes(struct Scene *scene, struct Keyframe *keyframes) {
	FILE *file = fopen(scene->scenePath, "r"); // No need to check NULL
	char line[1024];
	char currentValue[1024];

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
	char keytime[1024];
	char rpm[1024];
	char load[1024];
	int n = 0;

	while (fgets(line, sizeof(line), file) && i < scene->keyframeCount) {
		// Go over whitespace
		while (line[offset] == ' ' || line[offset] == '\t') {
			offset++;
		}

		// Ignore comments
		if (line[offset] == '/' && line[offset + 1] == '/') {
			offset = 0;
			continue;
		}

		// Ignore empty lines
		if (line[offset] == '\0' || line[offset] == '\n') {
			offset = 0;
			continue;
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
		n = 0;
		memset(rpm, 0, 32); // Reset the rpm string
		while (isDigit(line[offset]) == true) {
			rpm[n] = line[offset];
			n++;
			offset++;
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

	fclose(file);
}
