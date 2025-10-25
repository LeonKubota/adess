package commands;

import config.MakeProject;

public class MakeProjectCommand implements Command {
    private String projectName;
    private String projectDirectory;

    public MakeProjectCommand() {
	System.err.println("fatal: You must provide a project name");
	System.exit(1);
    }

    public MakeProjectCommand(String projectName) {
        this.projectName = projectName;
        this.projectDirectory = System.getProperty("user.dir");
	
	execute();
    }

    public MakeProjectCommand(String projectName, String projectDirectory) {
        this.projectName = projectName;
        this.projectDirectory = projectDirectory;

	execute();
    }

    @Override
    public void execute() {
        MakeProject mkp = new MakeProject(projectName, projectDirectory);
	System.exit(0);
    }
}
