import java.util.Scanner;


public class Proj04_TestBuildRBFrom234
{
	public static void main(String[] args)
	{
		/* read in the shape of the tree from stdin */
		Scanner in = new Scanner(System.in);
		Proj04_234Node<Integer,String> root = null;
		while (in.hasNext())
			root = addNode(root, buildNode(in.next()));


		/* convert it to a Red-Black tree */
		Proj04_Dictionary<Integer,String> tree = null;
		if (args.length == 1 && args[0].equals("example"))
			tree = new Proj04_RedBlack_example<Integer,String>("debug_example_from234", root);
		else if (args.length == 0)
			tree = new Proj04_RedBlack_student<Integer,String>("debug_student_from234", root);
		else
		{
			System.err.printf("SYNTAX: java Proj04_TestBuildRBFrom234 example?\n");
			System.exit(1);
		}


		int size = tree.getSize();
		Integer[] keys = new Integer[size];
		String [] vals = new String [size];
		String [] aux  = new String [size];

		tree.inOrder(keys,vals,aux);
		Proj04_TestDriver.printTraversal("inOrder","234->red_black", keys,vals,aux);

		tree.postOrder(keys,vals,aux);
		Proj04_TestDriver.printTraversal("postOrder","234->red_black", keys,vals,aux);

		tree.genDebugDot();

		System.out.printf("--- TESTCASE TERMINATED ---\n");
	}

	public static Proj04_234Node<Integer,String> buildNode(String str)
	{
		Proj04_234Node<Integer,String> retval = new Proj04_234Node<>();

		String[] keys = str.split(",");
		if (keys.length < 0 || keys.length > 3)
			throw new IllegalArgumentException("Invalid number of keys in a node, in the input text file");

		retval.numKeys = keys.length;

		retval.key1 = Integer.parseInt(keys[0]);
		retval.val1 = "x";

		if (keys.length >= 2)
		{
			retval.key2 = Integer.parseInt(keys[1]);
			retval.val2 = "x";
		}

		if (keys.length == 3)
		{
			retval.key3 = Integer.parseInt(keys[2]);
			retval.val3 = "x";
		}

		return retval;
	}

	public static Proj04_234Node<Integer,String> addNode(
	                         Proj04_234Node<Integer,String> root,
	                         Proj04_234Node<Integer,String> newNode)
	{
		if (newNode == null)
			throw new IllegalArgumentException("newNode is null");

		if (root == null)
			return newNode;

		if (newNode.key1.compareTo(root.key1) < 0)
			root.child1 = addNode(root.child1, newNode);
		else if (root.numKeys == 1 || newNode.key1.compareTo(root.key2) < 0)
			root.child2 = addNode(root.child2, newNode);
		else if (root.numKeys == 2 || newNode.key1.compareTo(root.key3) < 0)
			root.child3 = addNode(root.child3, newNode);
		else
			root.child4 = addNode(root.child4, newNode);

		return root;
	}
}

