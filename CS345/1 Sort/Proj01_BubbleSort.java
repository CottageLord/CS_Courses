/* Implementation of Bubble Sort algorithm
 *
 * Author: Yang Hu
 */
import java.util.Arrays;

public class Proj01_BubbleSort implements Proj01_Sort
{
	private Boolean debug;

	public Proj01_BubbleSort(Boolean debug_flag)
	{
		debug = debug_flag;
	}

	public void sort(Comparable[] arr)
	{
		int i, j;
		Comparable temp;
		int length = arr.length - 1;
		// bubble index = -1 means no parentheses will
		// be printed, only the original array
		if(debug) {
			debug_printer(arr, length, -1);
		}		

		for (i = 0; i < length; i++) {
			
			for (j = 0; j < length - i; j++) {

				// if there are unsorted pairs
				if (arr[j].compareTo(arr[j + 1]) > 0) {
					if(debug) {
						debug_printer(arr, length, j);
					}
					// swap
					swap(arr, j, j + 1);
					// Print the array
					// bubbling pairs will be in parentheses
					if(debug) {
						debug_printer(arr, length, j);
						System.out.println();
					}	
				}
			}
		}
	}

	// swap two elements in the array
	private void swap(Comparable[] arr, int index_1, int index_2) 
	{
		Comparable temp;

		temp = arr[index_1];
	    arr[index_1] = arr[index_2];
	    arr[index_2] = temp;
	}

	// length is arr.length
	// index is the index of bubbling pairs
	// 		(a)-(b)	-> (b)-(a)
	private void debug_printer(Comparable[] arr, int length, int index)
	{
		int i;
		for (i = 0; i <= length; i++) {
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