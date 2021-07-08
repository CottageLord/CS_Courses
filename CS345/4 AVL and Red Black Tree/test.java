public class test{
	public static void main(String[] args) {
		Proj04_AVL_student avlTester = new Proj04_AVL_student<Integer,String>("avl_tester");

		avlTester.set(8, "eight");

		avlTester.genDebugDot(); 
		avlTester.set(9, "nine");

		avlTester.genDebugDot();
		avlTester.set(10, "ten");


		avlTester.genDebugDot();
		//avlTester.set(11, "eleven");

		//avlTester.rotateLeft(avlTester.getNode(10));
		/*
		System.out.printf("key: %d\nvalue: %s\ncount: %d\nheight: %d\n",
					avlTester.getNode(15).key, 
					avlTester.getNode(15).value, 
					avlTester.getNode(15).count, 
					avlTester.getNode(15).height);
		*/

	}
}