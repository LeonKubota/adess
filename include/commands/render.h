#include "files/extract.h"
#include "files/check.h"
#include "main.h"

#ifndef RENDER_H
#define RENDER_H

int render(char **args);

int renderAll(char *projectFilePath);
int renderScene(char **sceneName, char *projectFilePath);

#endif
