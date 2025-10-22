// import analysis.*;
// import commands.*;
import config.*;
// import data.*;
// import generation.*;
// import rendering.*;

class Main {
    public static void main(String[] args) {
        // time (for performance measuring)
        long startTime = System.nanoTime();

        CommandParser cp = new CommandParser();
        cp.parse(args).execute();

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
