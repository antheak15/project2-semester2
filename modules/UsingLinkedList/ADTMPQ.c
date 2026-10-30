///////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT MPQ μέσω συνδεδεμένης λίστας.
//
///////////////////////////////////////////////////////////

#include <stdlib.h>

#include "ADTMPQ.h"

typedef struct list_node{ //stuct που αναπαριστά τον κόμβο της συνδεδεμένης λίστας
	Pointer value; 
	struct list_node* next;
}Listnode;

// Ενα MPQ είναι pointer σε αυτό το struct
struct mpq {
	Listnode* list;//δείκτης στην αρχή-πρώτο κόμβο της λίστας
	Listnode* max;//δείκτης στο κόμβο με το μεγαλύτερο στοιχείο  
	Listnode* last;//δείκτης στο κόμβο με το τελευταίο στοιχείο  
	int size;// πόσα στοιχεία έχει μέσα η λίστα 
	CompareFunc compare;// η συνάρτηση που θα συγκρίνει 2 values 
	DestroyFunc destroy_value;//η συνάρτηση που θα κατστρέφει μια τιμή
};

MPQ mpq_create(CompareFunc compare, DestroyFunc destroy_value) {
	MPQ mpq=malloc(sizeof(*mpq));//δεσμεύω μνήμη για τη δομή MPQ που θα επιστραφεί στο τέλος
	
	if(mpq==NULL)
		return NULL;
	//και αρχικοποιώ τα πεδία της
	mpq->list=NULL;
	mpq->max=NULL;
	mpq->last=NULL;
	mpq->size=0;
	mpq->compare=compare;
	mpq->destroy_value=destroy_value;

	return mpq;
}

int mpq_size(MPQ mpq) {
	return mpq->size;
}

Pointer mpq_max(MPQ mpq) {
	if(mpq->max==NULL)
		return NULL;
	return mpq->max->value;
}

void mpq_insert(MPQ mpq, Pointer value) {
	Listnode* new_node=malloc(sizeof(*new_node));//δημιουργία καινούργιου κόμβου για το νέο στοιχείο

	if(new_node==NULL)
		return;

	// τοποθετώ τον νέο κόμβο στην αρχή της λίστας 
	new_node->value=value;
	new_node->next=mpq->list;
	mpq->list=new_node;

	if(mpq->size==0)//αν λίστα είναι άδεια το καινουργιο στοιχείο είναι και το τελευταιο
		mpq->last=new_node;

	if(mpq->max==NULL || mpq->compare(value,mpq->max->value)>0 )//αν η λίστα είναι αδεια άρα δεν υπάρχει max ή αν το νέο στοιχείο είναι μεγαλύτερο απο το max της λίστας
		mpq->max=new_node;//το max της λίστας αλλάζει

	mpq->size++;//υπάρχει ένα νέο στοιχείο στην λίστα το μέγεθος της λίστας αυξάνεται
}


void mpq_remove_max(MPQ mpq) {
	Listnode* current_max=mpq->max;//αποθήκευση του κόμβου που περιέχει το max
	Listnode* current_list=mpq->list;//δείκτης που θα διασχίζει την λίστα
	Listnode* previous=NULL;//θα περιέχει τον προηγούμενο κόμβο του current_list

	while(current_list!=current_max){//διάσχιση λίστας μέχρι να βρεθεί το max και ο προηγούμενος κόμβος
		previous=current_list;
		current_list=current_list->next;
	}

	if(previous==NULL){//αν το max βρισκόταν στον πρώτο κόμβο η νέα λίστα συνεχίζει απο τον επόμενο κόμβο
		mpq->list=current_list->next;
	}

	else if(current_list==mpq->last){//αν το max value βρισκόταν στον τελευταίο κόμβο
		mpq->last=previous; //ενημερώνεται το lasr 
		previous->next=current_list->next;//αγνοήται ο κόμβος που αφαιρέθηκε
	}
	else {//αν το max βρίσκεται ενδιάμεσα της λίστας
		previous->next=current_list->next;
	}

	mpq->size--;//αφαιρώ ένα κόμβο άρα μείωνεται κατά ένα το size

	if(mpq->destroy_value!=NULL)//καταστροφή του value αν υπάρχει συγκεκριμένη συνάρτηση 
		mpq->destroy_value(current_list->value);

	free(current_list);//και στη συνέχεια αποδέσμευση του κόμβου

	if(mpq->list==NULL){// αν το max ήταν το μόνο στοιχείο στην λίστα τώρα η λίστα είναι άδεια και μηδενίζονται οι δείκτες
		mpq->max=NULL;
		mpq->last=NULL;
	}
	else {//αν υπάρχουν ακόμα στοιχεία στην λίστα βρίσκουμε το καινούργιο max
		Listnode* new_max=mpq->list;
		Listnode* next_node=mpq->list->next;

		while(next_node!=NULL){//βρίσκουμε το μεγαλύτερο value ελέγχοντας μέχρι το τέλος της λίστας
			if(mpq->compare(next_node->value,new_max->value)>0){
				new_max=next_node;
			}
			next_node=next_node->next;
		}
		mpq->max=new_max;//ενημέρωση του max στην δομή
	}
}

void mpq_merge(MPQ mpq, MPQ mpq2) {
	if(mpq2->list==NULL)//αν η λίστα στο mpq2 είναι άδεια δεν χρειάζεται να γίνει merge
		return;

	else if(mpq->list==NULL){//αν η λίστα στο mpq είναι άδεια ενημερώνουμε τη λίστα του mpq με την λίστα που υπάρχει στο 2
		mpq->list=mpq2->list;
		mpq->max=mpq2->max;
		mpq->last=mpq2->last;
		mpq->size=mpq2->size;
	}
	else{//αλλιώς έχουμε merge των δύο λιστων 
		mpq->last->next=mpq2->list;//σύνδεση του τέλους τησ πρώτης λίστας με την αρχή της δευτερης
		mpq->last=mpq2->last;//ο τελευταίος κόμβος ενημερώνεται
		mpq->size+=mpq2->size;//το size γίνεται ίσο με τα στοιχεία των δύο λιστών

		if(mpq->compare(mpq2->max->value,mpq->max->value)>0)//αν το max value στη λίστα του 2 είναι μεγαλύτερο απο το max στη λίστα του mpq το max του mpq ενημερώνεται
			mpq->max=mpq2->max;
	}

	//αδειάζω το mpq2 χωρίς να καταστρέφονται οι κόμβοι
	mpq2->list=NULL;
	mpq2->max=NULL;
	mpq2->last=NULL;
	mpq2->size=0;
}

DestroyFunc mpq_set_destroy_value(MPQ mpq, DestroyFunc destroy_value) {//ενημέρωση της destroy function και επιστροφή της προηγούμενης
	DestroyFunc destroyed=mpq->destroy_value;//αποθηκευση της παλιάς destroy function
	mpq->destroy_value=destroy_value;
	return destroyed;
}

void mpq_destroy(MPQ mpq) {

	Listnode* current=mpq->list;
	while(current!=NULL){//διάσχιση της λίστας  ώστε να διαγράψουμε τους κόμβους
		Listnode* next=current->next;
		if(mpq->destroy_value!=NULL)//αν υπάρχει destroy function καταστρέφουμε το value
			mpq->destroy_value(current->value);
		free(current);//και αποδεσμεύουμε τον κόμβο
		current=next;
	}
	free(mpq);//αποδεσμευση της δομής 
}
