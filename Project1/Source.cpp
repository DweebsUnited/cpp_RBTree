#include "RB.h"
using namespace std;


void quicksort( vector<int> & arr, int left, int right ) { // 3 1 2 -- 1 3 2

	// Sort from left to right inclusive

	// Base case - nothing to sort
	if( left >= right )
		return;

	// Pick a pivot, swap to back
	// Naive, use last elem as-is
	int pivot_val = arr[ right ];

	// Loop from left and right, find elements on wrong side
	int from_left = left;
	int from_right = right; // Start at the pivot element, which we will skip in the first loop of from_right, unless it is the largest element in the segment in which case no work will be done

	while( from_left < from_right ) {
		// Skip all elements on left that are in correct partition
		// We don't need to check for the end here because we will always stop at least at the pivot
		while( arr[ from_left ] < pivot_val )
			from_left++;

		// Here we have to make sure we stop at from_left
		// We can blow off end if pivot happens to be the smallest element in this segment
		while( arr[ from_right ] >= pivot_val && from_left < from_right )
			from_right--;

		// If we should swap these elements, do so
		if( from_left < from_right ) {
			swap( arr[ from_left ], arr[ from_right ] );
		}
	}

	// Iterators will always point to first element in larger partition

	// Partitioning is done!
	// Swap pivot in to place, recurse
	swap( arr[ right ], arr[ from_left ] );

	// Skip the pivot though, no need to do more work than necessary
	quicksort( arr, left, from_left - 1 );
	quicksort( arr, from_left + 1, right );

}



class Payload {
public:
	int value;
	Payload( int v ) : value( v ) { };

	inline bool operator==( const Payload & r ) { return this->value == r.value; }
	inline bool operator<( const Payload & r ) { return this->value < r.value; }
	inline bool operator==( const int & r ) { return this->value == r; }
	inline bool operator<( const int & r ) { return this->value < r; }
};

inline ostream & operator<<( ostream & out, Payload & p ) { return out << p.value; };


#define NUM_ELEM 64

int main( ) {

	vector<int> arr;
	arr.reserve( NUM_ELEM );

	random_device rng;
	default_random_engine gen( rng( ) );
	uniform_int_distribution<int> dist( 1, 1024 );

	RBTree<Payload, int> t;

	for( int _ = 0; _ < NUM_ELEM; ++_ ) {
		shared_ptr<Payload> p = make_shared<Payload>( dist( gen ) );
		t.insert( p );
	}

	for( int _ = 0; _ < 32; ++_ ) {
		int guess = dist( gen );
		optional<shared_ptr<Payload>> res = t.search( guess );
		if( res )
			cout << guess << ": " << **res << endl;
		else
			cout << guess << ": MISSING" << endl;
	}

	cout << t;

	return 0;

}