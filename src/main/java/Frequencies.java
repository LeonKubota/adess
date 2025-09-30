public class Frequencies {

    // Tau (useful for calculations)
    public static final float TAU = 2.0f * (float) Math.PI;

    // Generates a testing set of frequencies from minFreq to maxFreq
    public float[] testFrequency(int length, float minFreq, float maxFreq) {
        float[] frequencies = new float[length];

        double range = maxFreq - minFreq;

        double epsilon = 1e-10;
        double increment = (range / length) > epsilon ? (range / length) : epsilon;

        for (int i = 0; i < length; i++) {
            frequencies[i] = (float) (minFreq + (increment * i));
        }

        return frequencies;
    }
}
