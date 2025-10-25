package commands;

public class HelpCommand implements Command {
	public HelpCommand() {
		execute();
	}

    @Override
    public void execute() {
        System.out.println("usage: adess <command> [<component>] [<options>]\n");

	System.out.println("Adess - Artist Directed Engine Sound Synthetizer\n");

	System.out.println("Adess generates sound using these 4 hierarchical components:");
	System.out.println("\tproject\tProjects are directories that contain all adess files needed\n\t\tfor engine sound generation, they are signified by a .adess \n\t\tmeta file. You can find settings such as sample rate, bitrate,\n\t\tect. in the config.adess file.");
	System.out.println("\tengine\tThe engine files define the character of their sound, you can\n\t\tcontrol their pitch, volume, harmonics, timbre, and a lot more.");
	System.out.println("\tscene\tScenes have data about the scenes length, the engine used, and\n\t\tkeyframe interpolation.");
	System.out.println("\tkey\tKeys (keyframes) hold information like time (in seconds or frames),\n\t\trpm, load and more. They belong to scenes.");

	System.out.println("\nThese components can be accessed by the these commands:");
	System.out.println("\tmake\t[mk]\tCreate a component");
	System.out.println("\tedit\t[ed]\tEdit a component");
	System.out.println("\tshow\t[sh]\tShow information about a component");
	System.out.println("\tremove\t[rm]\tRemove a component");
	
	System.out.println("\nSee 'adess help <command>' for help with a specific command or use 'adess help -a'\nto list all available commands. See 'adess help guide' for an in-depth explanation\nof the whole system.");
    }
}
