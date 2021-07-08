/* GraphNode
 *
 * author: Yang Hu
 *
 * hold the info of a single node
 */

import java.util.HashMap; 
import java.util.Map; 

public class GraphNode {

	private boolean visited = false;
	private GraphHeap edges = new GraphHeap();
	private String name;
	private int distance;
	private GraphNode parent;

	public GraphNode(String givenName) {
		name = givenName;
		distance = -1;
		parent = null;
	}

	public void addEdge(int weight, GraphNode to) {
		// min heap
		edges.insert(new GraphEdge(weight, this, to));
	}

	public void setDistance(int dist) {
		distance = dist;
	}

	public void setParent(GraphNode givenParent) {
		parent = givenParent;
	}
	public int getDistance() {
		return distance;
	}

	public String getName() {
		return name;
	}

	public GraphHeap getEdges() {
		return edges;
	}

	public GraphNode getParent() {
		return parent;
	}

	public boolean isVisited() {
		return visited;
	}

	public void visit() {
		visited = true;
	}
}