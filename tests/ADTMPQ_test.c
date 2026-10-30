//////////////////////////////////////////////////////////////////
//
// Unit tests για τον ADT Mergeable Priority Queue.
// Οποιαδήποτε υλοποίηση οφείλει να περνάει όλα τα tests.
//
//////////////////////////////////////////////////////////////////

#include "acutest.h"			// Απλή βιβλιοθήκη για unit testing

#include "ADTMPQ.h"
#include "ADTMPQ_aux.h"

// θέτει τα στοιχεία του πίνακα array  σε τυχαία σειρά
void shuffle(int* array[], int size) {
	for (int i = 0; i < size; i++) {
		int new_pos = i + rand() / (RAND_MAX / (size - i) + 1);
		int* temp = array[new_pos];
		array[new_pos] = array[i];
		array[i] = temp;
	}
}

// Επιστρέφει έναν ακέραιο σε νέα μνήμη με τιμή value
int* create_int(int value) {
	int* p = malloc(sizeof(int));
	*p = value;
	return p;
}

int compare_ints(Pointer a, Pointer b) {
	return *(int*)a - *(int*)b;
}

int destroy_count = 0;

void destroy_int(Pointer value) {
	destroy_count++;
	free(value);
}

void test_create(void) {
	MPQ mpq = mpq_create(compare_ints, NULL);
	mpq_set_destroy_value(mpq, NULL);

	TEST_ASSERT(mpq != NULL);
	TEST_ASSERT(mpq_size(mpq) == 0);

	mpq_destroy(mpq);
}

void test_insert(void) {
	MPQ mpq = mpq_create(compare_ints, NULL);
	int N = 1000;
	int* array = malloc(N * sizeof(*array));					// Στο mpq θα προσθέσουμε pointers προς τα στοιχεία αυτού του πίνακα

	// insert N στοιχεία
	for (int i = 0; i < N; i++) {
		array[i] = i;
		mpq_insert(mpq, &array[i]);
		TEST_ASSERT(mpq_size(mpq) == i+1);			// Το size ενημερώθηκε;
		TEST_ASSERT(mpq_max(mpq) == &array[i]);	// Εισαγωγή σε αύξουσα σειρά, το στοιχείο που μόλις βάλαμε πρέπει να είναι στην κορυφή
	}

	mpq_destroy(mpq);
	free(array);
}

void test_remove(void) {
	MPQ mpq = mpq_create(compare_ints, free);

	// προσθήκη δεδομένων, τυχαία σειρά
	int N = 10;
	int** array = malloc(N * sizeof(*array));
	for (int i = 0; i < N; i++)
		array[i] = create_int(i);
	shuffle(array, N);

	for (int i = 0; i < N; i++)
		mpq_insert(mpq, array[i]);

	// Διαδοχικά remove ώστε να συμβούν και resizes
	for (int i = N-1; i >= 0; i--) {
		int* value = mpq_max(mpq);
		TEST_ASSERT(*value == i);
		mpq_remove_max(mpq);
		TEST_ASSERT(mpq_size(mpq) == i);
	}

	mpq_destroy(mpq);

	// remove από ουρά χωρίς συνάρτηση destroy
	mpq = mpq_create(compare_ints, NULL);
	mpq_insert(mpq, &N);
	TEST_ASSERT(mpq_max(mpq) == &N);
	mpq_remove_max(mpq);
	mpq_destroy(mpq);
	free(array);
}

void test_merge(void) {
	MPQ mpq1 = mpq_create(compare_ints, free);
	MPQ mpq2 = mpq_create(compare_ints, free);
	int N = 20;
	int** values1 = malloc(N * sizeof(*values1));
	int** values2 = malloc(N * sizeof(*values2));

	for (int i = 0; i < N; i++) {
		values1[i] = create_int(i);
		values2[i] = create_int(i + N);
	}

	shuffle(values1, N);
	shuffle(values2, N);

	for (int i = 0; i < N; i++)
		mpq_insert(mpq1, values1[i]);
	for (int i = 0; i < N; i++)
		mpq_insert(mpq2, values2[i]);

	mpq_merge(mpq1, mpq2);

	TEST_ASSERT(mpq_size(mpq1) == 2 * N);

	for (int i = 2 * N - 1; i >= 0; i--) {
		TEST_ASSERT(*(int*)mpq_max(mpq1) == i);
		mpq_remove_max(mpq1);
	}

	mpq_destroy(mpq1);
	free(values1);
	free(values2);
}

