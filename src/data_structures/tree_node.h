#ifndef TREE_NODE_H
#define TREE_NODE_H

#include <list>
#include <memory>

template <class T>
class TreeNode
{
private:
	T data;

	std::weak_ptr<TreeNode<T>> parent;
	std::list<std::unique_ptr<TreeNode<T>>> children;

public:

	void add_child(std::unique_ptr<TreeNode<T>> &p_node)
	{
		//p_node->set_parent(this);
		children.push_back(std::move(p_node));
	}

	const std::list<std::unique_ptr<TreeNode<T>>> &get_children()
	{
		return children;
	}

	void set_parent(std::unique_ptr<TreeNode<T>> &p_node)
	{
		parent = p_node;
	}

	void set_data(const T &p_data)
	{
		data = p_data;
	}


	T get_data() const{
		return data;
	}
};
#endif // TREE_NODE_H
