/**
 * TMState_student
 *
 * author: Yang Hu
 *
 * This class represents a single state in a Turing Machine
 */

import java.util.HashMap;
import java.util.HashSet;

public class TMState_student {
	// properties
	private String stateName;
	private boolean accept;
	// all transitions that go from this state
	private HashSet<Transition_student> transitions;

	public TMState_student(String givenStateName, boolean givenAccept) {
		transitions = new HashSet<Transition_student>(); 
		stateName = givenStateName;
		accept = givenAccept;
	}
	
	/*-------------------- GETTERS -----------------*/

	public HashSet<Transition_student> getTransition() {
		return transitions;
	}

	public void addTransition(Transition_student transition) {
		transitions.add(transition);
	}

	public String getName() {
		return stateName;
	}

	public boolean getAccept() {
		return accept;
	}
}