/* Proj04_BSTNode
 *
 * author: Yang Hu
 *
 * Class of the Proj04_BSTNode. It is used both in AVL tree and RedBlack Tree
 */

public class Proj04_BSTNode<K extends Comparable, V> {

	public  K  key;
	public  V  value;
	public int count;
	public int height;
	public String color;
	public Proj04_BSTNode<K,V> left,right;


	/* constructor
	 *
	 * Parameters: K,V
	 *
	 * Generates a single BSTNode (with no children), with the key/value
	 * pair given.
	 */
	public Proj04_BSTNode(K key, V value)
	{
		if (value == null)
			throw new IllegalArgumentException("BSTNode: The 'value' parameter was null");

		this.key   	= key;
		this.value 	= value;
		this.height = 0;
		this.count 	= 1;
		this.color 	= "r";
		// the left,right nodes default to 'null' (Java defaults)
	}
}