///////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT MPQ μέσω σωρού.
//
///////////////////////////////////////////////////////////

#include <stdlib.h>
#include <assert.h>

#include "ADTMPQ.h"
#include "ADTVector.h"

// Ενα MPQ είναι pointer σε αυτό το struct
struct mpq {
	Vector values;				// Τα δεδομένα, σε Vector ώστε να έχουμε μεταβλητό μέγεθος χωρίς κόπο
	CompareFunc compare;		// Η διάταξη
	DestroyFunc destroy_value;	// Συνάρτηση που καταστρέφει ένα στοιχείο του vector.
};

//ΒΟΗΘΗΤΙΚΕΣ ΣΥΝΑΡΤΗΣΕΙΣ
static Pointer node_value(MPQ mpq, int node_id) {
	// τα node_ids είναι 1-based, το node_id αποθηκεύεται στη θέση node_id - 1
	return vector_get_at(mpq->values, node_id - 1);
}

// Ανταλλάσει τις τιμές των κόμβων node_id1 και node_id2

static void node_swap(MPQ mpq, int node_id1, int node_id2) {
	// τα node_ids είναι 1-based, το node_id αποθηκεύεται στη θέση node_id - 1
	Pointer value1 = node_value(mpq, node_id1);
	Pointer value2 = node_value(mpq, node_id2);

	vector_set_at(mpq->values, node_id1 - 1, value2);
	vector_set_at(mpq->values, node_id2 - 1, value1);
}

// Αποκαθιστά την ιδιότητα του σωρού.
// Πριν: όλοι οι κόμβοι ικανοποιούν την ιδιότητα του σωρού, εκτός από
//       τον node_id που μπορεί να είναι _μεγαλύτερος_ από τον πατέρα του.
// Μετά: όλοι οι κόμβοι ικανοποιούν την ιδιότητα του σωρού.

static void bubble_up(MPQ mpq, int node_id) {
	// Αν φτάσαμε στη ρίζα, σταματάμε
	if (node_id == 1)
		return;

	int parent = node_id / 2;		// Ο πατέρας του κόμβου. Τα node_ids είναι 1-based

	// Αν ο πατέρας έχει μικρότερη τιμή από τον κόμβο, swap και συνεχίζουμε αναδρομικά προς τα πάνω
	if (mpq->compare(node_value(mpq, parent), node_value(mpq, node_id)) < 0) {
		node_swap(mpq, parent, node_id);
		bubble_up(mpq, parent);
	}
}

// Αποκαθιστά την ιδιότητα του σωρού.
// Πριν: όλοι οι κόμβοι ικανοποιούν την ιδιότητα του σωρού, εκτός από τον
//       node_id που μπορεί να είναι _μικρότερος_ από κάποιο από τα παιδιά του.
// Μετά: όλοι οι κόμβοι ικανοποιούν την ιδιότητα του σωρού.

static void bubble_down(MPQ mpq, int node_id) {
	// βρίσκουμε τα παιδιά του κόμβου (αν δεν υπάρχουν σταματάμε)
	int left_child = 2 * node_id;
	int right_child = left_child + 1;

	int size = mpq_size(mpq);
	if (left_child > size)
		return;

	// βρίσκουμε το μέγιστο από τα 2 παιδιά
	int max_child = left_child;
	if (right_child <= size && mpq->compare(node_value(mpq, left_child), node_value(mpq, right_child)) < 0)
			max_child = right_child;

	// Αν ο κόμβος είναι μικρότερος από το μέγιστο παιδί, swap και συνεχίζουμε προς τα κάτω
	if (mpq->compare(node_value(mpq, node_id), node_value(mpq, max_child)) < 0) {
		node_swap(mpq, node_id, max_child);
		bubble_down(mpq, max_child);
	}
}


MPQ mpq_create(CompareFunc compare, DestroyFunc destroy_value) {
	MPQ mpq=malloc(sizeof(*mpq));
	mpq->compare = compare;
	mpq->destroy_value = destroy_value;

	mpq->values=vector_create(0, NULL);//δημιουργία άδειου vector χωρίς destroy function

	return mpq;
}

int mpq_size(MPQ mpq) {
	int size=vector_size(mpq->values);
	return size;
}

Pointer mpq_max(MPQ mpq) {
	return vector_get_at(mpq->values, 0);//βρισκόμασται στο heap άρα το max στοιχείο βρίσκεται στη ρίζα
}

void mpq_insert(MPQ mpq, Pointer value) {
	vector_insert_last(mpq->values, value);
	bubble_up(mpq, vector_size(mpq->values));//για να ικανοποιείται η ιδιότητα του σωρού το νέο στοιχείο μετακινείται προς τα πάνω αν χρειάζεται
}

void mpq_remove_max(MPQ mpq) {

	int last_node =vector_size(mpq->values);
	assert(last_node != 0);//έλεγχος αν το vector δεν είναι άδειο

	if (mpq->destroy_value != NULL)
		mpq->destroy_value(mpq_max(mpq));//καταστροφή max value αν υπάρχει destroy function

	
	node_swap(mpq, 1, last_node);//αλλάζω θέσεις του τελευταίου κόμβου και της ρίζας
	vector_remove_last(mpq->values);//και διαγράφω τον τελευταίο κόμβο άρα και το max value

	bubble_down(mpq, 1);//επεναφέρουμε την ιδιότητα του σωρού ξεκινώντας απο την ρίζα
}

void mpq_merge(MPQ mpq, MPQ mpq2) {
	int size2=mpq_size(mpq2);
	for(int i=0;i<size2;i++){//παίρνω τα στοιχειά του vector απο το mpq2 και τα τοποθετώ στο τέλος του vector του mpq
		Pointer mpq2_value=vector_get_at(mpq2->values, i);
		vector_insert_last(mpq->values,mpq2_value);
	}
	int size=mpq_size(mpq)/2;
	for(int i=size;i>=1;i--){//παίρνω κάθε πατέρα και κάνω bubble down ώστε να ισχυεί η ιδιότητα του σωρού
		bubble_down(mpq,i);
	}
	vector_destroy(mpq2->values);//καταστρέφω το vector του mpq2,όχι τα στοιχεία του
	free(mpq2);//και αποδεσμεύω την δομή
}

DestroyFunc mpq_set_destroy_value(MPQ mpq, DestroyFunc destroy_value) {
	DestroyFunc old = mpq->destroy_value;//αποθηκευω την παλιά συνάρτηση
	mpq->destroy_value = destroy_value;//ενημερώνω το πεδίο της δομής
	return old;//και επιστρέφω τη παλιά
}

void mpq_destroy(MPQ mpq) {
	vector_set_destroy_value(mpq->values, mpq->destroy_value);//ορίζω τη destroy function ώστε να καταστραφούν και τα στοιχεία του vector στη συνέχεια
	vector_destroy(mpq->values);//καταστρέφω το vector

	free(mpq);//και αποδεσμευω τη δομή
}
