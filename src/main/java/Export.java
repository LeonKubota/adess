import javax.sound.sampled.*;
import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.io.FileWriter;

public class Export {
    public static void exportWaveform(float sampleRate, float[] samples, String filePath) {

        // Convert float[] to byte[] field with little-endian standard
        byte[] audioData = new byte[samples.length * 2];

        for (int i = 0; i < samples.length; i++) {
            short value = (short) (samples[i] * Short.MAX_VALUE); // Converting -1 to 1 to -32768 to 32767 (16-bit short)
            audioData[i * 2] = (byte) (value & 0xFF);
            audioData[i * 2 + 1] = (byte) ((value >> 8) & 0xFF);
        }

        AudioFormat format = new AudioFormat(sampleRate, 16, 1, true, false);

        // Making the WAV file
        try (AudioInputStream audioInputStream = new AudioInputStream(new ByteArrayInputStream(audioData), format, audioData.length / format.getFrameSize())) {
            File outputFile = new File(filePath);
            AudioSystem.write(audioInputStream, AudioFileFormat.Type.WAVE, outputFile);
            System.out.println("Audio successfully saved to directory: " + filePath);
        }
        catch (IOException e) {
            e.printStackTrace();
        }
    }
    public void dumpInText(float[] sineWave, String filePath) {
        String output = "";

        for (int i = 0; i < sineWave.length; i++) {
            output += Float.toString(sineWave[i]) + ",";
        }

        try {
            FileWriter wr = new FileWriter(filePath);
            wr.write(output);
            wr.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
