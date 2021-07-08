/* Proj05_Dijkstra_student
 *
 * author: Yang Hu
 *
 * Class of the Dijkstra algorithm. It implements the Dijkstra algorithm
 * to find the shortest path to each node on graph
 */

import java.util.Iterator;
import java.util.HashMap; 
import java.util.Map; 
import java.io.*;

public class Proj05_Dijkstra_student implements Proj05_Dijkstra{

	private boolean debugFlag = false;
	private boolean isDiagraph = false;
	private HashMap<String, GraphNode> nodes;
	private GraphHeap priorityMin;
	private int dotFileCount;
	private String filename;

	/*
	 * constructor, initialize needed data structure
	 */
	public Proj05_Dijkstra_student(boolean isDiag) {
		isDiagraph = isDiag;
		// String name : node
		nodes = new HashMap<>();
		// will be used in run
		priorityMin = new GraphHeap();
		dotFileCount = 0;
		filename = "dijkstra_student_debug";
	}

	/*
	 * add a node to the graph
	 */
	public void addNode(String name){
		// this project assumes that all keys passed in
		// are unique. Therefore no duplication check
		// i.e. : if(nodes.get(s)) != null
		GraphNode newNode = new GraphNode(name);
		// add a path to itself
		// newNode.addEdge(0, newNode);
		// add to the map
		nodes.put(name, newNode);
	}
	/*
	 * add a edge to the graph
	 */
	public void addEdge(String from, String to, int weight){
		// If the graph is not directed
		// consider the edge twice (back-and-forth)
		GraphNode fromNode = nodes.get(from);
		GraphNode toNode = nodes.get(to);
		fromNode.addEdge(weight, toNode);
		if(!isDiagraph) toNode.addEdge(weight, fromNode);
	}
	/*
	 * run Dijkstra algorithm
	 * the determined path will be recorded by nodes' parents
	 */
	public void runDijkstra(String startNodeName){
		//printMap();
		/* Initialization
		 * start from the start point
		 * gradually insert/pop the queue
		 */
	   	GraphNode currNode = nodes.get(startNodeName);
	   	currNode.setDistance(0);
	   	GraphHeap neighbors;
	   	// the edge from currNode to neighbor node
	   	GraphEdge neighbor, tempEdge;
	   	// the temporary distance
	   	int distance;
	   	// while the queue is not empty
	   	while(currNode != null) {
	   		if(!currNode.isVisited()){
		   		neighbors = currNode.getEdges();
		   		// get the shortest distance
		   		neighbor = neighbors.removeMin();
		   		while(neighbor != null) {
		   			// the temp distance = current distance + edge weight to neighbor
		   			distance = currNode.getDistance() + neighbor.getWeight();
		   			// the geodesic distance to neighbor node
		   			int neighborDistance = neighbor.getTo().getDistance();
		   			// if the neighbor is not visited or the current distance is shorter
		   			if(neighborDistance < 0 || distance < neighborDistance) {
		   				// update neight bor distance
		   				neighbor.getTo().setDistance(distance);
		   				neighbor.getTo().setParent(currNode);
		   				priorityMin.insert(new GraphEdge(distance, neighbor.getTo(), null));
		   			}
		   			// go to next neighbor
		   			neighbor = neighbors.removeMin();
		   		}
		   		// check another path until the whole map is walked through
	   			currNode.visit();
		   	}

	   		tempEdge = priorityMin.removeMin();
	   		if(tempEdge == null) break;
	   		currNode = tempEdge.getFrom();
	   	}
	}
	/*
	 * print Dijkstra Results
	 * Iterate through all nodes, print out infos
	 */
	public void printDijkstraResults(String startNodeName){
		GraphNode currNode;
		// iterate through the hashMap
		Iterator it = nodes.entrySet().iterator();
	    while (it.hasNext()) {
	        Map.Entry pair = (Map.Entry)it.next();
	        // for each node, print out relative path / distance
	        currNode = nodes.get(pair.getKey());

	        System.out.printf("%s -> %s: ",startNodeName, currNode.getName());
	        if(currNode.getDistance() >= 0) {
	        	System.out.printf("best %d:", currNode.getDistance());
	        	printParents(currNode);
	        	System.out.print("\n");
	        } else {
	        	System.out.print("NO PATH\n");
	        }
	        //it.remove(); // avoids a ConcurrentModificationException
	    }
	}
	/*
	 * print Parents
	 * print out all parents of a single node recursively
	 */
	public void printParents(GraphNode currNode){
		// BASE CASE: if reach the root, return
		if(currNode.getParent() == null) {
			System.out.print(" " + currNode.getName());
			return;
		}
		// recurse in until each the root
		printParents(currNode.getParent());
		// print out current node afterward
		System.out.print(" " + currNode.getName());
	}
	/*
	 * write Solution DotFile
	 * generate a dot file according to the progress
	 */
	public void writeSolutionDotFile(){
		try{
			FileWriter writer = new FileWriter(filename + "_" + dotFileCount + ".dot");
			dotFileCount++;
			PrintWriter printWriter = new PrintWriter(writer);
			if(isDiagraph) {
				printWriter.printf("digraph\n{\n");
			} else {
				printWriter.printf("graph\n{\n");
			}
			// write all vertices
			nodePrinter(printWriter);
			// write all edges
			edgePrinter(printWriter);

			printWriter.printf("}\n");
			printWriter.close();
		} catch (IOException e) {
			System.err.println("cannot generate .dot file");
		}
	}
	/*
	 * node Printer
	 * print out all nodes in the graph to the dot file
	 */
	private void nodePrinter(PrintWriter printWriter) {
		
		// iterate through the map
	    Iterator it = nodes.entrySet().iterator();
	    while (it.hasNext()) {
	        Map.Entry pair = (Map.Entry)it.next();
	        // if the node has been included into a shortest path
	        // highlight it
	        GraphNode tempNode = nodes.get(pair.getKey());
	        if(tempNode.getDistance() < 0){
	        	printWriter.printf("%s [label=\"%s\"];\n", 
	        		tempNode.getName(),
	        		tempNode.getName());
	        } else {
	        	printWriter.printf("%s [label=\"%s\\n%d\" penwidth=4];\n",
	        		tempNode.getName(),
	        		tempNode.getName(),
	        		tempNode.getDistance());
	        }

	        //it.remove(); // avoids a ConcurrentModificationException
	    }
	}
	/*
	 * edge Printer
	 * print out all edges in the graph to the dot file
	 */
	private void edgePrinter(PrintWriter printWriter) {
		// iterate through the map
	    Iterator it = nodes.entrySet().iterator();
	    while (it.hasNext()) {
	    	Map.Entry pair = (Map.Entry)it.next();

	        GraphHeap edges = ((GraphNode)pair.getValue()).getEdges();
	        // restore the elements that was removed
	        edges.restoreLength();

	        GraphEdge currEdge = edges.removeMin();
	        //System.out.println(currEdge);
	        while(currEdge != null) {
	        	// only consider single side
	        	GraphNode from = currEdge.getFrom();
	        	GraphNode to = currEdge.getTo();
	        	// if the edge is a undirected edge
	        	// the printer will only print one way
	        	// for example: A -> E, but not E -> A
	        	String arrow = "--";
	        	if(isDiagraph) {
	        		arrow = "->";
	        	}
	        	if(from.getName().compareTo(to.getName()) < 0) {
	        		printWriter.printf("%s %s %s [label=%d", 
						from.getName(),
						arrow,
						to.getName(),
						currEdge.getWeight());
	        		if((to.getParent() != null && to.getParent().getName().compareTo(from.getName()) == 0) || 
	        			(from.getParent() != null && from.getParent().getName().compareTo(to.getName()) == 0)) {
	        			printWriter.print(" penwidth=4 color=red");
	        		} 
	        		printWriter.print("]\n");
	        	}
	        	currEdge = edges.removeMin();
	        }
	        it.remove(); // avoids a ConcurrentModificationException
	    }
	}
}