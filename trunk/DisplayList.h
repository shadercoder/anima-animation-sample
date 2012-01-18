#if !defined(__DISPLY_LIST_H__)
#define __DISPLY_LIST_H__

class RenderContext;

/* Extremely simple display list implementation: each list node has a 'render()' function and a pointer to the next node */
namespace DisplayList
{
	class Node
	{
		Node* next;

	public:
		Node() : next(0) {}
		
		virtual void Render( RenderContext* context ) = 0;
		virtual Node* Next() { return next; }	
		virtual void SetNext( Node* n ) { next = n; }
	};
}

#endif