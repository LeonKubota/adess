#include <stdbool.h>

#ifndef MAIN_H
#define MAIN_H

// TODO - change to 1 later
#define VERSION "0.0.1"

#define MAX_ARG_COUNT 16 // This is based on the user
#define MAX_OPT_COUNT 8
#define MAX_VAL_COUNT 2

#define FLOAT_FAIL -69420.42069f // Hopefully no user enters 69420.42069f as their float
#define INT_FAIL -676967 // I hope noone uses this

#ifdef _WIN32
#include <windows.h>
#include <direct.h> // for _getcwd
#define PATH_SEPARATOR "\\" // Windows' weird things (idiot Bill Gates)
#define getcwd _getcwd // so that its the same later
#else
#include <unistd.h> // for gecwd
#define PATH_SEPARATOR "/" // for sane people
#endif

struct Scene {
	char engine[1024]; // The engine name
	float length; // Length of the scenes in seconds

	// Calculated
	char scenePath[1024];
	int keyframeCount;
};

struct Engine {
	// Physical parameters
	int stroke; // The amount of cycles per one work cycle
	int cylinderCount; // The amount of cylinders

	// Noise characteristics
	float baseNoise; // The base noise (with 0.0f load)
	float loadNoise; // Maximum noise (with 1.0f load)

	// Volume characteristics
	float baseVolume; // The base volume (with 0.0f load)
	float loadVolume; // Maximum volume (with 1.0f load)
	float rpmVolumeMultiplier; // What to multiply by with RPM
	float volumeVariation; // How much the volume varies
	float camshaftVolume; // The volume of the cam
};

struct Keyframe {
	float keytime; // Time of the keyframe
	float rpm; // The rpm at that time
	float load; // The load at that time
};


extern bool g_debug;
extern char *g_optslist;

extern int g_maxoptcount;
extern int g_maxvalcount;

extern bool g_opts[MAX_OPT_COUNT];
extern char *g_vals[MAX_OPT_COUNT][MAX_VAL_COUNT];

int main(int argc, char **argv);

#endif
