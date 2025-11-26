#include <stdio.h>

int appendLine(char *path, char *input) {
	FILE *file;
	
	file = fopen(path, "a"); // 'a' is for append

	fprintf(file, "%s", input);

	fclose(file);
	return 0;
}
