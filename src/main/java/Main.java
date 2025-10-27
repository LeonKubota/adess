// import analysis.*;
import commands.*;
import config.*;
// import data.*;
// import generation.*;
// import rendering.*;

import java.util.HashSet;
import java.util.Set;

class Main {
	// Turn on for debugging
	public static final boolean DEBUG = true;
    	public static void main(String[] inputArgs) {
        	// time (for performance measuring)
        	long startTime = System.nanoTime();
	
		// If no command given
		if (inputArgs.length == 0) {
			new HelpCommand();
			System.exit(0);
		}

		// Parse raw input into useful information
		// Parse command
		String command = inputArgs[0];
		inputArgs.remove(0);

		// Parse arguments
		int numOfArgs = 0;
		for (int i = 0; i < inputArgs.length; i++) {
			if (inputArgs.startsWith("-")) {
				break;
			} else {
				numOfArgs += 1;
			}
		}
		System.out.println("numOfArgs: " + numOfArgs);

		// private String[] args = new String[numOfArgs];

		if (DEBUG) {
			debug("[COMMAND]\t" + command);
			for (int i = 0; i < args.length; i++) {
				debug("[ARGUMENT]\t" + args[i]);
			}
			// for (int i = 0; i < opts.length; i++) {
				// debug("[OPTION]\t" + opts[i]);
			// }
		}
	
		switch (command) {
			case "mk":
			case "make":
				debug("MakeCommand");
				if (args.length == 0) {
					new MakeCommand();
				} else if (args.length == 1) {
					new MakeCommand(args[0]);
				} else if (args.lenght == 2) {
					new MakeCommand(args[0], args[1]);
				} else {
					new MakeCommand(args);
				}
			case "ed":
			case "edit":
				if (DEBUG) {System.out.println("[DEBUG]\tEditCommand");}
				break;
	
			case "sh":
			case "show":
				if (DEBUG) {System.out.println("[DEBUG]\tShowCommand");}
				break;
	
			case "rm":
			case "remove":
				if (DEBUG) {System.out.println("[DEBUG]\tRemoveCommand");}
				break;
	
			case "-h":
			case "--help":
				if (DEBUG) {System.out.println("[DEBUG]\tHelpCommand");}
				new HelpCommand();
				break;
	
			case "help":
				if (DEBUG) {System.out.println("[DEBUG]\tHelpCommand");}
				new HelpCommand();
				break;
			default:
				new UnknownCommand(args[0]);
		}
	}
	
	public static void debug(String message) {
		if (DEBUG) {
			System.out.println("[DEBUG]\t" + message);
		}
	}
}
