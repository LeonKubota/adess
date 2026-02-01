#include <string.h>
#include <stdio.h>

#include "commands/command.h"
#include "commands/guide.h"
//#include "utils.h"
#include "files/check.h"

int guide(char **args) {
    d_showInput("guide", args);

    if (args[2]) {
        // Step by step
        if (strcmp(args[2], "1") == 0) return overviewGuide(true);
        if (strcmp(args[2], "2") == 0) return projectGuide(true);
        if (strcmp(args[2], "3") == 0) return sceneGuide(true);
        if (strcmp(args[2], "4") == 0) return engineGuide(true);
        if (strcmp(args[2], "5") == 0) return renderGuide(true);

        // Show guide by topic
        if (strcmp(args[2], "overview") == 0) return overviewGuide(false);
        if (strcmp(args[2], "project") == 0) return projectGuide(false);
        if (strcmp(args[2], "scene") == 0) return sceneGuide(false);
        if (strcmp(args[2], "engine") == 0) return engineGuide(false);
        if (strcmp(args[2], "render") == 0) return renderGuide(false);
        if (strcmp(args[2], "syntax") == 0) return syntaxGuide();

        // Unexpected
        printf("there is no guide called '%s'\n", args[2]);
        howToUse();
    } else { // If no user input
        howToUse();
    }

    return 0;
}

int howToUse(void) {
    printf("Adess guide:\n\n");
    printf("Use a number as an argument for the step by guide,\n");
    printf("use a topics name for info about that topic. See adess guide list for a list of topics.\n\n");

    printf("List of available guides:\n");
    printf("\tStep by step guide [1 - 5]\n");
    printf("\n\toverview\n");
    printf("\tproject\n");
    printf("\tscene\n");
    printf("\tengine\n");
    printf("\trender\n");
    printf("\tsyntax\n");

    return 0;
}

int overviewGuide(bool step) {
    if (step) printf("Step 1 - overview of adess\n\n");
    
    printf("What is adess used for?\n");
    printf("Adess is used for generating engine sound for use in animation and film. You can export a WAVE file (.wav).\n");

    printf("\nYou use .adess files to controll the sound. There are three types:\n");

    printf("\tproject files\t\t\t- contain information about the project, such as export\n\t\t\t\t\t  information and paths to other files.\n");
    printf("\tscene files\t\t\t- contains information about scenes (length and the used engine)\n\t\t\t\t\t  and keyframes.\n");
    printf("\tengine files\t\t\t- contains information about engine parameters, both physical\n\t\t\t\t\t  (cylinder count) and arbitrary (noise multiplier).\n");

    printf("\nYou can see more info about these files with 'adess guide <filename>'.\n");

    if (step) printf("\nUp next: step 2 - projects\n");

    return 0;
}

int projectGuide(bool step) {
    if (step) printf("Step 2 - projects\n\n");

    printf("The project files contain important information about the output files properties and the paths to other files. Here are the expected values:\n");
    printf("\tsample_rate\t\t\t- the sample rate of the resulting file, common values include\n\t\t\t\t\t  44100 and 96000.\n");
    printf("\tbit-depth\t\t\t- the bit depth of the resulting file, accepted values are\n\t\t\t\t\t  8, 16, 24 and 32.\n");
    printf("\tengine_path\t\t\t- path to the directory with engine files.\n");
    printf("\tscene_path\t\t\t- path to the directory with scenes.\n");
    printf("\toutput_path\t\t\t- path to the output directory.\n");
    printf("\tseed\t\t\t\t- seed used for random number generation.\n");

    if (step) printf("\nNext step: 3 - scenes\n");

    return 0;
}

int sceneGuide(bool step) {
    if (step) printf("Step 3 - scenes\n\n");

    printf("The scene files contain important information about the scene as well as a sequence of keyframes containing information about time, rpm and load. Here are the expected values:\n");
    printf("\tlength\t\t\t\t- the length of the scene, in seconds.\n");
    printf("\tengine\t\t\t\t- the the name of the used engine.\n");
    printf("\tkeyframes\t\t\t- list of keyframes.\n");

    printf("\nThe keyframes list is formatted as follows:\n");
    printf("\t0.0f, 1000, 0.5f;");
    printf("\tThis sets a keyframe at 0.0 seconds at 1000 rmp and 0.5 load.\n");

    if (step) printf("\nNext step: 4 - engines\n");

    return 0;
}

int engineGuide(bool step) {
    if (step) printf("Step 4 - engines\n\n");

    printf("The engine files contain important variables about engines. Here are the expected values:\n");
    printf("\tstroke\t\t\t\t- the stroke of an engine (usually 4 or 2).\n");
    printf("\tcylinder_count\t\t\t- the number of cylinders.\n");
    printf("\tidle_rpm\t\t\t- the idle rpm of the engine.\n");
    printf("\tmax_rpm\t\t\t\t- the maximum rpm of the engine.\n");
    printf("\tvalvetrain_timing_offset\t- offset of the exhaust valve compared to the intake valve.\n");

    printf("\tlow_frequency_noise_frequency\t- frequency of low frequency noise, used for idle sound.\n");
    printf("\tlow_frequency_noise_falloff\t- falloff of low frequency noise,\n\t\t\t\t\t  it's strongest at idle rpm and is zero at idle rpm + falloff value.\n");
    printf("\tlow_frequency_noise_strength\t- strength of low frequency noise.\n");

    printf("\tharmonics\t\t\t- how many harmonic frequencies the engine has. This setting is\n\t\t\t\t\t  very expensive, values above 10 are strongly discouraged.\n");

    printf("\tbase_volume\t\t\t- the volume of the base sound.\n");
    printf("\tvalvetrain_volume\t\t- the volume of the valvetrain sound, very low values are suggested.\n");

    printf("\tminimum_volume\t\t\t- minimum volume (at idle rpm with no load).\n");
    printf("\trpm_volume_multiplier\t\t- volume multiplication factor for rpm.\n");
    printf("\tload_volume_multiplier\t\t- volume multiplication factor for load.\n");

    printf("\tminimum_noise\t\t\t- minimum noise (at ide woth no load).\n");
    printf("\tload_noise_multipler\t\t- noise multiplication factor for load.\n");

    if (step) printf("\nNext step: 5 - rendering\n");

    return 0;
}

int renderGuide(bool step) {
    if (step) printf("Step 5 - rendering\n\n");

    printf("After setting up your project, scene and engine, you can render your project.\n");
    printf("Use: adess render <scene-name> to render, you can use -p for preview -n to specify the outputs name and -a to render all.\n");
    printf("Now you can find the output in the output directory!\n");

    return 0;
}

int syntaxGuide(void) {
    return 0;
}
