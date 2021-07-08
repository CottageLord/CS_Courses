public interface Proj05_Dijkstra
{
	void addNode(String s);
	void addEdge(String from, String to, int weight);

	void runDijkstra(String startNodeName);
	void printDijkstraResults(String startNodeName);
	void writeSolutionDotFile();
}

