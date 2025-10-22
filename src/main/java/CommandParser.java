import commands.*;

public class CommandParser {
    public Command parse(String[] args) {
        if (args.length == 0) {
           return new HelpCommand(); 

        }

        switch (args[0]) {
            case "create":
                return new CreateProjectCommand(args[0]);
            case "help":
                return new HelpCommand();
            default:
                return new UnknownCommand(args[0]);
        }
    }
}
