package commands;

public class UnknownCommand implements Command {
    private String command;

    public UnknownCommand(String command) {
        this.command = command;
    }

    @Override
    public void execute() {
        System.out.println("adess: '" + command + "' is not an adess command. See 'adess --help'.");
    }
}
