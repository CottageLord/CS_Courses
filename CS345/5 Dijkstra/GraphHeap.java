/* GraphHeap
 * Adapted from Project 2
 *
 * author: Yang Hu
 *
 * A min heap of GrapgEdges, sorted by distance
 */

import java.io.*;

public class GraphHeap {

	private GraphEdge[] storage;
	private int length, capacity, dump_length, restoreLength;

	public GraphHeap() {
		storage = new GraphEdge[4];
		length = 0;
		restoreLength = 0;
		capacity = 4;
	}

	public int getLength() {
		return length;
	}
	/**
    * This method insert one element into the max heap.
    * @param value the to be inserted element
    * @return nothing
    */
	public void insert(GraphEdge value) {
		length++;
		if(length > restoreLength) restoreLength = length;
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
		GraphEdge[] temp = new GraphEdge[capacity];
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
		// GraphEdgeeep fiding parents
		while ( parent_index >= 0 ) {
			parent_index /= 2;

			// if the inserted value is smaller than the parent
			if (storage[index].getWeight() < storage[parent_index].getWeight()) {
				// swap
				GraphEdge temp = storage[parent_index];
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
	public GraphEdge removeMin() {
		dump_length = length - 1;
		// nothing to remove
		if (length <= 0) {
			return null;
		}
		// swap head and tail
		GraphEdge root = storage[0];
		storage[0] = storage[length - 1];
		storage[length - 1] = root;
		// exclude the tail (who was the root)
		length--;
		bubble_down(0);
		return root;
	}

	public void restoreLength() {
		length = restoreLength;
	}
	/**
    * This method bubble an element down to right position.
    * @param index the position of the element
    * @return nothing
    */
	private void bubble_down(int index) {
		int child_index = 2 * index + 1;
		GraphEdge temp;
		while(child_index < length) {
			// find the smallest in current parent 
			//                            /      \
			// 						   child   child
			if(child_index + 1 >= length) {
				// only one child
				// do nothing
			} else if(storage[child_index].getWeight() > storage[child_index + 1].getWeight()) {
				child_index++;
			}
			// return if parent < child (min heap built)
			if(storage[child_index].getWeight() > storage[index].getWeight()) {
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
    * This method is a debug printer. print the physical array and the heap area
    */
	public void printer () {
		if (capacity <= 0) {
			return;
		}
		for (int i = 0; i < capacity - 1; i++) {
			if(i == length - 1) {
				System.out.print(storage[i].getWeight() + " | ");
			} else {
				System.out.print(storage[i].getWeight() + ", ");
			}
		}
		System.out.print(storage[capacity - 1]);
		System.out.println();
	}
}

