#pragma once
#include <iostream>
#include <exception>
#include <string>
#include <stdexcept>
#include <vector>
#include <cmath>
#include <random>
#include <optional>
using namespace std;


enum RBColor { RED, BLACK };


template<class T, typename K>
class RBNode {
public:
	shared_ptr<T> payload;

	RBColor color;

	RBNode * child_l;
	RBNode * child_r;
	RBNode * parent;

	RBNode( shared_ptr<T> & payload ) : payload( payload ), color( RBColor::RED ), child_l( nullptr ), child_r( nullptr ), parent( nullptr ) { };

	inline bool operator==( const RBNode<T, K> & r ) { return *this->payload == *r.payload; };
	inline bool operator<( const RBNode<T, K> & r ) { return *this->payload < *r.payload; };
	inline bool operator==( const T & r ) { return *this->payload == r; };
	inline bool operator<( const T & r ) { return *this->payload < r; };
	inline bool operator==( const K & r ) { return *this->payload == r; };
	inline bool operator<( const K & r ) { return *this->payload < r; };

	void print( ostream & out, int depth ) {
		out << string( depth, ' ' );
		out << ( this->color == RBColor::BLACK ? "B: " : "R: " ) << *this->payload << endl;

		if( this->child_l != nullptr )
			this->child_l->print( out, depth + 1 );
		if( this->child_r != nullptr )
			this->child_r->print( out, depth + 1 );
	}

};


template<class T, typename K>
class RBTree {
protected:
	// Manage memory by a vector of unique_ptrs, but all links between nodes will be raw pointers
	// This is safe, because they will all exist as long as the graph exists, and once the graph is killed, all nodes will be deallocated as expected
	// We could theoretically use weak_ptrs, but it would involve too much overhead
	// We never expose the nodes, so if someone gets a pointer to one they are not using the tree API
	vector<unique_ptr<RBNode<T, K>>> node_arena;
	RBNode<T, K> * root = nullptr;

	void rotate_right( RBNode<T, K> * node ) {

		// Replace node with its left child, preserving in-order traversal
		
		// Double check that child exists...
		if( node->child_l == nullptr )
			return;

		RBNode<T, K> * cl = node->child_l;

		// Make our left tree the child's right
		node->child_l = cl->child_r;
		if( node->child_l != nullptr )
			node->child_l->parent = node;

		// Move child up
		cl->parent = node->parent;
		if( node->parent == nullptr ) {
			this->root = cl;
		} else {
			if( node->parent->child_l == node ) {
				node->parent->child_l = cl;
			} else {
				node->parent->child_r = cl;
			}
		}

		// Now move node down
		node->parent = cl;
		cl->child_r = node;

	}
	void rotate_left( RBNode<T, K> * node ) {

		// Replace node with its right child, preserving in-order traversal

		// Double check that child exists...
		if( node->child_r == nullptr )
			return;

		RBNode<T, K> * cr = node->child_r;

		// Make our left tree the child's right
		node->child_r = cr->child_l;
		if( node->child_r != nullptr )
			node->child_r->parent = node;

		// Move child up
		cr->parent = node->parent;
		if( node->parent == nullptr ) {
			this->root = cr;
		} else {
			if( node->parent->child_r == node ) {
				node->parent->child_r = cr;
			} else {
				node->parent->child_l = cr;
			}
		}

		// Now move node down
		node->parent = cr;
		cr->child_l = node;

	}

	void fixup_tree( RBNode<T, K> * node ) {

		RBNode<T, K> * parent = node->parent;

		// If parent is none, we are the root
		if( parent == nullptr ) {
			node->color = RBColor::BLACK;
			return;
		}

		bool is_left = parent->child_l == node;

		// If parent is black, do nothing
		if( node->parent->color == RBColor::BLACK )
			return;

		RBNode<T, K> * grandparent = node->parent->parent;

		// Parent is red, rest of cases will depend on uncle color
		// Check which way parent is, retrieve uncle
		bool parent_is_left = grandparent->child_l == parent;
		RBNode<T, K> * uncle = parent_is_left ? grandparent->child_r : grandparent->child_l;

		// If uncle is red, recolor and recurse
		if( uncle != nullptr && uncle->color == RBColor::RED ) {

			// Recolor and recurse on grandparent
			grandparent->color = RBColor::RED;
			uncle->color = RBColor::BLACK;
			parent->color = RBColor::BLACK;

			fixup_tree( grandparent );
			return;

		}

		// Uncle is black - rotate to fix
		if( parent_is_left ) {

			// If we make a triangle, need to rotate to a line
			if( !is_left ) {
				this->rotate_left( parent );
				swap( node, parent );
			}

			// Now that we are a line, rotate once more
			this->rotate_right( grandparent );
			parent->color = RBColor::BLACK;
			grandparent->color = RBColor::RED;

		} else {

			// Parent is right -- flipped from above

			// If we make a triangle, need to rotate to a line
			if( is_left ) {
				this->rotate_right( parent );
				swap( node, parent );
			}

			// Now that we are a line, rotate once more
			this->rotate_left( grandparent );
			parent->color = RBColor::BLACK;
			grandparent->color = RBColor::RED;

		}

	};

public:
	optional<shared_ptr<T>> search( K key ) {
		RBNode<T, K> * n = this->root;

		while( n != nullptr ) {
			if( *n == key ) {
				return { n->payload };
			} else if( *n < key ) {
				n = n->child_r;
			} else {
				n = n->child_l;
			}
		}

		return nullopt;
	};

	bool insert( shared_ptr<T> & payload ) {

		// Find spot to insert, fail if already in tree
		RBNode<T, K> * parent = this->root;
		bool is_left = true;

		while( parent != nullptr ) {
			if( *parent == *payload ) {

				// Fail, node already exists in tree
				return false;

			} else if( *parent < *payload ) {
				// Look right
				if( parent->child_r == nullptr ) {
					// We need to be right child
					is_left = false;
					break;
				} else {
					// Loop to right child
					parent = parent->child_r;
				}
			} else if( parent->child_l == nullptr ) {
				// We need to be the left child
				is_left = true;
				break;
			} else {
				// Loop to left child
				parent = parent->child_l;
			}
		}

		// Make a new node
		unique_ptr<RBNode<T, K>> node_ptr = make_unique<RBNode<T, K>>( payload );
		// Get raw ptr before we move it to the arena
		RBNode<T, K> * node = node_ptr.get( );
		// Move to the arena
		this->node_arena.push_back( move( node_ptr ) );

		// Attach it to its parent
		node->parent = parent;

		// If we are the first, set as root and return
		if( parent == nullptr ) {

			this->root = node;
			node->color = RBColor::BLACK;
			return true;

		} else {

			// Otherwise link parent to us
			if( is_left ) {
				parent->child_l = node;
			} else {
				parent->child_r = node;
			}

		}

		// Now we have to repair the tree
		fixup_tree( node );

		return true;

	};

	friend ostream & operator<<( ostream & out, const RBTree<T, K> & t ) {

		if( t.root != nullptr )
			t.root->print( out, 0 );

		return out;

	};
};