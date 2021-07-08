/* GraphNode
 *
 * author: Yang Hu
 *
 * hold the info of a single edge
 */

public class GraphEdge {

	private int weight;
	private GraphNode from;
	private GraphNode to;

	public GraphEdge(int givenWeight, GraphNode fromNode, GraphNode toNode) {
		weight 	= givenWeight;
		from 	= fromNode;
		to 		= toNode;
	}

	public GraphNode getFrom () {
		return from;
	}

	public GraphNode getTo () {
		return to;
	}

	public int getWeight() {
		return weight;
	}

}