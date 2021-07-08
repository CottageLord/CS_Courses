/* Proj04_RedBlack_student
 *
 * author: Yang Hu
 *
 * RedBlack tree main class. It implements the Top-Down insert and can
 * convert from 2-3-4 tree to a RedBlack tree
 */

import java.io.*;

public class Proj04_RedBlack_student<K extends Comparable<K>, V> implements Proj04_Dictionary<K,V> {

	private Proj04_BSTNode<K, V> root;
	private String filename;
	private int dotFileCount;
	private int size;

	/*
	 * Default constructor, build tree gradually
	 */
	public Proj04_RedBlack_student(String debugStr) {
		filename = debugStr;
		dotFileCount = 0;
	}

	/*
	 * 234 tree converter constructor, build a tree immediately
	 * according to the given 234 tree
	 */
	public Proj04_RedBlack_student(String debugStr, Proj04_234Node<K, V> givenRoot) {
		filename = debugStr;
		dotFileCount = 0;
		size = 0;
		root = convertFrom234(givenRoot);
	}

	/* Proj04_BSTNode<K, V> convertFrom234(Proj04_234Node<K, V>
	 * 
	 * A recursive function that convert 234 tree layer by layer
	 */
	private Proj04_BSTNode<K, V> convertFrom234(Proj04_234Node<K, V> currNode) {
		Proj04_BSTNode<K, V> newRoot = null;
		if(currNode == null) return null;

		size += currNode.numKeys;

		if(currNode.numKeys == 3) {
			// pull the root out of the keys
			newRoot = new Proj04_BSTNode<K, V>(currNode.key2, currNode.val2);
			Proj04_BSTNode<K, V> lChild = new Proj04_BSTNode<K, V>(currNode.key1, currNode.val1);
			Proj04_BSTNode<K, V> rChild = new Proj04_BSTNode<K, V>(currNode.key3, currNode.val3);
			newRoot.color = "b";
			lChild.color = "r";
			rChild.color = "r";
			// recurse into new children
			lChild.left = convertFrom234(currNode.child1);
			lChild.right = convertFrom234(currNode.child2);
			rChild.left = convertFrom234(currNode.child3);
			rChild.right = convertFrom234(currNode.child4);

			newRoot.left = lChild;
			newRoot.right = rChild;
		}
		if(currNode.numKeys == 2) {
			// pull the root out of the keys
			newRoot = new Proj04_BSTNode<K, V>(currNode.key2, currNode.val2);
			Proj04_BSTNode<K, V>  lChild = new Proj04_BSTNode<K, V>(currNode.key1, currNode.val1);
			newRoot.color = "b";
			lChild.color = "r";
			// recurse into new children
			lChild.left = convertFrom234(currNode.child1);
			lChild.right = convertFrom234(currNode.child2);

			newRoot.left = lChild;
			newRoot.right = convertFrom234(currNode.child3);
		}
		if(currNode.numKeys == 1) {
			// pull the root out of the keys
			newRoot = new Proj04_BSTNode<K, V>(currNode.key1, currNode.val1);
			newRoot.color = "b";
			// recurse into new children
			newRoot.left = convertFrom234(currNode.child1);
			newRoot.right = convertFrom234(currNode.child2);
		}
		return newRoot;
	}

	/* void set(K,V)
	 *
	 * Inserts a given (key,value) pair into the tree.
	 *
	 * This call may assume that both the key and value are non-null.
	 */
	@Override
	public void set(K key, V value) {
		size++;
		root = set_helper(root, key, value, 1);
		if(root != null && root.color.equals("r")) root.color = "b";
	}

	private Proj04_BSTNode<K,V> set_helper(Proj04_BSTNode<K,V> oldRoot,
	                                       K key, V value, int countIncre)
	{
		// base case: new key
		if(oldRoot == null) {
			return new Proj04_BSTNode<K,V>(key, value);
		}

		// split already fulled widget
		if(oldRoot.left != null && oldRoot.right != null &&
			oldRoot.left.color.equals("r") && oldRoot.right.color.equals("r")) {
			oldRoot.color = "r";
			oldRoot.left.color = "b";
			oldRoot.right.color = "b";
		}

		// traverse through the tree until find
		// the proper position to set
		if(oldRoot.key.compareTo(key) > 0) {
			// key is smaller than current
			oldRoot.left = set_helper(oldRoot.left, key, value, countIncre);
		} else if(oldRoot.key.compareTo(key) < 0) {
			// key is larger than current
			// go to the right
			oldRoot.right = set_helper(oldRoot.right, key, value, countIncre);
		} else {
			// update existing value
			oldRoot.value = value;
		}

		/*-----------------------
		 |		FOUR CASES
		 |Case 1: Left-Left
		 |Case 2: Right-Right
		 |Case 3: Left-Right
		 |Case 4: Right-Left
		 *----------------------*/

		// Left Left Case 
        if (oldRoot.left != null && oldRoot.left.left != null &&
        	oldRoot.left.color.equals("r") && oldRoot.left.left.color.equals("r")) {
            return rotateRight(oldRoot); 
        }
  
        // Right Right Case 
        if (oldRoot.right != null && oldRoot.right.right != null &&
        	oldRoot.right.color.equals("r") && oldRoot.right.right.color.equals("r")) {
            return rotateLeft(oldRoot); 
        }
  
        // Left Right Case 
        if (oldRoot.left != null && oldRoot.left.right != null &&
        	oldRoot.left.color.equals("r") && oldRoot.left.right.color.equals("r")) { 
            oldRoot.left = rotateLeft(oldRoot.left);
            return rotateRight(oldRoot); 
        } 
  
        // Right Left Case 
        if (oldRoot.right != null && oldRoot.right.left != null &&
        	oldRoot.right.color.equals("r") && oldRoot.right.left.color.equals("r")) { 
            oldRoot.right = rotateRight(oldRoot.right); 
            return rotateLeft(oldRoot); 
        } 
  		
			
        /* return the (unchanged) oldRoot pointer */
		return oldRoot;
	}


