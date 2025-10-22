package commands;

import config.CreateProject;

public class CreateProjectCommand implements Command {
    private String projectName;
    private String projectDirectory;

    public CreateProjectCommand(String projectName) {
        this.projectName = projectName;
        this.projectDirectory = System.getProperty("user.dir");
    }

    public CreateProjectCommand(String projectName, String projectDirectory) {
        this.projectName = projectName;
        this.projectDirectory = projectDirectory;
    }

    @Override
    public void execute() {
        CreateProject cp = new CreateProject(projectName, projectDirectory);
    }
}
