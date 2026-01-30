#include <stdbool.h>

#include "files/extract.h"
#include "main.h"

int guide(char **args);

void howToUse(void);

// Set by step
void overviewGuide(bool step);
void projectGuide(bool step);
void sceneGuide(bool step);
void engineGuide(bool step);
void renderGuide(bool step);
