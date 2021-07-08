# README.md

A few samples on smart pointers. You do not have to do anything with thesei examples, but they are stored here because some asked about them during office hours.

Smart pointers are useful (and relevant) for resource managers, because you can use them to count references to a resource. If there is at least one reference to an object (i.e. more than one pointer to the same object), then you know that it is not safe to deallocate (i.e. free) that resource from the resource manager. A shared_ptr thus may be a good tool.

Other smart pointers exist like weak_ptr and uniq_ptr as well, and some day I may include more samples here.
