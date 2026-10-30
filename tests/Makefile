# Κάνοντας compile το <foo>_test.c με μια υλοποίηση <foo>.c του
# συγκεκριμένου τύπου, παράγουμε ένα test για την υλοποίηση αυτή.

# Για όλες τις υλοποιήσεις χρειαζόμαστε το ίδιο το test, το ADTMPQ_aux, και μια υλοποίηση του ADTVector
COMMON = ADTMPQ_test.o $(MODULES)/ADTMPQ_aux.o $(MODULES)/UsingDynamicArray/ADTVector.o

# Υλοποίηση μέσω ADTPriorityQueue
UsingADTPiorityQueue_ADTMPQ_test_OBJS = $(COMMON) $(MODULES)/UsingADTPriorityQueue/ADTMPQ.o $(MODULES)/UsingHeap/ADTPriorityQueue.o

# Υλοποίηση μέσω συνδεδεμένης λίστας
UsingLinkedList_ADTMPQ_test_OBJS      = $(COMMON) $(MODULES)/UsingLinkedList/ADTMPQ.o

# Υλοποίηση μέσω Heap
UsingHeap_ADTMPQ_test_OBJS            = $(COMMON) $(MODULES)/UsingHeap/ADTMPQ.o

# Υλοποίηση μέσω Binomial Heap
UsingBinomialHeap_ADTMPQ_test_OBJS    = $(COMMON) $(MODULES)/UsingBinomialHeap/ADTMPQ.o


# Ο βασικός κορμός του Makefile
include ../common.mk
