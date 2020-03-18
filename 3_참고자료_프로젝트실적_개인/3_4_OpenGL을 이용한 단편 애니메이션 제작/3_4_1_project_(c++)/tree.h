#ifndef TREE_H
#define TREE_H

#define MAX_LEAFS 3000

class tree
{
public:
	tree();

	void render();
	void init();
	bool falling = false;
};

#endif // TREE_H