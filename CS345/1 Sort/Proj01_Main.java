/* Main
 *
 * Part of Project 1 solution; runs the various sort algorithms.  The
 * data that gets sorted it randomly generated; the seed for the random
 * number generator is auto-generated (if not provided on the command line),
 * or it can be hard-coded on command line (so as to repeat experiments).
 *
 * Author: Russell Lewis
 */



import java.util.Random;
import java.util.Arrays;

public class Proj01_Main
{
	public static void main(String[] args)
	{
		// is this debug statement?
		boolean debug = false;
		if (args.length > 0 && args[args.length-1].equals("debug"))
		{
			debug = true;

			// this chops off the last argument from the array
			args = Arrays.copyOf(args, args.length-1);
		}


		// shall we sort the input data?
		boolean doSort = false;
		if (args.length > 0 && args[args.length-1].equals("sorted"))
		{
			doSort = true;

			// this chops off the last argument from the array
			args = Arrays.copyOf(args, args.length-1);
		}


		// is this the special "million" mode?
		boolean million = false;
		if (args.length > 0 && args[args.length-1].equals("million"))
		{
			million = true;

			// this chops off the last argument from the array
			args = Arrays.copyOf(args, args.length-1);
		}


		// is this the special "million" mode?
		boolean string = false;
		if (args.length > 0 && args[args.length-1].equals("String"))
		{
			string = true;

			// this chops off the last argument from the array
			args = Arrays.copyOf(args, args.length-1);
		}


		// very simple sanity checking of the arguments.  We'll
		// do more details down below - but this is the *ONLY*
		// case where we print the syntax help-text.  Lazy?  Yes.
		// Survivable?  On a one week project?  Yep.
		if (args.length == 0 || args.length > 2)
		{
			System.err.printf("SYNTAX: Proj01_Main <sortType> [<randomSeed>] [String] [million] [sorted] [debug]\n");
			System.err.printf("  sort types:\n");
			System.err.printf("        BubbleSort\n");
			System.err.printf("        InsertionSort\n");
			System.err.printf("        SelectionSort\n");
			System.err.printf("        MergeSort\n");
			System.err.printf("        QuickSort\n");
			System.err.printf("\n");
			System.err.printf("  [<randomSee>] - selects exactly what dataset to use (instead of a randomly-chosen one)\n");
			System.err.printf("\n");
			System.err.printf("  String  - the input data is Strings, instead of integers\n");
			System.err.printf("  million - if set, the dataset will have exactly 10^6 elements\n");
			System.err.printf("  sorted  - if set, we'll pre-sort the data before we send it to the sort algo\n");
			System.err.printf("            (forces mode 2 if QuickSort)\n");
			System.err.printf("  debug   - if set, the sort class will be created with debug=true\n");
			return;
		}


		// we determine the random seed either by (a) reading
		// it from the second command-line argument, or
		// (b) auto-generating it as a random number.
		//
		// Why generate a "random" number as a seed for your
		// own generator?  Certainly, it doesn't make things
		// any *more* random than they had been.  Answer: simply
		// so that the code below (which prints, and then uses,
		// the seed) is common in both code paths.
		int seed;
		if (args.length == 2)
		{
			try {
				seed = Integer.parseInt(args[1]);
			} catch (NumberFormatException e) {
				System.err.printf("ERROR: Could not convert the 2nd command-line argument, '%s', into an integer.\n", args[1]);
				return;
			}
		}
		else
		{
			seed = (int)(1000*1000*Math.random());
		}


		// now that we have the seed, we'll print it out, so that
		// people attempting to re-create their broken testcases
		// can know what seed to re-use.
		//
		// Then we'll use it.  See above to understand why we re-init
		// the random system, even if we've used it to generate the
		// very seed we plan to use.
		System.out.printf("SEED:   %d\n", seed);
		Random rand = new Random(seed);


		// some of the sorts (not all) use one or two integer
		// arguments: the Quicksort pivot mode (0,1,2) and the block
		// size (2..12, inclusive).  We'll generate the data length
		// (and contents) later.
		int mode    = (doSort ? 2 : rand.nextInt(3));
		int baseLen = 2+rand.nextInt(11);


		// now, parse the 1st command-line argument, and create an
		// object of the proper type.
		Proj01_Sort sort;

		// UPDATE: I made this check case-insensitive.  Originally, I
		//         wanted to add support for just "Quicksort" vs. the
		//         standard "QuickSort" - but I decide to just be
		//         altogether flexible.  Why not?
		switch (args[0].toLowerCase())
		{
		case "bubblesort":      sort = new Proj01_BubbleSort(debug);   break;
		case "insertionsort":   sort = new Proj01_InsertionSort(debug);   break;
		case "selectionsort":   sort = new Proj01_SelectionSort(debug);   break;
		case "mergesort":       sort = new Proj01_MergeSort(debug, baseLen);   break;
		case "quicksort":       sort = new Proj01_QuickSort(debug, mode,baseLen);   break;

		default:
			System.err.printf("ERROR: The <sortType> argument '%s' is not recognized.\n", args[0]);
			return;
		}


		// generate the data.  The length of the data needs to be long
		// enough to hold more than one baseLen block - but don't
		// hard-code it to be a multiple of the block length.
		int count = (million ? 1000*1000 : 3*baseLen + rand.nextInt(10*baseLen));

		Comparable[] arr = new Comparable[count];
		for (int i=0; i<arr.length; i++)
			if (string == false)
				arr[i] = rand.nextInt(arr.length*10);
			else
				arr[i] = genRandomString(rand);

		// pre-sort the input data, if the user requested that.
		if (doSort)
			Arrays.sort(arr);

		// we'll always have a duplicate of the data, which is sorted (for output checks)
		Comparable[] sorted = Arrays.copyOf(arr, arr.length);
		Arrays.sort(sorted);

		// don't print out the buffers if millions is set!
		if (million)
		{
			System.out.printf("DATA:   <1 million elements>\n");
			System.out.printf("SORTED: <1 million elements>\n");
		}
		else
		{
			System.out.printf("DATA:  ");
			for (int i=0; i<arr.length; i++)
				System.out.printf(" %s", arr[i]);
			System.out.printf("\n");

			System.out.printf("SORTED:");
			for (int i=0; i<arr.length; i++)
				System.out.printf(" %s", sorted[i]);
			System.out.printf("\n");

			System.out.printf("\n");
		}

		System.out.printf("---- RUNNING %s ----\n", sort);
		sort.sort(arr);
		System.out.printf("\n");

		for (int i=0; i<arr.length; i++)
		if (arr[i].equals(sorted[i]) == false)
		{
			System.out.printf("  *** MISCOMPARE ***    first error at index %d\n", i);

			System.out.printf("  CORRECT:");
			for (int j=0; j<sorted.length; j++)
			{
				if (j == i)
					System.out.printf(" .");
				System.out.printf(" %s", sorted[j]);
			}
			System.out.printf("\n");

			System.out.printf("  ERROR:  ");
			for (int j=0; j<sorted.length; j++)
			{
				if (j == i)
					System.out.printf(" .");
				System.out.printf(" %s", arr[j]);
			}
			System.out.printf("\n");

			// terminate the testcase without printing
			// the "OK" message at the bottom!
			return;
		}


		// this line is printed to allow the grading script to
		// detect, easily, whether all of the tests completed.  Do
		// *NOT* attempt to falsify this line if your tests are
		// broken - that is a violation of the Code of Academic
		// Integrity!

		System.out.printf("--- TESTCASE TERMINATED, NO ERRORS FOUND ---\n");
	}


	public static String genRandomString(Random rand)
	{
		int len = 8+rand.nextInt(4);

		String retval = "";
		for (int i=0; i<len; i++)
			retval += (char)('a'+rand.nextInt(26));

		return retval;
	}
}

