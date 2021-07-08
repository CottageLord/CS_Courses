/* Implementation of Selection Sort algorithm
 *
 * Author: Yang Hu
 */
public class Proj01_SelectionSort implements Proj01_Sort
{
	private Boolean debug;

	public Proj01_SelectionSort(Boolean debug_flag)
	{
		debug = debug_flag;
	}

	public void sort(Comparable[] arr)
	{
		int i, j;
		int smallest = 0;
		Comparable temp;
		int length = arr.length;
		// print out the original array
		if(debug) {
			System.out.print("Before sort:   ");
			debug_printer(arr, length, 0, -1, -1);
			System.out.println();
		}		
		// iterate through array
		for (i = 0; i < length; i++) {
			smallest = i;
			// First step: find the smallest member in the unsorted
			for (j = i + 1; j < length; j++) {
				if (arr[j].compareTo(arr[smallest]) < 0) {
					smallest = j;
				}
			}

			if(debug) {
				System.out.print("Find smallest: ");
				debug_printer(arr, length, i, smallest, i);
			}	
			// move (swap) the smallest to
			// the right most position of the sorted part
			// a b c| f e (d)
			// a b c (d)| e (f)

			swap(arr, i, smallest);

			if(debug) {
				System.out.print("After swap   : ");
				debug_printer(arr, length, i + 1, i, smallest);
				System.out.println();
			}
		}
	}

	private void swap(Comparable[] arr, int index_1, int index_2) 
	{
		Comparable temp;

		temp = arr[index_1];
	    arr[index_1] = arr[index_2];
	    arr[index_2] = temp;
	}

	private void debug_printer(Comparable[] arr, int length, 
		int sorted, int smallest, int swap)
	{
		int i;
		for (i = 0; i < length; i++) {
			// if the number to swap is right after the sorted part
			// the flag "|" need to print with "()"
			if(i == sorted && i == swap) {
				System.out.print("|(" + arr[i] + ") ");
				continue;
			}else if(i == sorted) {
				System.out.print("|" + arr[i] + " ");
				continue;
			}
			// print normal swap members
			if(i == smallest || i == swap) {
				System.out.print("(" + arr[i] + ") ");
			} else {
				// print normal members
				System.out.print(arr[i] + " ");
			}
		}
		System.out.println();
	}
}