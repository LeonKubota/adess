import java.util.ArrayList;
import java.util.List;

public class Render {

    // Tau (useful for calculations)
    public static final float TAU = 2.0f * (float) Math.PI;

    // Generate a sine wave as a base for further filtering, float[] frequencies MUST be as long as the whole thing.
    public float[] generateSine(float sampleRate, float[] frequencies, float amplitude, int harmonicSeries) {
        float[] wave = new float[frequencies.length];
        float phase; // phase to ensure phase continuity

        for (int i = 0; i < frequencies.length; i++) {
            float frequency = frequencies[i]; // get current frequency
            frequency *= harmonicSeries; // multiply by harmonic series, useful for creating harmonics
            phase = (TAU * frequency * i) / sampleRate;
            wave[i] = (float) (Math.sin(phase) * amplitude);
        }

        return wave;
    }
}
