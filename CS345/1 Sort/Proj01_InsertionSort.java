/* Implementation of Insertion Sort algorithm
 *
 * Author: Yang Hu
 */
public class Proj01_InsertionSort implements Proj01_Sort
{
	private Boolean debug;

	public Proj01_InsertionSort(Boolean debug_flag)
	{
		debug = debug_flag;
	}

	public void insertionSort(Comparable[] arr, int start_index, int end_index)
	{
		int i, j;
		int length = end_index - start_index;
		// print out the original array
		if(debug) {
			System.out.print("Before sort: ");
			debug_printer(arr, length, -1, 0);
			System.out.println();
		}
		
		// iterate through array
		for (i = start_index + 1; i < end_index; i++) {
			j = i;
			// First:  find the smallest member
			// Second: bubble it to the proper prosition in the sorted
			while(j > start_index && arr[j].compareTo(arr[j - 1]) < 0) {
				// show swaping information
				if(debug) {
					System.out.print("Swap:        ");
					debug_printer(arr, length, j - 1, -1);
				}
				swap(arr, j, j - 1);
				j--;
			}
			// show the boundary change after swaping
			if(debug && j != i) {
				System.out.print("After swap : ");
				debug_printer(arr, length, -1, i - 1);
				System.out.println();
			}
		}
		if(debug) {
			System.out.print("After sort : ");
			debug_printer(arr, length, -1, length - 1);
		}
	}
	// call the many-parameter sorting method
	public void sort(Comparable[] arr)
	{
		insertionSort(arr, 0, arr.length);
	}

	private void swap(Comparable[] arr, int index_1, int index_2) 
	{
		Comparable temp;

		temp = arr[index_1];
	    arr[index_1] = arr[index_2];
	    arr[index_2] = temp;
	}
	

	// index is the index of bubbling pair
	// sorted is the boundary between sorted and unsorted
	private void debug_printer(Comparable[] arr, int length, int index, int sorted)
	{
		int i;
		for (i = 0; i < length; i++) {
			if (i == sorted) {
				System.out.print(arr[i] + " | ");
				continue;
			}
			if(i == index) {
				System.out.print("(" + arr[i] + ")-(" + arr[i + 1] + ") ");
				i++;
			} else {
				System.out.print(arr[i] + " ");
			}
		}
		System.out.println();
	}
}