///////////////////////////////////////////////////////////
//
// Βοηθητικές συναρτήσεις για τον ADT MPQ.
//
// Υποστηρίζουν οποιαδήποτε υλοποίηση του MPQ (χωρίς να
// γνωρίζουμε τις λεπτομέρειες της υλοποίησης).
//
///////////////////////////////////////////////////////////

#include "ADTMPQ_aux.h"

static MPQ mpq_helper(Vector values,CompareFunc compare, DestroyFunc destroy_value){//βοηθητική αναδρομική συνάρτηση που θα κτίσει το mpq

	int size=vector_size(values);
	MPQ mpq;
	if(size==0){//αν δεν υπάρχουν στοιχεία στο vector επιστρέφεται άδειο mpq
		mpq=mpq_create(compare, destroy_value);
		return mpq;
	}

	if(size==1){//αν υπάρχει 1 στοιχείο προσθήτεται στο mpq και η αναδρομή τελειώνει
		mpq=mpq_create(compare, destroy_value);
		mpq_insert(mpq,vector_get_at(values,0));
		return mpq;
	}

	int mid=size/2;
	Vector left=vector_create(0, NULL);
	Vector right=vector_create(0, NULL);

	for(int i=0;i<mid;i++){
		vector_insert_last(left, vector_get_at(values, i));
	}

	for(int i=mid;i<size;i++){
		vector_insert_last(right, vector_get_at(values, i));
	}

	MPQ left_mpq=mpq_helper(left,compare,destroy_value);//αναδρομή για το αριστερό μισό του values
	MPQ right_mpq=mpq_helper(right,compare,destroy_value);//αναδρομή για το δεξι μισό του values

	mpq_merge(left_mpq, right_mpq);//συγχώνευση των δύο μισών mpq

	vector_destroy(left);
	vector_destroy(right);
	return left_mpq;//το πρώτο mpq που δίνεται στη συνάρτηση είναι αυτό που έχει όλα τα στοιχεία
}

MPQ mpq_create_from_values(CompareFunc compare, DestroyFunc destroy_value, Vector values) {

	return mpq_helper(values,compare,destroy_value);
}
