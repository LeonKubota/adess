// import analysis.*;
import commands.*;
import config.*;
// import data.*;
// import generation.*;
// import rendering.*;

class Main {
    public static void main(String[] args) {
        // time (for performance measuring)
        long startTime = System.nanoTime();

	// Turn on for debugging
	final boolean DEBUG = false;

	// If no command given
	if (args.length == 0) {
		new HelpCommand();
		System.exit(0);
	}

	if (DEBUG) {
		for (int i = 0; i < args.length; i++) {
			System.out.print("[DEBUG]\t");
			System.out.println("args[" + i + "]: " + args[i]);
		}
	}

	// Parse first command (action)
	switch (args[0]) {
		case "mk":
		case "make":
			if (DEBUG) {System.out.println("[DEBUG]\tMakeCommand");}
			if (args.length > 1) {
				switch (args[1]) {
					case "project":
						if (DEBUG) {System.out.println("[DEBUG]\tMakeCommand project");}
						if (args.length > 2) {
							new MakeProjectCommand(args[2]);
						} else {
							new MakeProjectCommand();
						}
						break;
					case "engine":
						if (DEBUG) {System.out.println("[DEBUG]\tMakeCommand engine");}
						if (args.length > 2) {
							// new MakeEngineCommand(args[2]);
						} else {
							// new MakeEngineCommand();
						}
						break;
					case "scene":
						if (DEBUG) {System.out.println("[DEBUG]\tMakeCommand scene");}
						if (args.length > 2) {
							// new MakeSceneCommand(args[2]);
						} else {
							// new MakeSceneCommand();
						}
						break;
					case "key":
						if (DEBUG) {System.out.println("[DEBUG]\tMakeCommand key");}
						if (args.length > 2) {
							// new MakeKeyCommand(args[2]);
						} else {
							// new MakeKeyCommand();
						}
						break;
					default:
						if (DEBUG) {System.out.println("[DEBUG]\tMakeCommand invalid component");}
						System.err.println("fatal: '" + args[1] + "' is not a valid component");
				}
			} else {
				if (DEBUG) {System.out.println("[DEBUG]\tMakeCommand no component");}
				System.out.println("fatal: no component specified");
				// new HelpCommand(args[0]);
			}
			break;
		case "ed":
		case "edit":
			if (DEBUG) {System.out.println("[DEBUG]\tEditCommand");}
			break;

		case "sh":
		case "show":
			if (DEBUG) {System.out.println("[DEBUG]\tShowCommand");}
			break;

		case "rm":
		case "remove":
			if (DEBUG) {System.out.println("[DEBUG]\tRemoveCommand");}
			break;

		case "-h":
		case "--help":
			if (DEBUG) {System.out.println("[DEBUG]\tHelpCommand");}
			new HelpCommand();
			break;

		case "help":
			if (DEBUG) {System.out.println("[DEBUG]\tHelpCommand");}
			new HelpCommand();
			break;
		default:
			new UnknownCommand(args[0]);
	}


        /*
        // These will later be read from some file
        Frequencies fq = new Frequencies();
        Render rd = new Render();
        Preview pr = new Preview();
        Export ep = new Export();
        WaveProcessing wp = new WaveProcessing();

        float sampleRate = 44100.0f;
        int length = (int) (sampleRate * 10);

        float[] frequencies = fq.testFrequency(length, 70.0f, 70.0f); 
        // frequencies = wp.addVibration(sampleRate, frequencies, 0.1f, 0.7f);
        // frequencies = wp.addNoise(frequencies, 0.01f);
        System.out.println("Frequencies generation complete");

        // Make the wave
        float[] sine = rd.generateSine(sampleRate, frequencies, 1.0f, 1);
        System.out.println("Sine generation complete");

        sine = wp.combineWaves(sine, rd.generateSine(sampleRate, frequencies, 0.5f, 2));
        sine = wp.combineWaves(sine, rd.generateSine(sampleRate, frequencies, 0.3f, 3));
        sine = wp.combineWaves(sine, rd.generateSine(sampleRate, frequencies, 0.25f, 4));

        sine = wp.normalize(sine);

        // Protect user and hardware from extreme amplitudes
        if (!wp.isNormalized(sine)) {
            System.out.println("Wave is not normalized, aborting...");
            System.exit(0);
        }

        // Save the wave
        ep.exportWaveform(sampleRate, sine, "sineWave.wav");

        long endTime = System.nanoTime();
        System.out.println("Execution time: " + ((endTime - startTime) / 1_000_000) + " ms");
        // Play the wave 
        pr.playAudio(sampleRate, sine);
        System.out.println("Audio playback complete");

        // Dump wave to text file
        ep.dumpInText(sine, "test.txt");
        */
    }
}
