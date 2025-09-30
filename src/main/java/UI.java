import javax.swing.*;
import java.awt.*;

public class UI {
    private JFrame frame;

    public UI() {
        // Creating and defining frame
        frame = new JFrame("Adess - Artist Directed Engine Sound Synthetizer");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setSize(1920, 1080);

        // Add the main panel (so that it's responsive)
        JPanel mainPanel = new JPanel(new BorderLayout());

        // Add control panels into main panel
        mainPanel.add(createControlPanels(), BorderLayout.CENTER);

        frame.add(mainPanel);
        frame.setJMenuBar(createMenuBar());
        frame.setVisible(true);
        JMenuBar menuBar = new JMenuBar();
        
    }

    private JMenuBar createMenuBar() {
        JMenuBar menuBar = new JMenuBar();

        // File Menu
        JMenu fileMenu = new JMenu("File");
        JMenuItem openItem = new JMenuItem("Open");
        JMenuItem exitItem = new JMenuItem("Exit");
        exitItem.addActionListener(e -> System.exit(0));
        fileMenu.add(openItem);
        fileMenu.add(exitItem);

        // Render Menu
        JMenu renderMenu = new JMenu("Render");
        JMenuItem renderItemWAV = new JMenuItem("Render Audio as WAV");
        JMenuItem renderItemMp3 = new JMenuItem("Render Audio as mp3");
        renderMenu.add(renderItemWAV);
        renderMenu.add(renderItemMp3);

        // Help Menu
        JMenu helpMenu = new JMenu("Help");
        JMenuItem helpItem = new JMenuItem("Help");
        helpItem.addActionListener(e -> System.out.println("There is no help, figure it out, it's not that complicated :P"));
        JMenuItem aboutItem = new JMenuItem("About");
        helpMenu.add(helpItem);
        helpMenu.add(aboutItem);

        // Adding Menus into the Bar
        menuBar.add(fileMenu);
        menuBar.add(renderMenu);
        menuBar.add(helpMenu);

        return menuBar;
    }

    private JPanel createControlPanels() {
        // Create top and bottom panel
        JPanel userPanel = new JPanel();
        userPanel.setLayout(new BoxLayout(userPanel, BoxLayout.Y_AXIS));

        // Control pane
        JPanel controlPanel = new JPanel(new GridLayout (1, 4));

        // Create technical parameters panel
        JPanel technicalPanel = new JPanel();
        JLabel technicalPanelLabel = new JLabel("Technical Parameters");
        technicalPanel.add(technicalPanelLabel);
        JScrollPane technicalScrollPane = new JScrollPane(technicalPanel);
        controlPanel.add(technicalScrollPane);

        // Create engine parameters panel
        JPanel enginePanel = new JPanel();
        JLabel enginePanelLabel = new JLabel("Engine Parameters");
        enginePanel.add(enginePanelLabel);
        JScrollPane engineScrollPane = new JScrollPane(enginePanel);
        controlPanel.add(engineScrollPane);

        // Create animation parameters panel
        JPanel animationPanel = new JPanel();
        JLabel animationPanelLabel = new JLabel("Animation Parameters");
        animationPanel.add(animationPanelLabel);
        JScrollPane animationScrollPane = new JScrollPane(animationPanel);
        controlPanel.add(animationScrollPane);

        // Create filter parameters panel
        JPanel filterPanel = new JPanel();
        JLabel filterPanelLabel = new JLabel("Filter Parameters");
        filterPanel.add(filterPanelLabel);
        JScrollPane filterScrollPane = new JScrollPane(filterPanel);
        controlPanel.add(filterScrollPane);

        JPanel graphPanel = new JPanel();
        graphPanel.add(new JLabel("Graph Panel"));

        userPanel.add(controlPanel);
        userPanel.add(graphPanel);

        return userPanel;
    }
}
