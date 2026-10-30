///////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT MPQ μέσω του ADT Priority Queue.
//
///////////////////////////////////////////////////////////

#include <stdlib.h>

#include "ADTMPQ.h"
#include "ADTPriorityQueue.h"

// Ενα MPQ είναι pointer σε αυτό το struct
struct mpq {
	PriorityQueue pqueue; //κάθε mpq struct περιέχει ενα priority queue
};

MPQ mpq_create(CompareFunc compare, DestroyFunc destroy_value) {
	MPQ mpq=malloc(sizeof(*mpq));//δεσμεύω μνήμη για το struct που θα επιστραφεί
	mpq->pqueue=pqueue_create(compare, destroy_value, NULL);// δημιουργώ το priority queue που βρίσκεται στο struct
	return mpq;
}

int mpq_size(MPQ mpq) {
	int size=pqueue_size(mpq->pqueue);
	return size;
}

Pointer mpq_max(MPQ mpq) {
	Pointer max=pqueue_max(mpq->pqueue);
	return max;
}

void mpq_insert(MPQ mpq, Pointer value) {
	pqueue_insert(mpq->pqueue, value);
}

void mpq_remove_max(MPQ mpq) {
	pqueue_remove_max(mpq->pqueue);
}

void mpq_merge(MPQ mpq, MPQ mpq2) {
	int size2=mpq_size(mpq2);
	pqueue_set_destroy_value(mpq2->pqueue,NULL);//καλώ την συνάρτηση ώστε όταν αφαιρέσω το στοιχείο να μην γίνεται και free-έτσι σίγουρα το value τοποθετείται στην ουρά 1
	while(size2>0){
		Pointer new_value=pqueue_max(mpq2->pqueue);//αποθηκεύω το max value της ουράς 2 κάθε φορά 
		pqueue_remove_max(mpq2->pqueue);//και το αφαιρώ από την ουρά κάθε φορά -είναι σίγουρο πως δεν θα καταστραφεί
		pqueue_insert(mpq->pqueue, new_value);//τοποθετώ το value κάθε φορρά στην ουρά 1 ώστε να γίνει το merge
		size2--;
	}
	mpq_destroy(mpq2);
}

DestroyFunc mpq_set_destroy_value(MPQ mpq, DestroyFunc destroy_value) {
	DestroyFunc destroyed=pqueue_set_destroy_value(mpq->pqueue,destroy_value);//αλλάζω τη συνάρτηση destroy της ουράς και αποθηκευεται η προηγούμενη
	return destroyed;//και επιστρέφεται η παλιά 
}

void mpq_destroy(MPQ mpq) {
	pqueue_destroy(mpq->pqueue);////καταστρέφω το priority queue του struct
	free(mpq);//και τέλος καταστρέφω όλο το struct
}
