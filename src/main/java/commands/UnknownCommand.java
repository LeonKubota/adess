package commands;

public class UnknownCommand implements Command {
    private String command;

    public UnknownCommand(String command) {
        this.command = command;

	execute();
    }

    @Override
    public void execute() {
        System.err.println("adess: '" + command + "' is not an adess command. See 'adess --help'.");
	System.exit(1);
    }
}
