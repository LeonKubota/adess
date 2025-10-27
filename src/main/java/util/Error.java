package util;

public class Error {
	private String MESSAGE;

	public Error(String MESSAGE) {
		System.err.println("fatal: " + MESSAGE);
		System.exit(1);
	}
}
