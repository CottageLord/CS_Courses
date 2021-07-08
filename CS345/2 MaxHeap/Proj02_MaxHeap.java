/**
 * Implementation of Heap functions. Builder, insert, remove, dump (print).
 *
 * @author Yang Hu
 *
 */

import java.io.*;

public class Proj02_MaxHeap {

	private Comparable[] storage;
	private boolean debug;
	private int length, capacity, dump_length;

	public Proj02_MaxHeap (boolean debugFlag) {
		storage = new Comparable[4];
		debug = debugFlag;
		length = 0;
		capacity = 4;
	}

	public Proj02_MaxHeap (boolean debugFlag, Comparable[] arr) {
		storage = arr;
		capacity = storage.length;
		length = capacity;
		debug = debugFlag;
		length = arr.length;
		build_max_heap();
	}

	/**
    * Max heap builder. find all parents and sort respective braches (bubble down).
    */

	public void build_max_heap() {
		dump_length = length;
		int parent_index = length / 2 - 1;
		//int child_index = parent_index * 2 + 1;

		while (parent_index >= 0) {
			bubble_down(parent_index);
			parent_index--;
		}
	}

	/**
    * This method insert one element into the max heap.
    * @param value the to be inserted element
    * @return nothing
    */
	public void insert(Comparable value) {
		length++;
		dump_length = length;
		if (length > capacity) {
			extend();
		}
		storage[length - 1] = value;
		bubble_up(length - 1);
	}
	/**
    * This method extends the original array (double size). Copy the original
    * array into the new expanded array
    */
	private void extend () {
		capacity *= 2;
		Comparable[] temp = new Comparable[capacity];
		for (int i = 0; i < length - 1; i++) {
			temp[i] = storage[i];
		}
		storage = temp;
	}

	/**
    * This method bubble an element up to right position.
    * @param index the position of the element
    * @return nothing
    */
	private void bubble_up (int index) {
		// find parent
		int parent_index = index - 1;
		// keep fiding parents
		while ( parent_index >= 0 ) {
			parent_index /= 2;

			// if the inserted value is larger than the parent
			if (storage[index].compareTo(storage[parent_index]) > 0) {
				// swap
				Comparable temp = storage[parent_index];
				storage[parent_index] = storage[index];
				storage[index] = temp;

				// find the upper parent
				index = parent_index;
				parent_index = parent_index - 1;
			} else {
				break;
			}
		}
	}

	/**
    * This method remove the max element from the heap structure.
    * @return the removed element
    */
	public Comparable removeMax() {
		dump_length = length - 1;
		// nothing to remove
		if (length <= 0) {
			return null;
		}
		// swap head and tail
		Comparable root = storage[0];
		storage[0] = storage[length - 1];
		storage[length - 1] = root;
		// exclude the tail (who was the root)
		length--;
		if(debug) {
			System.out.print("length: " + length + " swap: ");
			printer();
		}
		bubble_down(0);
		if (debug) {
			System.out.print("bubble down: ");
			printer();
		}
		return root;
	}

	/**
    * This method bubble an element down to right position.
    * @param index the position of the element
    * @return nothing
    */
	private void bubble_down(int index) {
		int child_index = 2 * index + 1;
		Comparable temp;
		while(child_index < length) {
			// find the largest in current parent 
			//                            /      \
			// 						   child   child
			if(child_index + 1 >= length) {
				// only one child
				// do nothing
			} else if(storage[child_index].compareTo(storage[child_index + 1]) < 0) {
				child_index++;
			}
			// return if parent > child (max heap built)
			if(storage[child_index].compareTo(storage[index]) < 0) {
				return;
			}
			// swap
			temp = storage[index];
			storage[index] = storage[child_index];
			storage[child_index] = temp;

			index = child_index;
			child_index = 2 * child_index + 1;
		}
	}
	
	/**
    * This method prints out the current state of the array;
    * @param printWriter the given printwriter
    * @return nothing
    */
	public void dump(PrintWriter printWriter) {
		// if nothing to print
		// print empty line
		if (dump_length == 0) {
			printWriter.println();
			return;
		}
		// print elements
		for (int i = 0; i < dump_length - 1; i++) {
			printWriter.printf("%s ", storage[i]);
		}
		printWriter.printf("%s\n", storage[dump_length - 1]);
		printWriter.flush();
		if (debug) {
			make_dot();
		}
	}
	
	/**
    * This method generates the .dot file for visualizing the heap tree
    */
	private void make_dot() {
		try{
			FileWriter writer = new FileWriter("pattern.dot");
			//File pattern = new File("pattern.dot");
			PrintWriter printWriter = new PrintWriter(writer);
			printWriter.printf("digraph\n{\n");
			// print vartexies
			for (int i = 0; i < length; i++) {
				printWriter.printf("%d [label=\"%s\"];\n", i, storage[i]);
			}
			// print edges
			for (int i = 0; i < length; i++) {
				int child_index = 2 * i + 1;
				if (child_index < length) {
					printWriter.printf("%d -> %s [label=\"left\"];\n", i, child_index);
				}
				if (child_index + 1 < length) {
					printWriter.printf("%d -> %s  [label=\"right\"];\n", i, child_index + 1);
				}
			}
			printWriter.printf("}\n");
			printWriter.close();
		} catch (IOException e) {
			System.err.println("cannot generate .dot file");
		}
	}
	/**
    * This method is a debug printer. print the physical array and the heap area
    */
	public void printer () {
		if (capacity <= 0) {
			return;
		}
		for (int i = 0; i < capacity - 1; i++) {
			if(i == length - 1) {
				System.out.print(storage[i] + " | ");
			} else {
				System.out.print(storage[i] + ", ");
			}
		}
		System.out.print(storage[capacity - 1]);
		System.out.println();
	}
}