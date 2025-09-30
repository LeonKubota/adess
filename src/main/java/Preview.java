import javax.sound.sampled.*;
import java.util.ArrayList;
import java.util.List;

public class Preview {
    public static void playAudio(float sampleRate, float[] samples) {

        // Convert float[] to byte[] field with little-endian standard
        byte[] audioData = new byte[samples.length * 2];

        for (int i = 0; i < samples.length; i++) {
            short value = (short) (samples[i] * Short.MAX_VALUE); // Converting -1 to 1 to -32768 to 32767 (16-bit short)
            audioData[i * 2] = (byte) (value & 0xFF);
            audioData[i * 2 + 1] = (byte) ((value >> 8) & 0xFF);
        }

        AudioFormat format = new AudioFormat(sampleRate, 16, 1, true, false); // AudioFormat(sampleRate, bits, channels, signed, little/big endian)
        DataLine.Info info = new DataLine.Info(SourceDataLine.class, format);

        try {
            // Obtain and open the line
            SourceDataLine line = (SourceDataLine) AudioSystem.getLine(info);
            line.open(format);
            line.start();

            // Write audio data to the line
            line.write(audioData, 0, audioData.length);

            line.drain();
            line.stop();
            line.close();
        } catch (LineUnavailableException e) {
            e.printStackTrace();
        }
    }
}
