package config;

import java.io.File;
import java.io.IOException;

public class CreateProject {
	private String projectName;
	private String directoryPath;

	public CreateProject(String projectName) {
		this.projectName = projectName;
		
		createProjectDirectory(projectName);
		createConfigFile(projectName);
	}

	public CreateProject(String projectName, String directoryPath) {
		this.projectName = projectName;
		this.directoryPath = directoryPath;

		createProjectDirectory(projectName, directoryPath);
		createConfigFile(projectName, directoryPath);
	}

	public void createProjectDirectory(String projectName) {
		String currentDirectory = System.getProperty("user.dir");
		File directory = new File(currentDirectory + File.separator + projectName);
		boolean directoryCreated = directory.mkdir();

		if (directoryCreated) {
			System.out.println("Main directory created successfully at: " + currentDirectory + File.separator + projectName);
		} else {
			System.out.println("Failed to create project directory, directory may already exist"); 
		}
	}

	public void createProjectDirectory(String projectName, String directoryPath) {
		File directory = new File(directoryPath + File.separator + projectName);
		boolean directoryCreated = directory.mkdir();

		if (directoryCreated) {
			System.out.println("Main directory created successfully at: " + directoryPath + File.separator + projectName);
		} else {
			System.out.println("Failed to create project directory, directory may already exist"); 
		}
	}

	public void createConfigFile(String projectName) {
		String currentDirectory = System.getProperty("user.dir");
		try {
			File file = new File(currentDirectory + File.separator + projectName + File.separator + "config.adess");
			boolean fileCreated = file.createNewFile();

			if (fileCreated) {
				System.out.println("Config file created successfully at: " + currentDirectory + File.separator + projectName + File.separator + "config.adess");
			} else {
				System.out.println("Failed to create config file, file may already exist");
			}
		} catch (IOException e) {
			System.out.println("An error occured during creation of config file");
			e.printStackTrace();
		}
	}

	public void createConfigFile(String projectName, String directoryPath) {
		try {
			File file = new File(directoryPath + File.separator + projectName + File.separator + "config.adess");
			boolean fileCreated = file.createNewFile();

			if (fileCreated) {
				System.out.println("Config file created successfully at: " + directoryPath + File.separator + projectName + File.separator + "config.adess");
			} else {
				System.out.println("Failed to create config file, file may already exist");
			}
		} catch (IOException e) {
			System.out.println("An error occured during creation of config file");
			e.printStackTrace();
		}
	}
}    