	/* void rotateLeft(K)
	 *
	 * Performs a rotate left at given location.  
	 * A "rotate left" means that the selected node
	 * moves down, and the right child of that node moves up to be the new
	 * root.
	 *
	 * If the key does not exist, or if it doesn't have a right child,
	 * then this is a NOP.
	 */
	public Proj04_BSTNode<K,V> rotateLeft(Proj04_BSTNode<K,V> currNode){
		// NOP if right child is null
		if(currNode == null || currNode.right == null) return currNode;
		// rotation preperations
		Proj04_BSTNode<K,V> newRoot = currNode.right;
		Proj04_BSTNode<K,V> tempNode = newRoot.left;

		// rotate
		newRoot.left = currNode;
		currNode.right = tempNode;

		newRoot.color = "b";
		newRoot.left.color = "r";
		if(newRoot.right != null) newRoot.right.color = "r";

		return newRoot;
	}

	/* void rotateRight(K)
	 *
	 * Same as rotateLeft, except mirrored.
	 */
	public Proj04_BSTNode<K,V> rotateRight(Proj04_BSTNode<K,V> currNode){
		// NOP if right child is null
		if(currNode == null || currNode.left == null) return currNode;
		// rotation preperations
		Proj04_BSTNode<K,V> newRoot = currNode.left;
		Proj04_BSTNode<K,V> tempNode = newRoot.right;

		// rotate
		newRoot.right = currNode;
		currNode.left = tempNode;

		newRoot.color = "b";
		newRoot.right.color = "r";
		if(newRoot.left != null) newRoot.left.color = "r";
		
		return newRoot;
	}
	/* V get(K)
	 *
	 * Searches the tree for a given key; returns the associated value
	 * if the key is found, or null if it is not found.
	 */
	@Override
	public V get(K key) {

		Proj04_BSTNode<K, V> currNode = root;

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

	/*
	 * A place holder for remove()
	 */
	@Override
	public void remove(K key) {
		return;
	}
	/* int getSize
	 * 
	 * returns the number of nodes in the tree
	 */
	@Override
	public int getSize() {
		return size;
	}

	/* void inOrder(K[], V[], int[])
	 *
	 * Performs an in-order traversal of the tree.  The caller must
	 * provide arrays to be populated - one for keys, one for values,
	 * and one for the count fields of each node.
	 *
	 */
	@Override
	public void inOrder(K[] keysOut, V[] valuesOut, String[] aux){
		inOrderHelper(keysOut, valuesOut, aux, 0, root);
	}

	/* int inOrderHelper(K[], V[], int[])
	 *
	 * recursively call the inOrderHelper function. put nodes' values in arrays
	 * by in-order traversal
	 */
	private int inOrderHelper(K[] keysOut, V[] valuesOut, String[] aux, 
		int index, Proj04_BSTNode<K,V> currNode) {
		// base case: traverse to the left-most child
		if(currNode == null) return index;
		// append left child info
		// by traversing into it
		index = inOrderHelper(keysOut, valuesOut, aux, index, currNode.left);
		
		// add the current node info
		keysOut[index] = currNode.key;
		valuesOut[index] = currNode.value;
		aux[index] = currNode.color;
		index++;
		// append right child info
		// by traversing into it
		index = inOrderHelper(keysOut, valuesOut, aux, index, currNode.right);

		return index;
	}
	/* void postOrder(K[], V[])
	 *
	 * Same as inOrder(), except that it performs a post-order traversal.
	 */
	@Override
	public void postOrder(K[] keysOut, V[] valuesOut, String[] aux){
		postOrderHelper(keysOut, valuesOut, aux, 0, root);
	}

	/* int postOrderHelper(K[], V[], String[])
	 *
	 * recursively call the postOrderHelper function. put nodes' values in arrays
	 * by post-order traversal
	 */
	private int postOrderHelper(K[] keysOut, V[] valuesOut, String[] aux, 
		int index, Proj04_BSTNode<K,V> currNode) {
		// base case: traverse to the bottom child
		if(currNode == null) return index;
		// append left child info
		// by traversing into it
		index = postOrderHelper(keysOut, valuesOut, aux, index, currNode.left);
		
		// append right child info
		// by traversing into it
		index = postOrderHelper(keysOut, valuesOut, aux, index, currNode.right);
		
		// append the current node info
		keysOut[index] = currNode.key;
		valuesOut[index] = currNode.value;
		aux[index] = currNode.color;
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
	@Override
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

	/* void dotGenerator(PrintWriter, Proj04_BSTNode<K,V>)
	 *
	 * recursively add dot file contents
	 * 1. define current node
	 * 2. define arrows beneath
	 * 3. recurse into children
	 */
	private void dotGenerator(PrintWriter printWriter, 
		Proj04_BSTNode<K,V> currNode) {
		// step 1: define curr node
		String currName = "node_" + currNode.key;
		String fill = currNode.color.equals("b") ? "black" : "red";
		String font = currNode.color.equals("b") ? "white" : "black";
		printWriter.printf("%s [style=filled fillcolor=%s fontcolor=%s label=\"%s\\n%s\"];\n",
			currName, fill, font, currNode.key, currNode.value);
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
				printValid(printWriter, currName, leftChildName, "L", "sw");
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