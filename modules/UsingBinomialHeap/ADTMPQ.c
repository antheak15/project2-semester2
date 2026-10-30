///////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT MPQ μέσω Binomial Heap
//
///////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdbool.h>
#include "ADTMPQ.h"

struct tree{//δομή του κάθε δέντρου που βρίσκεται στο forest
	Pointer value;//αποθηκεύει την τιμής της ρίζας του δέντρου
	Vector children;//vector με τα φύλλα-παιδιά , ένα για κάθε τάξη
};

typedef struct tree* Tree;
// Ενα MPQ είναι pointer σε αυτό το struct
struct mpq {
	Vector forest;//το δάσος που αποθηκεύει τα binomial trees
	int size;//πλήθος στοιχείων της δομής mpq
	CompareFunc compare;
	DestroyFunc destroy_value;
};

static Tree tree_create(Pointer value){//βοηθητική συνάρτηση που δημιουργεί ένα δέντρο τάξης 0, δηλ. μόνο με ρίζα
	Tree tree=malloc(sizeof(*tree));
	if(tree==NULL)
		return NULL;

	tree->value=value;
	tree->children=vector_create(0,NULL);

	if(tree->children==NULL){
		free(tree);
		return NULL;
	}

	return tree;
}

static Tree merge_trees(Tree tree1, Tree tree2, CompareFunc compare){//βοηθητική συνάρτηση που συγχωνεύει δύο binomial trees ίδιας τάξης
	//η ρίζα με τη μεγαλύτερη τιμή παραμένει ρίζα και το άλλο tree προσθείτεται στα παιδιά της
	if(compare(tree1->value,tree2->value)>=0){
		vector_insert_last(tree1->children,tree2);
		return tree1;
	}

	else{
		vector_insert_last(tree2->children,tree1);
		return tree2;
	}
	
}

static void add_tree(MPQ mpq, Tree tree){//βοηθητική συνάρτηση που εντάσσει ένα δέντρο στο δάσος
	int pos=vector_size(tree->children);//βρίσκω την τάξη του δέντρου

		while(true){
		if(vector_size(mpq->forest)<=pos)//αν το forest δεν έχει θέση για αυτη την τάξη προσθέτουμε μια κενή θέση
			vector_insert_last(mpq->forest,NULL);

		if(vector_get_at(mpq->forest,pos)==NULL ){//αν η θέση του vector είναι άδεια τοποθετώ το νέο tree
			vector_set_at(mpq->forest,pos,tree);
			break;//και το loop σταματά
		}
		
		else{//αν υπάρχει δέντρο ίδιας τάξης στο δάσος ,τα συγχωνεύουμε
			tree=merge_trees(vector_get_at(mpq->forest,pos),tree,mpq->compare);
			vector_set_at(mpq->forest,pos,NULL);//αδειάζουμε την συγκεκριμένη θέση αφού το νέο δέντρο δεν ανήκει στη τάξη της συγκεκριμένης θέσης
			pos++;//αυξάνουμε τη θέση που θα εισαχθει το δέντρο μέχρι να βρεθεί η σωστή
		}
	}
}

static void destroy_tree_helper(Tree tree,DestroyFunc destroy_value){//βοηθητική συνάρτηση που καταστρέφει αναδρομικά ένα binomial tree και τις τιμές του
	if(tree==NULL)//αν το δέντρο είναι άδειο σταματά η αναδρομή
		return;

	int size=vector_size(tree->children);//βρίσκω πόσα παιδιά έχει η ρίζα
	
	for(int i=0;i<size;i++){//παίρνω κάθε παιδί του δέντρου
		Tree child=vector_get_at(tree->children,i);
		destroy_tree_helper(child,destroy_value);//και αναδρομικά το επεξεργάζομαι
	}

	if(destroy_value!=NULL)//αν υπάρχει destroy function καταστρέφουμε τη τιμή της ρίζας
		destroy_value(tree->value);

	vector_destroy(tree->children);//καταστρέφω το vector των παιδιών
	free(tree);//αποδεσμεύουμε τη δομή tree 

}

MPQ mpq_create(CompareFunc compare, DestroyFunc destroy_value) {
	MPQ mpq=malloc(sizeof(*mpq));
	if(mpq==NULL)
		return NULL;

	mpq->forest=vector_create(0,NULL);//δημιουργώ ένα κενό vector για το forest
	
	if(mpq->forest==NULL){//ελέγχω αν πέτυχε η δημιουργία του vector
		free(mpq);
		return NULL;
	}
	mpq->size=0;//αφού το vector είναι κενό αρχικά και το size της δομή είναι μηδεν
	mpq->compare=compare;
	mpq->destroy_value=destroy_value;

	return mpq;
}

