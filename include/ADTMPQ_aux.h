///////////////////////////////////////////////////////////////////
//
// Βοηθητικές συναρτήσεις για τον ADT MPQ.
// Υποστηρίζουν οποιαδήποτε υλοποίηση του MPQ.
//
///////////////////////////////////////////////////////////////////

#pragma once // #include το πολύ μία φορά

#include <stdlib.h>

#include "ADTMPQ.h"


// Δημιουργεί και επιστρέφει μια νέα ουρά προτεραιότητας αρχικοποιημένη με τα στοιχεία του Vector values.
// Τα στοιχεία συγκρίνονται με βάση τη συνάρτηση compare.
// Αν destroy_value != NULL, τότε καλείται destroy_value(value) κάθε φορά που αφαιρείται ένα στοιχείο.

MPQ mpq_create_from_values(CompareFunc compare, DestroyFunc destroy_value, Vector values);
