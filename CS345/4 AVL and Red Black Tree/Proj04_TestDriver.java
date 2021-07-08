/* Proj04_TestDriver
 *
 * Class to exercise the BST class.  It reads a command file from stdin, and
 * performs operations on the BSTs.
 */

import java.util.*;

public class Proj04_TestDriver
{
	/* main
	 *
	 * Test driver for the BST_student class (and, for comparison,
	 * the BST_example class).
	 *
	 * Reads commands from stdin; passes these commands as operations
	 * on the tree.
	 */
	public static void main(String[] args)
	{
		/* there are 3 trees, each with different key/value types */
		Proj04_Dictionary<Integer,String>  is = null;
		Proj04_Dictionary<String, Integer> si = null;


		boolean avl = false;
		if (args.length > 0 && args[0].equals("AVL"))
			avl = true;
		else if (args.length > 0 && args[0].equals("RedBlack"))
			avl = false;
		else
			args = new String[0];  // forces the helper text later


		if (args.length == 2 && args[1].equals("example"))
		{
			if (avl)
			{
				is = new Proj04_AVL_example<Integer,String> ("debug_AVL_example_is");
				si = new Proj04_AVL_example<String, Integer>("debug_AVL_example_si");
			}
			else
			{
				is = new Proj04_RedBlack_example<Integer,String> ("debug_redblack_example_is");
				si = new Proj04_RedBlack_example<String, Integer>("debug_redblack_example_si");
			}
		}
		else if (args.length == 1)
		{
			if (avl)
			{
				is = new Proj04_AVL_student<Integer,String> ("debug_AVL_student_is");
				si = new Proj04_AVL_student<String, Integer>("debug_AVL_student_si");
			}
			else
			{
				is = new Proj04_RedBlack_student<Integer,String> ("debug_redblack_student_is");
				si = new Proj04_RedBlack_student<String, Integer>("debug_redblack_student_si");
			}
		}
		else
		{
			System.out.printf("SYNTAX: (AVL|RedBlack) example?\n");
			System.exit(1);
		}



		/* we'll read the commands from stdin.  Note that our input
		 * parser is *VERY* forgiving; we'll always assume that the
		 * input has valid format.
		 */
		Scanner in = new Scanner(System.in);

		while (in.hasNext())
		{
			/* read the tree-name string first. */
			String tree = in.next();

			/* validate the tree type */
			switch (tree)
			{
			case "is": case "si":
				break;

			default:
				System.err.printf("TESTCASE ERROR: The first word on the line is not a valid tree name: is/si\n");
				System.exit(1);
			}


			/* the tree type must be followed by another word,
			 * which is the command
			 */
			if (in.hasNext() == false)
			{
				System.out.printf("TESTCASE ERROR: Invalid command.  A tree name was not followed by anything else in the file\n");
				System.exit(1);
			}
			String cmd = in.next();


			switch (cmd)
			{
			default:
				System.out.printf("TESTCASE ERROR: Unrecognized command '%s'\n", cmd);
				System.exit(1);

			case "set":
				switch (tree)
				{
				case "is":
				{
					Integer i = readInt(in);
					String  s = readStr(in);

					/* if this is a Red-Black tree, then look for
					 * duplicates before we try the set().
					 */
					if (avl == false && is.get(i) != null)
						break;

					is.set(i,s);
				}
				break;

				case "si":
				{
					String  s = readStr(in);
					Integer i = readInt(in);

					/* if this is a Red-Black tree, then look for
					 * duplicates before we try the set().
					 */
					if (avl == false && si.get(s) != null)
						break;

					si.set(s,i);
				}
				break;
				}
				break;

			case "get":
			{
				Comparable key = null;
				Object     val = null;

				Integer i;
				String  s;

				switch (tree)
				{
				case "is":
					key = i = readInt(in);
					val = is.get(i);
					break;
				case "si":
					key = s = readStr(in);
					val = si.get(s);
					break;
				}

				System.out.printf("tree %s: get(%s) returned %s\n",
				                  tree, key, val);
			}
			break;

			case "remove":
				if (avl == false)
				{
					System.out.printf("-- skipping the 'remove' command, because this is not an AVL tree.\n");

					// consume the argument, ignore it
					in.next();
					break;
				}

				switch (tree)
				{
				case "is": is.remove(readInt(in)); break;
				case "si": si.remove(readStr(in)); break;
				}
				break;

			case "getSize":
			{
				int size = 0;
				switch(tree)
				{
				case "is": System.out.printf("is.getSize()=%d\n", is.getSize()); break;
				case "si": System.out.printf("si.getSize()=%d\n", si.getSize()); break;
				}
			}
			break;

			case "inOrder":
			{
				if (tree.equals("is"))
				{
					int       size = is.getSize();
					Integer[] keys = new Integer[size];
					String [] vals = new String [size];
					String [] aux  = new String [size];
					is.inOrder(keys,vals,aux);
					printTraversal("inOrder",tree, keys,vals,aux);
				}
				if (tree.equals("si"))
				{
					int           size = si.getSize();
					String [] keys = new String [size];
					Integer[] vals = new Integer[size];
					String [] aux  = new String [size];
					si.inOrder(keys,vals,aux);
					printTraversal("inOrder",tree, keys,vals,aux);
				}
			}
			break;

			case "postOrder":
			{
				if (tree.equals("is"))
				{
					int       size = is.getSize();
					Integer[] keys = new Integer[size];
					String [] vals = new String [size];
					String [] aux  = new String [size];
					is.postOrder(keys,vals,aux);
					printTraversal("postOrder",tree, keys,vals,aux);
				}
				if (tree.equals("si"))
				{
					int       size = si.getSize();
					String [] keys = new String [size];
					Integer[] vals = new Integer[size];
					String [] aux  = new String [size];
					si.postOrder(keys,vals,aux);
					printTraversal("postOrder",tree, keys,vals,aux);
				}
			}
			break;

			case "dotFile":
				switch (tree)
				{
				case "is": is.genDebugDot(); break;
				case "si": si.genDebugDot(); break;
				}
				break;

			}
		}

		System.out.printf("--- TESTCASE TERMINATED ---\n");
	}


	public static int readInt(Scanner in)
	{
		if (in.hasNext() == false)
		{
			System.out.printf("TESTCASE ERROR: The file ended, but the last command required another parameter.\n");
			System.exit(1);
		}

		if (in.hasNextInt() == false)
		{
			System.out.printf("TESTCASE ERROR: The command did not have the type of parameters that it expected.\n");
			System.exit(1);
		}

		return in.nextInt();
	}
	public static String readStr(Scanner in)
	{
		if (in.hasNext() == false)
		{
			System.out.printf("TESTCASE ERROR: The file ended, but the last command required another parameter.\n");
			System.exit(1);
		}

		return in.next();
	}


	public static <K,V> void printTraversal(String traversal, String tree,
	                                        K[] keys, V[] vals, String[] aux)
	{
		System.out.printf("%s traversal of the tree '%s':", traversal, tree);
		for (int i=0; i<keys.length; i++)
			System.out.printf(" %s->%s(%s)", keys[i],vals[i],aux[i]);
		System.out.printf("\n");
	}
}

