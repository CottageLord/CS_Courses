/**
 * Implementation of Heap Sort algorithm
 *
 * @author Yang Hu
 *
 */
public class Proj02_HeapSort implements Proj01_Sort {

	private boolean debug;
	public Proj02_HeapSort(boolean debug_flag) {
		debug = debug_flag;
	}
	/**
    * This method calls max heap builder. Then sort the whole heap.
    * @param arr the to be sorted array
    * @return nothing
    */
	public void sort(Comparable[] arr) {
		Proj02_MaxHeap heap = new Proj02_MaxHeap(false, arr);

		for (int i = 0; i < arr.length; i++) {
			heap.removeMax();
		}
	}
}