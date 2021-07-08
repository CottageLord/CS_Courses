/**
 * Proj06_TuringMachine_student
 *
 * author: Yang Hu
 *
 * This class represents a single transition in a Turing Machine
 */

public class Transition_student {
	// properties
	private String from;
	private char read;
	private char write;
	private boolean moveDir;
	private String to;
	
	public Transition_student(String givenFrom, char givenRead, 
		char givenWrite, boolean givenMoveD, String givenTo) {

		from = givenFrom;
		read = givenRead;
		write = givenWrite;
		moveDir = givenMoveD;
		to = givenTo;
	}

	/*-------------------- GETTERS -----------------*/

	public String getFrom() {
		return from;
	}

	public String getTo() {
		return to;
	}

	public char getRead() {
		return read;
	}

	public char getWrite() {
		return write;
	}

	public boolean getMoveDir() {
		return moveDir;
	}
}