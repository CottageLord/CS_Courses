import java.io.PrintWriter;

public class test {
	public static void main(String[] args) {
		Integer[] arr = {2, 1, 7, 4, 5, 3, 6, 0};
		//Proj02_MaxHeap heap = new Proj02_MaxHeap(false);
		//System.out.print(1/2);

		Proj02_MaxHeap heap = new Proj02_MaxHeap(true, arr);
		//Proj02_HeapSort heap = new Proj02_HeapSort(false);
		//heap.sort(arr);
		heap.build_max_heap();
		//heap.printer();
		PrintWriter printWriter = new PrintWriter(System.out);
		heap.dump(printWriter);
		//heap.build_max_heap();
		//heap.printer();
		/*
		for (int i = 0; i < arr.length; i++) {
			//System.out.print(arr[i]);
			heap.insert(arr[i]);
			//heap.printer();
		}
		heap.printer();
		
		while(heap.removeMax() != null) {
			heap.printer();
		}
		*/
	}
}