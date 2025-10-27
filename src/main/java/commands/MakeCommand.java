package commands;

import util.Error;

import config.MakeProject;
// import config.MakeEngine;
// import config.MakeScene;
// import config.MakeKey;

// Takes: TYPE, NAME, DIRECTORY (optional), OPTIONS
public class MakeCommand implements Command {
	final private String TYPE = null;
	final private String NAME = null;
	final private String DIRECTORY = null;
	final private String[] ARGS = null;

	public MakeCommand() {
		new Error("You must specify the type of component to create.");
	}

	public MakeCommand(String TYPE) {
		this.TYPE = TYPE;
		switch (TYPE) {
			case "project":
				new Error("A project has to have a name.");
				break;
			case "engine":
				new Error("An engine has to have a name.");
				break;
			case "scene":
				new Error("A scene has to have a name.");
				break;
			case "key":
				new Error("A key has to have a time value.");
				break;
			default:
				new Error("Cannot create component, '" + TYPE + "' is not a valid component.");
				break;
		}
	}

	public MakeCommand(String TYPE, String NAME) {
	}

	public MakeCommand(String[] ARGS) {
		new Error("Too many arguments.");
	}

	@Override
	public void execute() {
	} 
}
