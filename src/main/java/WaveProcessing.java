import java.util.Random;

public class WaveProcessing {

    // Tau (useful for calculations)
    public static final float TAU = 2.0f * (float) Math.PI;

    // Add vibration to frequencies
    public float[] addVibration(float sampleRate, float[] wave, float strength, float freqSpeedMult) {
        float phase;

        for (int i = 0; i < wave.length; i++) {
            float frequency = wave[i] * freqSpeedMult;
            phase = (TAU * frequency * i) / sampleRate;
            wave[i] += (float) (Math.sin(phase) * strength);
            System.out.println(wave[i]);
        }

        return wave;
    }

    // Combine two waves, this does not ensure that result is normalized
    public float[] combineWaves(float[] waveA, float[] waveB) {
        if (waveA.length != waveB.length) {
            throw new IllegalArgumentException("Wave lenghts must be equal");
        }

        float[] wave = new float[waveA.length];

        for (int i = 0; i < waveA.length; i++) {
            wave[i] = waveA[i] + waveB[i];
        }

        return wave;
    }

    public float[] normalize(float[] wave) {
        if (!isNormalized(wave)) {
            // Calculate minimum
            float min = wave[0];
            for (int i = 1; i < wave.length; i++) { // begin at 1 because 0 is the initial value
                if (min > wave[i]) { min = wave[i]; }
            }
            System.out.println("Min: " + min);

            // Calculate maximum
            float max = wave[0];
            for (int i = 1; i < wave.length; i++) { // begin at 1 because 0 is the initial value
                if (max < wave[i]) { max = wave[i]; }
            }
            System.out.println("Max: " + max);

            float maxmin = max - min; // Precalculate value for SPEED

            for (int i = 0; i < wave.length; i++) {
                wave[i] = 2 * ((wave[i] - min) / maxmin) - 1;
            }
        }

        return wave;
    }

    // Check if input wave is normalized. Audio playback will be broken if not
    public boolean isNormalized(float[] wave) {
        for (int i = 0; i < wave.length; i++) {
            if (wave[i] > 1) { return false; }
        }

        return true;
    }

    // Add noise to sine
    public float[] addNoise(float[] wave, float strength) {
        for (int i = 0; i < wave.length; i++) {
            wave[i] += Math.random() * strength;
        }

        return wave;
    }

    // Convert sine wave to triangle wave
    public float[] sineToTriangle(float[] wave) {
        for (int i = 0; i < wave.length; i++) {
            wave[i] = 2 * Math.abs(wave[i]) - 1;
        }

        return wave;
    }
}