void test_combined(void) {
	int N1 = 30, N2 = 30;
	int** values1 = malloc(N1 * sizeof(*values1));
	int** values2 = malloc(N2 * sizeof(*values2));
	Vector values = vector_create(0, NULL);

	destroy_count = 0;

	for (int i = 0; i < N1; i++)
		values1[i] = create_int(i);
	for (int i = 0; i < N2; i++)
		values2[i] = create_int(i + N1);

	shuffle(values1, N1);
	shuffle(values2, N2);

	for (int i = 0; i < N1; i++)
		vector_insert_last(values, values1[i]);

	MPQ mpq1 = mpq_create_from_values(compare_ints, destroy_int, values);
	MPQ mpq2 = mpq_create(compare_ints, destroy_int);

	for (int i = 0; i < N2; i++)
		mpq_insert(mpq2, values2[i]);

	mpq_insert(mpq1, create_int(N1 + N2));
	mpq_insert(mpq2, create_int(N1 + N2 + 1));

	vector_destroy(values);
	free(values1);
	free(values2);

	TEST_ASSERT(mpq_size(mpq1) == N1 + 1);
	TEST_ASSERT(mpq_size(mpq2) == N2 + 1);
	TEST_ASSERT(*(int*)mpq_max(mpq1) == N1 + N2);
	TEST_ASSERT(*(int*)mpq_max(mpq2) == N1 + N2 + 1);

	mpq_merge(mpq1, mpq2);

	TEST_ASSERT(mpq_size(mpq1) == N1 + N2 + 2);
	TEST_ASSERT(*(int*)mpq_max(mpq1) == N1 + N2 + 1);
	TEST_ASSERT(mpq_set_destroy_value(mpq1, NULL) == destroy_int);
	TEST_ASSERT(mpq_set_destroy_value(mpq1, destroy_int) == NULL);

	for (int i = N1 + N2 + 1; i >= 0; i--) {
		TEST_ASSERT(*(int*)mpq_max(mpq1) == i);
		mpq_remove_max(mpq1);
		TEST_ASSERT(mpq_size(mpq1) == i);
	}

	TEST_ASSERT(destroy_count == N1 + N2 + 2);
	mpq_destroy(mpq1);
}

void test_create_from_values(void) {
	int N = 8;
	int data[] = { 3, 1, 7, 0, 6, 2, 5, 4 };
	Vector values = vector_create(0, NULL);			// χωρίς destroy function, το destroy θα το κάνει η ουρά!

	destroy_count = 0;

	for (int i = 0; i < N; i++)
		vector_insert_last(values, create_int(data[i]));

	MPQ mpq = mpq_create_from_values(compare_ints, destroy_int, values);
	TEST_ASSERT(mpq != NULL);
	TEST_ASSERT(mpq_size(mpq) == N);
	TEST_ASSERT(*(int*)mpq_max(mpq) == N - 1);
	TEST_ASSERT(mpq_set_destroy_value(mpq, NULL) == destroy_int);
	TEST_ASSERT(mpq_set_destroy_value(mpq, destroy_int) == NULL);

	vector_destroy(values);

	for (int i = N - 1; i >= 3; i--) {
		TEST_ASSERT(*(int*)mpq_max(mpq) == i);
		mpq_remove_max(mpq);
		TEST_ASSERT(mpq_size(mpq) == i);
		TEST_ASSERT(destroy_count == N - 1 - i + 1);
	}

	TEST_ASSERT(*(int*)mpq_max(mpq) == 2);
	mpq_destroy(mpq);
	TEST_ASSERT(destroy_count == N);
}


// Λίστα με όλα τα tests προς εκτέλεση
TEST_LIST = {
	{ "mpq_create", test_create },
	{ "mpq_insert", test_insert },
	{ "mpq_remove_max", test_remove },
	{ "mpq_merge", test_merge },
	{ "mpq_combined", test_combined },
	{ "mpq_create_from_values", test_create_from_values },

	{ NULL, NULL } // τερματίζουμε τη λίστα με NULL
};
