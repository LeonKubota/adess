package commands;

public class HelpCommand implements Command {
    @Override
    public void execute() {
        System.out.println("usage: blablabla");
    }
}
