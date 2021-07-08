/* Implementation of Merge Sort algorithm
 *
 * Author: Yang Hu
 */
import java.util.Arrays;

public class Proj01_MergeSort implements Proj01_Sort
{
	private Boolean debug;
	private int baseLen;
	
	public Proj01_MergeSort(Boolean debug_flag, int length)
	{
		debug = debug_flag;
		baseLen = length;
	}

	public void merge(Comparable[] arr, int start_index, int midpoint, int end_index) {
		// Size of merged partition
    	int mergedSize = end_index - start_index + 1;
    	// Temporary array for merged arr
    	Comparable mergedNumbers [] = new Comparable[mergedSize]; 
    	// Position to insert merged number
    	int mergePos = 0;
    	// Position of elements in left partition                 
    	int leftPos = 0;
    	// Position of elements in right partition                  
    	int rightPos = 0;    

    	// Initialize left partition position
    	leftPos = start_index;
    	// Initialize right partition position                      
    	rightPos = midpoint + 1;                 

    	// Add smallest element from left or right partition to merged arr
    	while (leftPos <= midpoint && rightPos <= end_index) {
    		if (arr[leftPos].compareTo(arr[rightPos]) < 0) {
    			mergedNumbers[mergePos] = arr[leftPos];
    			++leftPos;
       		} 
    		else {
    			mergedNumbers[mergePos] = arr[rightPos];
    			++rightPos;
    		}
   			++mergePos;
		}

		// If left partition is not empty, add remaining elements to merged arr
		while (leftPos <= midpoint) {
		mergedNumbers[mergePos] = arr[leftPos];
		++leftPos;
		++mergePos;
		}

		// If right partition is not empty, add remaining elements to merged arr
		while (rightPos <= end_index) {
			mergedNumbers[mergePos] = arr[rightPos];
			++rightPos;
			++mergePos;
		}

		// Copy merge number back to arr
		for (mergePos = 0; mergePos < mergedSize; ++mergePos) {
			arr[start_index + mergePos] = mergedNumbers[mergePos];
		}
	}

	public void mergeSort(Comparable[] arr, int start_index, int end_index) {
		int midpoint = 0;
		// if partition is shorter than the baseLen
		// do O(n^2) sorting
		if (end_index - start_index + 1 <= baseLen) {
			Proj01_InsertionSort sort_partition = new Proj01_InsertionSort(false);
			// copy the partition into a small buffer
			Comparable[] partition = new Comparable[end_index - start_index + 1];
			partition = Arrays.copyOfRange(arr, start_index, end_index + 1);
			// do insertion sort
			sort_partition.insertionSort(partition, 0, partition.length);
			// copy the partition back into the array
			int i, j;
			for (i = start_index, j = 0; j < partition.length; i++, j++) {
				arr[i] = partition[j];
			}
			return;
		}

		if (start_index < end_index) {
			// Find the midpoint in the partition
			midpoint = (start_index + end_index) / 2;
			
			// Recursively sort left and right partitions
			mergeSort(arr, start_index, midpoint);
			mergeSort(arr, midpoint + 1, end_index);

			if (debug) {
				System.out.print("Merging:     ");
				// print the not in bracket heading (if exists)
				debug_printer(arr, -1, 0, start_index - 1);
				// print merge partition 1
				debug_printer(arr, 1, start_index, midpoint);
				// print merge partition 2
				debug_printer(arr, 1, midpoint + 1, end_index);
				// print the not in bracket tail (if exists)
				debug_printer(arr, -1, end_index + 1, arr.length - 1);
				System.out.println();
			}

			// Merge left and right partition in sorted order
			merge(arr, start_index, midpoint, end_index);
			if (debug) {
				System.out.print("After merge: ");
				// print the not in bracket heading (if exists)
				debug_printer(arr, -1, 0, start_index - 1);
				// print the merged result
				debug_printer(arr, 1, start_index, end_index);
				// print the not in bracket tail (if exists)
				debug_printer(arr, -1, end_index + 1, arr.length - 1);
				System.out.println();
				System.out.println();
			}
		}
	}

	public void sort(Comparable[] arr) {
		if (debug) {
			System.out.print("Before sort: ");
			debug_printer(arr, -1, 0, arr.length - 1);
			System.out.println();
			System.out.println();
		}
		mergeSort(arr, 0, arr.length - 1);
		if (debug) {
			System.out.print("After sort:  ");
			debug_printer(arr, -1, 0, arr.length - 1);
			System.out.println();
		}
	}

	private void debug_printer(Comparable[] arr, int bracket, int start_index, int end_index)
	{
		// doest print debug message when index is invalid
		if (start_index > end_index) {
			return;
		}
		int i;
		// print to be sorted or remaining part
		if (bracket > 0) {
			System.out.print("[ ");
			for (i = start_index; i < end_index + 1; i++) {
				System.out.print(arr[i] + " ");
			}
			System.out.print("] ");
		} else {
			for (i = start_index; i < end_index + 1; i++) {
				System.out.print(arr[i] + " ");
			}
		}
	}
}