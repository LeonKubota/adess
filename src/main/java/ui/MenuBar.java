package ui;

import javax.swing.*;
import java.awt.*;

public class MenuBar extends JMenuBar {
    public MenuBar() {

        // Create "File" menu
        JMenu fileMenu = new JMenu("File");

        JMenuItem openItem = new JMenuItem("Load");
        JMenuItem saveItem = new JMenuItem("Save");
        JMenuItem saveAsItem = new JMenuItem("Save as");
        JMenuItem quitItem = new JMenuItem("Quit");

        fileMenu.add(openItem);
        fileMenu.add(saveItem);
        fileMenu.add(saveAsItem);
        quitItem.addActionListener(e -> System.exit(0));
        fileMenu.add(quitItem);

        // Create "Export" menu
        JMenu exportMenu = new JMenu("Export");
        JMenuItem exportItem = new JMenuItem("Export");
        JMenuItem exportAsItem = new JMenuItem("Export as");

        exportMenu.add(exportItem);
        exportMenu.add(exportAsItem);
        
        // Create "Help" menu
        JMenu helpMenu = new JMenu("Help");

        JMenuItem documentationItem = new JMenuItem("Documentation");
        JMenuItem helpItem = new JMenuItem("Help");
        JMenuItem aboutItem = new JMenuItem("About");

        helpMenu.add(documentationItem);
        helpMenu.add(helpItem);
        helpMenu.add(aboutItem);

        // Adding Menus into the Bar
        add(fileMenu);
        add(exportMenu);
        add(helpMenu);
    }
}
