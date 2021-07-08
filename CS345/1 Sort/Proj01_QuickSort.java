/* Implementation of Quick Sort algorithm
 *
 * Author: Yang Hu
 */
public class Proj01_QuickSort implements Proj01_Sort
{
	private Boolean debug;
	private int mode;
	private int baseLen;
	
	public Proj01_QuickSort(Boolean debug_flag, int mode_flag, int length)
	{
		debug = debug_flag;
		mode = mode_flag;
		baseLen = length;
	}

	public void sort(Comparable[] arr) {
		if (debug) {
			System.out.print("Before sort: ");
			debug_printer(arr, -1, 0, arr.length);
			System.out.println();
		}

		quicksort(arr, 0, arr.length - 1);


		if (debug) {
			System.out.print("\nAfter sort:  ");
			debug_printer(arr, -1, 0, arr.length);
			System.out.println();
		}
	}

	private int choose_pivot(Comparable[] arr, int start_index, int end_index) {
		int midpoint = start_index + (end_index - start_index) / 2;
		/* Pick pivot */
		switch(mode) {
			case 0:
				midpoint = start_index;
				break;
			case 1:
				// midpoint unchanged
				break;
			case 2:
				Comparable a, b, c;
				a = arr[start_index];
				b = arr[midpoint];
				c = arr[end_index];
				if ( ((a.compareTo(b) >= 0) && (a.compareTo(c) <= 0)) || 
					(a.compareTo(b) <= 0 && a.compareTo(c) >= 0) ) {
					// a is the median
					midpoint = start_index;
				} else if ( ((b.compareTo(a) >= 0) && (b.compareTo(c) <= 0)) || 
					(b.compareTo(c) >= 0 && b.compareTo(a) <= 0) ) {
					// b is the median
					// midpoint unchanged
				} else {
					// c is the median
					midpoint = end_index;
				}
				break;
			default: 
				// midpoint unchanged
		}
		return midpoint;
	}
	public int partition(Comparable[] arr, int start_index, int end_index) {
		int l = 0;
		int h = 0;
		int midpoint = 0;
		Comparable pivot = 0;
		Comparable temp = 0;
		boolean done = false;

		midpoint = choose_pivot(arr, start_index, end_index);
		
		pivot = arr[midpoint];

		l = start_index;
		h = end_index;

		while (!done) {

			/* Increment l while arr[l] < pivot */
			while (arr[l].compareTo(pivot) < 0) {
				++l;
			}

			/* Decrement h while pivot < arr[h] */
			while (pivot.compareTo(arr[h])  < 0) {
				--h;
			}

			/* If there are zero or one items remaining,
			all arr are partitioned. Return h */
			if (l >= h) {
				done = true;
			} else {
				/* Swap arr[l] and arr[h],
				update l and h */
				temp = arr[l];
				arr[l] = arr[h];
				arr[h] = temp;

				++l;
				--h;
			}
		}
		return h;
	}

	public void quicksort(Comparable[] arr, int start_index, int end_index) {
		// if the partition is shorter than the baseLen
		// sort the partition in place
		if (end_index - start_index + 1 <= baseLen) {

			Proj01_InsertionSort sort_partition = new Proj01_InsertionSort(false);
			sort_partition.insertionSort(arr, start_index, end_index + 1);
			return;
		}

		int midpoint = 0;

		/* Base case: If there are 1 or zero entries to sort,
		partition is already sorted */
		if (start_index >= end_index) {
			return;
		}

		/* Partition the data within the array. Value midpoint returned
		from partitioning is location of last item in low partition. */
		midpoint = partition(arr, start_index, end_index);

		if (debug) {
			System.out.println("\nChoose pivot arr[" + midpoint + "] = " + arr[midpoint]);
			debug_printer(arr, -1, 0, start_index);
			debug_printer(arr, 1, start_index, midpoint + 1);
			debug_printer(arr, 1, midpoint + 1, end_index + 1);
			debug_printer(arr, -1, end_index + 1, arr.length);
			
			System.out.println();
		}

		/* Recursively sort low partition (start_index to midpoint) and
		high partition (midpoint + 1 to end_index) */
		if (debug) {
			System.out.print("\nSort partitions:  ");
			debug_printer(arr, 1, start_index, midpoint + 1);
		}
		quicksort(arr, start_index, midpoint);

		if (debug) {
			System.out.print("\nSort partitions:  ");
			debug_printer(arr, 1, midpoint + 1, end_index + 1);
		}
		quicksort(arr, midpoint + 1, end_index);

		if (debug) {
			System.out.print("\nPartition sorted ");
			debug_printer(arr, -1, 0, start_index);
			debug_printer(arr, 1, start_index, midpoint + 1);
			debug_printer(arr, 1, midpoint + 1, end_index + 1);
			debug_printer(arr, -1, end_index + 1, arr.length);
			
			System.out.println();
		}

		return;
	}


	private void debug_printer(Comparable[] arr, int bracket, int start_index, int end_index)
	{
		// doest print debug message when index is invalid
		if (start_index > end_index) {
			return;
		}
		int i;
		// print sorting-now [partition]s
		// or going-to-be-sorted parts (the array content outside the sorting area)
		if (bracket > 0) {
			System.out.print("[ ");
			for (i = start_index; i < end_index; i++) {
				System.out.print(arr[i] + " ");
			}
			System.out.print("] ");
		} else {
			for (i = start_index; i < end_index; i++) {
				System.out.print(arr[i] + " ");
			}
		}
	}
}