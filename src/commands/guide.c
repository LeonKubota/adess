#include <string.h>
#include <stdio.h>

#include "commands/command.h"
#include "commands/guide.h"
//#include "utils.h"
#include "files/check.h"

int guide(char **args) {
    d_showInput("guide", args);

    // Show how to use the guide when no argument is passed
    if (args[2]) {
        // List
        if (strcmp(args[2], "1") == 0) overviewGuide(true);
        // 1
        // 2
        // 3
        // 4
        // 5
        // ...
        // Project
        // Scene
        // Render
    } else {
        howToUse();
    }

    return 0;
}

void howToUse(void) {
    printf("Adess guide:\n\n");
    printf("Use a number as an argument for the step by guide,\n");
    printf("use a topics name for info about that topic. See adess guide list for a list of topics.\n\n");

    printf("List of available guides:\n");
    printf("Step by step guide [1 - ]\n");
    //printf("\n");
}

void overviewGuide(bool step) {
    if (step) printf("Step 1 - overview of adess\n\n");
    
    printf("What is adess used for?\n");
    printf("Adess is used for generating engine sound for use in animation and film. You can export a WAVE file (.wav).\n");

    printf("\nYou use .adess files to controll the sound. There are three types:\n");

    printf("\tproject files\t- contain information about the project, such as export information and paths to other files.\n");
    printf("\tscene files\t- contains information about scenes (length and the used engine) and keyframes.\n");
    printf("\tengine files\t- contains information about engine parameters, both physical (cylinder count) and arbitrary (noise multiplier).\n");

    printf("\nYou can see more info about these files with 'adess guide <filename>'.\n");

    if (step) printf("\nUp next: step 2 - seting up a project\n");
}
void projectGuide(bool step) {
    if (step) printf("Step 1 - overview of adess\n\n");

    printf("The project files contain important information about the output files properties and the paths to other files. Here are the expected values:\n");
    printf("\tsample_rate\t- the sample rate of the resulting file, common values include 44100 and 96000.\n");
    printf("\tbit-depth\t- the bit depth of the resulting file, accepted values are 8, 16, 24 and 32.\n");
    printf("\tengine_path\t- path to the directory with engine files.\n");
    printf("\tscene_path\t- path to the directory with scenes.\n");
    printf("\toutput_path\t- path to the output directory.\n");
    printf("\tseed\t- seed used for random number generation.\n");

    if (step) printf("\nNext step: 2 - seting up a project\n");
}
void sceneGuide(bool step) {
    if (step) printf("Step 1 - overview of adess\n\n");
    if (step) printf("\nNext step: 2 - seting up a project\n");
}
void engineGuide(bool step) {
    if (step) printf("Step 1 - overview of adess\n\n");
    if (step) printf("\nNext step: 2 - seting up a project\n");
}
void renderGuide(bool step) {
    if (step) printf("Step 1 - overview of adess\n\n");
    if (step) printf("\nNext step: 2 - seting up a project\n");
}
