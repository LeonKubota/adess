package config;

import java.io.File;
import java.io.IOException;

public class CreateProject {
	private String projectName;
	private String projectDirectory;

	public CreateProject(String projectName) {
		this.projectName = projectName;
		this.projectDirectory = System.getProperty("user.dir");

		createProjectDirectory(projectName, projectDirectory);
		createConfigFile(projectName, projectDirectory);
	}

	public CreateProject(String projectName, String projectDirectory) {
		this.projectName = projectName;
		this.projectDirectory = projectDirectory;

		createProjectDirectory(projectName, projectDirectory);
		createConfigFile(projectName, projectDirectory);
	}

	public void createProjectDirectory(String projectName, String projectDirectory) {
		File directory = new File(projectDirectory + File.separator + projectName);
		boolean directoryCreated = directory.mkdir();

		if (directoryCreated) {
			System.out.println("Main directory created successfully at: " + projectDirectory + File.separator + projectName);
		} else {
			System.out.println("Failed to create project directory, directory may already exist"); 
		}
	}

	public void createConfigFile(String projectName, String projectDirectory) {
		try {
			File file = new File(projectDirectory + File.separator + projectName + File.separator + "config.adess");
			boolean fileCreated = file.createNewFile();

			if (fileCreated) {
				System.out.println("Config file created successfully at: " + projectDirectory + File.separator + projectName + File.separator + "config.adess");
			} else {
				System.out.println("Failed to create config file, file may already exist");
			}
		} catch (IOException e) {
			System.out.println("An error occured during creation of config file");
			e.printStackTrace();
		}
	}
}    
