/**
 * Proj03_SimpleBST_student
 *
 * This class implements a basic Binary Tree Structure
 * which contains basic set/get/remove, print out in-order/post-order
 * and implement left/right rotation on a single node
 * it generate .dot file for debugging
 *
 * @author Yang Hu
 */

import java.io.*;

public class Proj03_SimpleBST_student<K extends Comparable<K>, V>
             implements Proj03_BST<K,V>
{
	private Proj03_BSTNode<K, V> root;
	private String filename;
	private int dotFileCount;
	public Proj03_SimpleBST_student(String debugStr)
	{
		filename = debugStr;
		dotFileCount = 0;
	}


	/* void set(K,V)
	 *
	 * Inserts a given (key,value) pair into the tree.
	 *
	 * This call may assume that both the key and value are non-null.
	 *
	 * If the key already exists, then update the stored value to the new
	 * value;
	 */
	public void set(K key, V value)
	{
		/* we use a recursive implementation, in the x=change(x)
		 * style, for most methods in this class.
		 */
		int countIncre = 0;
		if(get(key) == null) countIncre = 1;
		root = set_helper(root, key, value, countIncre);
	}

	private Proj03_BSTNode<K,V> set_helper(Proj03_BSTNode<K,V> oldRoot,
	                                       K key, V value, int countIncre)
	{
		// base case: new key
		if(oldRoot == null) {
			return new Proj03_BSTNode<K,V>(key, value);
		}
		// traverse through the tree until find
		// the proper position to set
		if(oldRoot.key.compareTo(key) > 0) {
			// key is smaller than current
			// go to the left
			oldRoot.count += countIncre;
			oldRoot.left = set_helper(oldRoot.left, key, value, countIncre);
		} else if(oldRoot.key.compareTo(key) < 0) {
			// key is larger than current
			// go to the right
			oldRoot.count += countIncre;
			oldRoot.right = set_helper(oldRoot.right, key, value, countIncre);
		} else {
			// update existing value
			oldRoot.value = value;
		}
		return oldRoot;
	}

	/* V get(K)
	 *
	 * Searches the tree for a given key; returns the associated value
	 * if the key is found, or null if it is not found.
	 */
	public V get(K key){

		Proj03_BSTNode<K, V> currNode = root;

		while(currNode != null) {
			// traverse through the tree
			// until find the matched key
			// or return null eventually
			if(currNode.key.compareTo(key) > 0) {
				currNode = currNode.left;
			} else if (currNode.key.compareTo(key) < 0) {
				currNode = currNode.right;
			} else {
				return currNode.value;
			}
		}

		return null;
	}


	/* int getSize()
	 *
	 * Returns the number of keys stored in the tree.  This *MUST* run in
	 * O(1) time.
	 */
	public int getSize()
	{
		return getCount(root);
	}
	private int getCount(Proj03_BSTNode<K,V> node)
	{
		if (node == null)
			return 0;
		return node.count;
	}

	/* void remove(K)
	 *
	 * Removes a given node from the tree, found by the key.
	 *
	 * This is a NOP if the key does not exist.
	 */
	public void remove(K key) {
		if(get(key) == null) return;
		root = removeHelper(root, key);
	}

	/* Proj03_BSTNode<K,V> removeHelper(Proj03_BSTNode<K,V>, K)
	 *
	 * recursively call the remove function. Do operations in 3 cases
	 */
	private Proj03_BSTNode<K,V> removeHelper(Proj03_BSTNode<K,V> currNode, K key) {

		// update the count at each related level
		currNode.count--;
		// locate the to be removed node
		if(currNode.key.compareTo(key) > 0) {
			currNode.left = removeHelper(currNode.left, key);
			return currNode;
		}
		if(currNode.key.compareTo(key) < 0) {
			currNode.right = removeHelper(currNode.right, key);
			return currNode;
		}
		// case 1: removing a leaf
		if (currNode.left == null && currNode.right == null) {
			return null;
		}
		// case 2: removing node with single child
		if(currNode.left == null || currNode.right == null) {
			if(currNode.left == null) return currNode.right;
			if(currNode.right == null) return currNode.left;
		}

		// case 3: removing node with 2 children
		Proj03_BSTNode<K,V> predecessor = findPredecessor(currNode);
		copyNodeContent(predecessor, currNode);
		currNode.left = removeHelper(currNode.left, predecessor.key);
		return currNode;
	}

	/* void copyNodeContent(Proj03_BSTNode<K,V>, Proj03_BSTNode<K,V>)
	 *
	 * copy key/value from one node to another
	 */
	private void copyNodeContent(Proj03_BSTNode<K,V> from, Proj03_BSTNode<K,V> to) {
		to.key = from.key;
		to.value = from.value;
	}

	/* Proj03_BSTNode<K,V> copyNodeContent(Proj03_BSTNode<K,V>)
	 *
	 * find and return the predecessor of the current node
	 */
	private Proj03_BSTNode<K,V> findPredecessor(Proj03_BSTNode<K,V> currNode) {

		Proj03_BSTNode<K,V> tempNode = currNode.left;

		if(tempNode == null) return null;

		while(tempNode.right != null) {
			tempNode = tempNode.right;
		}

		return tempNode;
	}

	/* void rotateLeft(K)
	 *
	 * Searches the tree for a given key, and then performs a rotate left
	 * at that location.  A "rotate left" means that the selected node
	 * moves down, and the right child of that node moves up to be the new
	 * root.
	 *
	 * If the key does not exist, or if it doesn't have a right child,
	 * then this is a NOP.
	 */
	public void rotateLeft(K at){
		if(get(at) == null) return;
		root = rotateLeftHelper(root, at);
	}

	/* Proj03_BSTNode<K,V> rotateLeftHelper(Proj03_BSTNode<K,V>, K)
	 *
	 * recursively call the rotateLeftHelper function. update the counts as well
	 */
	private Proj03_BSTNode<K,V> rotateLeftHelper(Proj03_BSTNode<K,V> currNode, K key) {
		// locate target node
		if(currNode.key.compareTo(key) > 0) {
			currNode.left = rotateLeftHelper(currNode.left, key);
			return currNode;
		}
		if(currNode.key.compareTo(key) < 0) {
			currNode.right = rotateLeftHelper(currNode.right, key);
			return currNode;
		}
		// NOP if right child is null
		if(currNode.right == null) return currNode;

		// rotation preperations
		Proj03_BSTNode<K,V> newRoot = currNode.right;
		Proj03_BSTNode<K,V> tempNode = newRoot.left;
		// update counts
		int countLeft = 0;
		int countRight = 0;
		
		if(tempNode != null) countRight = tempNode.count;
		if(currNode.left != null) countLeft = currNode.left.count;
		newRoot.count = currNode.count;
		currNode.count = countLeft + countRight + 1;
		// rotate
		newRoot.left = currNode;
		currNode.right = tempNode;

		return newRoot;

	}

	/* void rotateRight(K)
	 *
	 * Same as rotateLeft, except mirrored.
	 */
	public void rotateRight(K at){
		if(get(at) == null) return;
		root = rotateRightHelper(root, at);
	}

	/* Proj03_BSTNode<K,V> rotateRightHelper(Proj03_BSTNode<K,V>, K)
	 *
	 * recursively call the rotateRightHelper function. update the counts as well
	 */
	private Proj03_BSTNode<K,V> rotateRightHelper(Proj03_BSTNode<K,V> currNode, K key) {
		// locate target node
		if(currNode.key.compareTo(key) > 0) {
			currNode.left = rotateRightHelper(currNode.left, key);
			return currNode;
		}
		if(currNode.key.compareTo(key) < 0) {
			currNode.right = rotateRightHelper(currNode.right, key);
			return currNode;
		}

		// rotation preperations
		Proj03_BSTNode<K,V> newRoot = currNode.left;
		Proj03_BSTNode<K,V> tempNode = newRoot.right;
		// update counts
		int countLeft = 0;
		int countRight = 0;
		if(tempNode != null) countLeft = tempNode.count;
		if(currNode.right != null) countRight = currNode.right.count;
		newRoot.count = currNode.count;
		currNode.count = countLeft + countRight + 1;
		// rotate
		newRoot.right = currNode;
		currNode.left = tempNode;

		return newRoot;
	}

	/* void inOrder(K[], V[], int[])
	 *
	 * Performs an in-order traversal of the tree.  The caller must
	 * provide arrays to be populated - one for keys, one for values,
	 * and one for the count fields of each node.
	 *
	 */
	public void inOrder(K[] keysOut, V[] valuesOut, int[] countsOut){
		inOrderHelper(keysOut, valuesOut, countsOut, 0, root);
	}

	/* int inOrderHelper(K[], V[], int[])
	 *
	 * recursively call the inOrderHelper function. put nodes' values in arrays
	 * by in-order traversal
	 */
	private int inOrderHelper(K[] keysOut, V[] valuesOut, int[] countsOut, 
		int index, Proj03_BSTNode<K,V> currNode) {
		// base case: traverse to the left-most child
		if(currNode == null) return index;
		// append left child info
		// by traversing into it
		index = inOrderHelper(keysOut, valuesOut, countsOut, index, currNode.left);
		
		// add the current node info
		keysOut[index] = currNode.key;
		valuesOut[index] = currNode.value;
		countsOut[index] = currNode.count;
		index++;
		// append right child info
		// by traversing into it
		index = inOrderHelper(keysOut, valuesOut, countsOut, index, currNode.right);

		return index;
	}
	/* void postOrder(K[], V[])
	 *
	 * Same as inOrder(), except that it performs a post-order traversal.
	 */
	public void postOrder(K[] keysOut, V[] valuesOut, int[] countsOut){
		postOrderHelper(keysOut, valuesOut, countsOut, 0, root);
	}

	/* int postOrderHelper(K[], V[], int[])
	 *
	 * recursively call the postOrderHelper function. put nodes' values in arrays
	 * by post-order traversal
	 */
	private int postOrderHelper(K[] keysOut, V[] valuesOut, int[] countsOut, 
		int index, Proj03_BSTNode<K,V> currNode) {
		// base case: traverse to the bottom child
		if(currNode == null) return index;
		// append left child info
		// by traversing into it
		index = postOrderHelper(keysOut, valuesOut, countsOut, index, currNode.left);
		
		// append right child info
		// by traversing into it
		index = postOrderHelper(keysOut, valuesOut, countsOut, index, currNode.right);
		
		// append the current node info
		keysOut[index] = currNode.key;
		valuesOut[index] = currNode.value;
		countsOut[index] = currNode.count;
		index++;

		return index;
	}
	/* void genDebugDot()
	 *
	 * Generates a \texttt{.dot} file which represents the tree; if this
	 * is called multiple times, then they must all have different
	 * filenames.
	 *
	 * The generated files must be placed in the *current* directory, and
	 * must not include any whitespace in the name.
	 */
	public void genDebugDot()
	{
		try{
			FileWriter writer = new FileWriter(filename + "_" + dotFileCount + ".dot");
			dotFileCount++;
			PrintWriter printWriter = new PrintWriter(writer);
			printWriter.printf("digraph\n{\n");

			if(root == null) {
				printWriter.printf("root [label=\"tree is empty\" shape=rect]\n");
			} else {
				dotGenerator(printWriter, root);
			}

			printWriter.printf("}\n");
			printWriter.close();
		} catch (IOException e) {
			System.err.println("cannot generate .dot file");
		}
	}

	/* void dotGenerator(PrintWriter, Proj03_BSTNode<K,V>)
	 *
	 * recursively add dot file contents
	 * 1. define current node
	 * 2. define arrows beneath
	 * 3. recurse into children
	 */
	private void dotGenerator(PrintWriter printWriter, 
		Proj03_BSTNode<K,V> currNode) {
		// step 1: define curr node
		String currName = "node_" + currNode.key;
		printWriter.printf("%s [label=\"%s\\n%s\\ncount=%d\"];\n",
			currName, currNode.key, currNode.value, currNode.count);
		// step 2: define left/right child
		// 		case 1: leaf node, nothing to print
		if(currNode.left == null && currNode.right == null) return;
		// 		case 2: only one child
		if(currNode.left == null || currNode.right == null) {
			String childName;
			// print empty
			// if left is empty, print empty left and valid right
			if(currNode.left == null) {
				childName = currName + "_L";
				String rightChildName = "node_" + currNode.right.key;

				printInvis(printWriter, currName, childName);
				printValid(printWriter, currName, rightChildName, "R", "se");
				// go into valid right
				dotGenerator(printWriter, currNode.right);
				return;
			}
			// if right is empty, print empty right and valid left
			if(currNode.right == null) {
				childName = "node_" + currNode.key + "_R";
				String leftChildName = "node_" + currNode.left.key;

				printInvis(printWriter, currName, childName);
				printValid(printWriter, currName, leftChildName, "R", "se");
				// go into valid left
				dotGenerator(printWriter, currNode.left);
				return;
			}
		}
		// 		case 3: two valid children
		if(currNode.left != null && currNode.right != null) {
			String leftChildName = "node_" + currNode.left.key;
			String rightChildName = "node_" + currNode.right.key;

			printValid(printWriter, currName, leftChildName, "L", "sw");
			printValid(printWriter, currName, rightChildName, "R", "se");
			// go into valid left
			dotGenerator(printWriter, currNode.left);
			// go into valid right
			dotGenerator(printWriter, currNode.right);
		}
	}
	/* void printInvis(PrintWriter, String)
	 *
	 * print one line in dot file which
	 * represents an invisible arrow between current node and a
	 * null child
	 */
	private void printInvis(PrintWriter printWriter, String currName, 
		String childName) {

		printWriter.printf("%s -> %s [style=invis];\n", currName, childName);
		printWriter.printf("%s [style=invis];\n", childName);
	}
	/* void printValid(PrintWriter, String, String, String, String)
	 *
	 * print one line in dot file which
	 * represents an arrow between current node and a
	 * valid child
	 */
	private void printValid(PrintWriter printWriter, String currName, 
		String childName, String label, String taildir) {
		
		printWriter.printf("%s -> %s [label=%s taildir=%s];\n", 
			currName, childName, label, taildir);
	}
}

