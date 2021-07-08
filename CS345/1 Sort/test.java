public class test {
	public static void main(String[] args) {

		Integer[] arr = {2, 1, 5, 4, 7, 6, 3, 10, 9, 8};

		Proj01_BubbleSort test_sort_1 = new Proj01_BubbleSort(true);
		
		Proj01_SelectionSort test_sort_2 = new Proj01_SelectionSort(true);

		Proj01_InsertionSort test_sort_3 = new Proj01_InsertionSort(true);

		Proj01_MergeSort test_sort_4 = new Proj01_MergeSort(true, 3);

		Proj01_QuickSort test_sort_5 = new Proj01_QuickSort(true, 1, 3);

		System.out.println("-------BubbleSort--------");
		reset_array(arr);
		test_sort_1.sort(arr);

		System.out.println("------SelectionSort-------");
		reset_array(arr);
		test_sort_2.sort(arr);

		System.out.println("------InsertionSort-----");
		reset_array(arr);
		test_sort_3.sort(arr);

		System.out.println("-------MergeSort--------");
		reset_array(arr);
		test_sort_4.sort(arr);

		System.out.println("-------QuickSort--------");
		reset_array(arr);
		test_sort_5.sort(arr);
	}

	private static void reset_array(Comparable[] arr) {
		Integer[] reset = {2, 1, 5, 4, 7, 6, 3, 10, 9, 8};
		for (int i = 0; i < reset.length; i++) {
			arr[i] = reset[i];
		}
	}
}