/**
 * Proj06_TuringMachine_student
 *
 * author: Yang Hu
 *
 * This class simulates a Turing Machine
 */

import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Map;

public class Proj06_TuringMachine_student implements Proj06_TuringMachine{

	private HashMap<String, TMState_student> states;
	private TMState_student firstState;
	/*
	 * Constructor, initialize the needed data
	 */
	public Proj06_TuringMachine_student() {
		states = new HashMap<String, TMState_student>();
		firstState = null;
	}

	/*
	 * add a state to the state map
	 */
	public void addState(String stateName, boolean accept){
		// create a new state
		TMState_student newState = new TMState_student(stateName, accept);
		states.put(stateName, newState);
		// remember the first state
		if (firstState == null) firstState = newState;
	}

	/*
	 * add the transition to the from state
	 */
	public void addTransition(String from, char read,
	                   char write, boolean moveDir, String to){
		// create new transition
		Transition_student newTransition = new Transition_student(from, read, write, moveDir, to);
		// find the from state
		TMState_student fromState = states.get(from);
		// add transition to it
		fromState.addTransition(newTransition);
	}
	/*
	 * run Turing Machine on a given string by the states/transitions stored
	 */
	public void run(String startString, boolean debug){
		// from the first state
		TMState_student currState = firstState;
		// the index of the chars in the given startString
		int index = 0;

		if (!debug) debugPrinter(startString, index, currState);
		// whenever the current state is accepted
		// return (work done)
		if (currState.getAccept()) {
			debugPrinter(startString, index, currState);
			System.out.println("\nMACHINE ACCEPTS!");
			return;
		}

		// run Turing Machine algorithm.
		while (true) {

			if (debug) debugPrinter(startString, index, currState);
			// iterate through the input string
			char currChar = startString.charAt(index);

			// if (debug) printHelper(startString, index, currState);
			// Get the transitions of current state.
			HashSet<Transition_student> transitions = currState.getTransition();
			
			if (currState.getAccept()) {
				if (!debug) debugPrinter(startString, index, currState);
				System.out.println("\nMACHINE ACCEPTS!");
				return;
			}
			// rejection checker
			boolean rejected = true;

			// iterate through all transitions of the current state
			for (Transition_student transition : transitions) {
				// if the operation is read.
				if (transition.getRead() == currChar) {

					rejected = false;

					// Update string by index and Turing Machine State's write.
					// 1. if we are at the end of the string
					if (index == startString.length() - 1) {
						// append transition.getWrite() to the end of string
						startString = startString.substring(0, index) + transition.getWrite();
					}
					// 2. if we are in the middle of the string
					else if (0 <= index && index < startString.length()-1) {
						// insert the transition.getWrite() in the midle
						startString = startString.substring(0, index) + 
							transition.getWrite() + startString.substring(index + 1);
					}
					// 3. if we are going ahead of the string
					else if (index == -1) {
						// insert the transition.getWrite() to the beginning
						startString = transition.getWrite() + startString.substring(0, index);
						// set the current pos as 0 (was -1)
						index = 0;
					// 4. if we are going after the string
					} else if (index == startString.length()) {
						// append transition.getWrite() to the end of string
						startString = startString.substring(0, index) + transition.getWrite();
					}

					// Update the index base on the direction.
					if (transition.getMoveDir()) {
						index += 1;
						// if the index if longer than the current string
						if (index == startString.length()) {
							// expand the string by a "."
							startString += '.';
						}
					} else {
						index -= 1;
						// if the index is out of bound
						if (index == -1) {
							// expand the string by a "."
							startString = '.' + startString;
							// restore index
							index = 0;
						}
					}
					// move to the next state
					currState = states.get(transition.getTo());
					break;
				}
			}
			// if the turing machine is rejected
			// done
			if (rejected) {
				if (!debug) debugPrinter(startString, index, currState);
				System.out.println("\nMACHINE REJECTS!");
				return;
			}
		}
	}
	/*
	 * The debug printer
	 * print in format:
	 * givenString
	 *	    ^   state: init/done
	 */
	private void debugPrinter(String inputString, int currentPos, TMState_student currState) {
		// print out fiven string
		System.out.println(inputString);
		// print out the position pointer
		for (int i = 0; i < currentPos; i++) {
			System.out.print(" ");
		}
		// print out the current state
		System.out.printf("^   state: %s\n", currState.getName());
	}
}