int mpq_size(MPQ mpq) {
	return mpq->size;
}

Pointer mpq_max(MPQ mpq) {

	if(mpq->size==0)//αν η δομή είναι κενή δεν υπάρχει max value
		return NULL;

	int size=vector_size(mpq->forest);
	Pointer max=NULL;
	for(int i=0;i<size;i++){//παίρνω τη ρίζα του κάθε δέντρου που βρίσκεται στο forest
		Tree tree=vector_get_at(mpq->forest,i);
		if(tree==NULL)//αν η θέση είναι κενή προχωρώ
			continue;

		if(max==NULL || mpq->compare(tree->value,max)>0)//αν δεν βρέθηκε ακόμα max ή βρεθηκε μια μεγαλύτερη τιμή ενημερώνω το max
			max=tree->value;
	}

	return max;
}

void mpq_insert(MPQ mpq, Pointer value) {
	Tree tree=tree_create(value);//δημιουργώ ένα νεό δέντρο τάξης 0
	if(tree==NULL)//ελέγχω αν η δημιουργία πέτυχε
		return;
	add_tree(mpq,tree);//το εισάγω το νέο δέντρο στο forest
	mpq->size++;//αυξάνω το πλήθος στοιχείων ης δομής mpq
}

void mpq_remove_max(MPQ mpq) {
	if(mpq->size==0)//αν η δομή είναι κενή σταματά η διαδικασία
		return;

	Pointer max=mpq_max(mpq);//παίρνω την μέγιστη τιμή
	int size=vector_size(mpq->forest);
	int i=0;
	while(i<size){//βρίσκω το δέντρο που βρίσκεται το max value
		Tree tree=vector_get_at(mpq->forest,i);
		if(tree !=NULL && mpq->compare(tree->value,max)==0){
			break;
		}
		i++;
	}
	Tree max_tree=vector_get_at(mpq->forest,i);
	vector_set_at(mpq->forest,i,NULL);//αφαιρώ το συγκεκριμένο δέντρο από το δάσος
	
	for(int k=0;k<vector_size(max_tree->children);k++){//παίρνω το κάθε παιδί του δέντρου που υπήρχε το max value
		Tree child=vector_get_at(max_tree->children,k);
		add_tree(mpq,child);//και το εισάγω στο forest
		vector_set_at(max_tree->children,k,NULL);//αποσυνδέω το παιδί απο το max_tree
	}

	if(mpq->destroy_value!=NULL)
		mpq->destroy_value(max_tree->value);//καταστρέφω τη τιμή της ρίζας

	vector_destroy(max_tree->children);//καταστρέφω το vector με τα παιδιά
	free(max_tree);//αποδεσμεύω το συγκεκριμένο δέντρο
	mpq->size--;//αφού αφαιρέθηκε μια τιμή μειώνω το πλήθος των στοιχείων
	
}

void mpq_merge(MPQ mpq, MPQ mpq2) {
	int size2=vector_size(mpq2->forest);
	for(int i=0;i<size2;i++){//παίρνω κάθε δέντρο απο το δάσος του mpq2
		Tree tree=vector_get_at(mpq2->forest,i);

		if(tree!=NULL){//αν υπάρχει δέντρο ,το εισάγω στο δάσος του mpq
			add_tree(mpq,tree);
			vector_set_at(mpq2->forest,i,NULL);//αφαιρώ το συγκεκριμένο δέντρο
		}
	}
	mpq->size+=mpq2->size;//προσθέτω το πλήθος των στοιχείων του mpq2 στο mpq
	mpq2->size=0;
	vector_destroy(mpq2->forest);//καταστρέφω το vector με το δάσος του mpq2
	free(mpq2);//και αποδεσμεύω τη δομή
	
}

DestroyFunc mpq_set_destroy_value(MPQ mpq, DestroyFunc destroy_value) {
	DestroyFunc old = mpq->destroy_value;
	mpq->destroy_value = destroy_value;
	return old;
}

void mpq_destroy(MPQ mpq) {
	int size=vector_size(mpq->forest);
	for(int i=0;i<size;i++){//διατρέχω όλα τα δέντα του forest
		Tree tree=vector_get_at(mpq->forest,i);
		if(tree!=NULL){//αν υπάρχει δέντρο το καταστρέφω αναδρομικά
			destroy_tree_helper(tree,mpq->destroy_value);
		}
	}
	vector_destroy(mpq->forest);//καταστρέφω το vector του forest
	free(mpq);//και αποδεσμεύω τη δομή mpq
}
