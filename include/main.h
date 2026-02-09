#include <stdbool.h>
#include <inttypes.h>

#ifndef MAIN_H
#define MAIN_H

#define VERSION "1.0.0"
#define SLOW 0

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

// Make some constants (from my handy 'CASIO fx-85CE X CLASSWIZ')
#define PI 3.141592654f
#define TAU 6

struct Project {
	int sampleRate;
	int bitDepth;

	char *enginePath;
	char *scenePath;
	char *outputPath;

	int seed;
};

struct Scene {
	char *engine; // The engine name
	float length; // Length of the scenes in seconds

	// Calculated
	char *scenePath;
	uint64_t sampleCount;
	int keyframeCount;
};

struct Engine {
	// Physical parameters
	int stroke; // The amount of cycles per one work cycle
	int cylinderCount; // The amount of cylinders
	int idleRpm; // The idle rpm of the engine
	int maxRpm; // The maximum rpm of the engine
	float valvetrainTimingOffset; // The offset between the intake and exhaust valve time

	// Harmonics
	int harmonics; // How many harmonics the engine should have

	// Noise characteristics
	float baseNoise; // The base noise (with 0.0f load)
	float loadNoise; // Maximum noise (with 1.0f load)

	float lowFrequencyNoiseFrequency;
	float lowFrequencyNoiseStrength;
	float lowFrequencyNoiseFalloff;

	// Volume characteristics
	float baseVolume; // The base volume
	float valvetrainVolume; // The volume of the valvetrain

	float minimumVolume; // The minimum volume
	float loadVolumeMultiplier; // Maximum volume (with 1.0f load)
	float rpmVolumeMultiplier; // What to multiply by with RPM

	float minimumNoise;
	float loadNoiseMultiplier;
};

struct Keyframe {
	float keytime; // Time of the keyframe
	float rpm; // The rpm at that time
	float load; // The load at that time
};

// This struct is used for sending data to pthreads
struct ThreadData {
	// Buffers I can use
	float *buffer0;
	float *buffer1;
	float *buffer2;
	float *buffer3;
	float *buffer4;
	float *buffer5;
	float *buffer6;

	// Structs used for calculation
	struct Project *project;
	struct Scene *scene;
	struct Engine *engine;
	struct Keyframe *keyframes;

	// Failed
	bool failed;
};

extern bool g_debug;
extern char *g_optslist;

extern int g_maxoptcount;
extern int g_maxvalcount;

extern bool g_opts[MAX_OPT_COUNT];
extern char *g_vals[MAX_OPT_COUNT][MAX_VAL_COUNT];

int main(int argc, char **argv);

#endif
