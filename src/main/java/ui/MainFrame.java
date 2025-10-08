package ui;

import javax.swing.*;
import java.awt.*;

public class MainFrame extends JFrame {
    public MainFrame() {
        
        // Creating the main frame
        setTitle("ADESS - artist driven engine sound synthetizer");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setSize(800, 600);
        setLayout(new BorderLayout());

        // Adding the top menu bar
        MenuBar menuBar = new MenuBar();
        setJMenuBar(menuBar);

        // Create panels
        JPanel keyPanel= new KeyPanel();
        JPanel paramPanel = new ParamPanel();
        JPanel videoPanel = new VideoPanel();
        JPanel filterPanel = new FilterPanel();
        JPanel feedbackPanel = new FeedbackPanel();
        
        // Create grid and add the rest of the panels
        add(createGrid(keyPanel, paramPanel, videoPanel, filterPanel, feedbackPanel), BorderLayout.CENTER);
    }

    private JPanel createGrid(JPanel keyPanel, JPanel paramPanel, JPanel videoPanel, JPanel filterPanel, JPanel feedbackPanel) {
        JPanel mainPanel = new JPanel();
        mainPanel.setLayout(new GridBagLayout());
        GridBagConstraints gbc = new GridBagConstraints();
        gbc.fill = GridBagConstraints.BOTH;

        JPanel contentPanel = new JPanel();
        contentPanel.setLayout(new GridLayout(2, 1));

        JPanel topPanel = new JPanel();
        topPanel.setLayout(new GridLayout(1, 2));

        topPanel.add(paramPanel);
        topPanel.add(videoPanel);

        JPanel bottomPanel = new JPanel();
        bottomPanel.setLayout(new GridLayout(1, 2));

        bottomPanel.add(filterPanel);
        bottomPanel.add(feedbackPanel);

        contentPanel.add(topPanel);
        contentPanel.add(bottomPanel);

        // Add the main content to position (0, 0) and set weight for filling space
        gbc.weightx = 2; // Give double the weight to contentPanel
        gbc.weighty = 1; // Row height
        gbc.gridx = 0;   // Column 0
        gbc.gridy = 0;   // Row 0
        mainPanel.add(contentPanel, gbc);

        // Add keyPanel to the right (1, 0) with a fixed width
        gbc.weightx = 1; // Give less weight to keyPanel
        gbc.fill = GridBagConstraints.BOTH; // Make sure it's filling vertically
        gbc.gridx = 1;   // Column 1
        gbc.gridy = 0;   // Row 0
        mainPanel.add(keyPanel, gbc);

        return mainPanel;
    }
}
