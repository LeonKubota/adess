#include <stdbool.h>

#include "files/extract.h"
#include "main.h"

int guide(char **args);

int howToUse(void);

// Set by step
int overviewGuide(bool step);
int projectGuide(bool step);
int sceneGuide(bool step);
int engineGuide(bool step);
int renderGuide(bool step);

int syntaxGuide(void);
