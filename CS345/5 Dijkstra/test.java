
public class test {
	public static void main(String[] args) {
		GraphHeap heap = new GraphHeap();
		heap.insert(new GraphEdge(1, null, null));
		heap.insert(new GraphEdge(3, null, null));
		heap.insert(new GraphEdge(8, null, null));
		heap.insert(new GraphEdge(6, null, null));
		heap.insert(new GraphEdge(4, null, null));
		heap.insert(new GraphEdge(7, null, null));
		heap.insert(new GraphEdge(5, null, null));
		heap.printer();
		System.out.println(heap.removeMin().getWeight());
		heap.printer();
		heap.restoreLength();
		heap.printer();
	}
}