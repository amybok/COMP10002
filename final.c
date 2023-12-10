/* Assignment 2
 * COMP10002 Foundations of Algorithms, Semester 1, 2021
 * 
 * Full Name: Gia Han Ly       
 * Student Number:  1074109
 * Date:  25/05/2021
 */

/* algorithms are awesome! 	٩(◕‿◕｡)۶ */

/****** Include libraries ******/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

/****** Definitions of constants ******/
#define MAXCHARS 25
#define WORD_MAX 22
#define FORM_MAX 100
#define POS_TAG_LENGTH 20
#define STAGE_FORMAT 26
#define DICTIONARY 100
#define BS_NOT_FOUND (-1)
#define BS_FOUND 0

/****** Type definitions ******/
typedef char word_str_t [WORD_MAX+1];
typedef char form_str_t [FORM_MAX+1];
typedef char pos_str_t [POS_TAG_LENGTH+1];

typedef struct {
    char start;
    word_str_t word;
    pos_str_t tag;
    char hash;
    form_str_t form;
} word_t;

typedef struct node node_t;

struct node {
	void *data;              /* ptr to stored structure */
	node_t *left;            /* left subtree of node */
	node_t *right;            /* right subtree of node */
};

typedef struct {
	node_t *root;            /* root node of the tree */
	int (*cmp)(void*,void*); /* function pointer */
} tree_t;

typedef struct {
	char *word;      /* pointer to a word */
    word_str_t base_form; /* base form of word if applicable */
    int base_check;    /* check for base form */
    int form_check;    /* check for variant */
    int dict_index;    /* dictionary index of variant */
    pos_str_t pos_check;    /* pos tag if applicable */
} data_t;



/****** Function Prototypes ******/
void print_head1(void);
void print_check1(word_t);
word_t read_word (void);
int variation(form_str_t);
void print_check2(double, double);
int getword(char W[], int limit);
tree_t *make_empty_tree(int func(void*,void*));
tree_t *insert_in_order(tree_t *tree, void *value);
int compare_string_parts(void *x1, void *x2);
static void *recursive_search_tree(node_t*, void*, int(void*,void*));
void *search_tree(tree_t *tree, void *key);
static node_t *recursive_insert(node_t*, node_t*, int(void*,void*));
void traverse_tree(tree_t *tree, void action(void*));
static void recursive_traverse(node_t*, void action(void*));
int binary_search(word_t A[], int lo, int hi, char *key, int *locn);
void print_head3(void);
void print_check3(int check, word_t *dict, int index, data_t * word);
int return_word(char A[], char B[], int n);
int call_return_word(char key[], char form[]);
void print_head4(void);
void print_then_free(void *x);
void free_tree(tree_t *tree);
static void recursive_free_tree(node_t*);



int main(int argc, char** argv){

    //* Stage 1 --------------------------------------------------------------------------------------------------
    word_t word1;
    word1 = read_word();
    
    //* Stage 2 --------------------------------------------------------------------------------------------------
    int total_var = 0;
    int vars;
    int nvalid = 1;
    word_t dictionary[DICTIONARY];

    dictionary[0] = word1;

    // Count number of variation in a word struct
    vars = variation(word1.form);
    // Add to total variation in dictionary
    total_var += vars;

    print_head1();
    print_check1(word1);


    for (int i = 1; i< DICTIONARY; i++){
        dictionary[i] = read_word();
        
        // If the next line is the sentence, break dictionary read
        if (dictionary[i].start == '*'){
            dictionary[i].start = '\0';
            break;
        }
        nvalid++;
        // Count number of variation in a word struct
        // Add to total variation in dictionary
        vars = variation(dictionary[i].form);
        total_var += vars;

    }

    print_check2(nvalid, total_var);

    //* Stage 3 --------------------------------------------------------------------------------------------------
    data_t *new;
    tree_t *sentence;
    char oneword[MAXCHARS+1];

    print_head3();

    // make new tree and insert in order
    sentence = make_empty_tree(compare_string_parts);
    while (getword(oneword, MAXCHARS) != EOF){
        int index;  // dictionary index of base word
        int ocheck;  // binary search result for base word
		new = malloc(sizeof(*new));
		assert(new!=NULL);
		new->word = oneword;
		/* not in tree, so create a new string */
		new->word = malloc(1+strlen(oneword));
		assert(new->word!=NULL);
		strcpy(new->word, oneword);
		

        //* Stage 4 -------------------------------------------------------------------------------------------------
		ocheck = binary_search(dictionary, 0, nvalid, new->word, &index);  
        new->base_check = ocheck;
        // set memory of pos_check to null
        memset(new->pos_check, '\0', sizeof(pos_str_t)+1);
        
        // if there's a base word, then insert relevant informations
        if (ocheck == 0){
            strcpy(new->base_form, dictionary[index].word);
            strcpy(new->pos_check, dictionary[index].tag);
        }
        // if the word is not found, set base and pos to not found
        else {
            strcpy(new->base_form, new->word);
            strcpy(new->pos_check, "NOT_FOUND");
        }
        
        
        int check4 = 0;    // check for variantions
        int dictindex = -1;    // dictindex set at -1 when there's no matching variation
        
        if (ocheck != 0){
            for (int i = 0; i<nvalid; i++){
                // check for all variations of words in dictionary
                check4 = call_return_word(new->word,dictionary[i].form); 
                //if found, insert relevant information to data
                if (check4 == 0){
                    dictindex = i;
                    strcpy(new->pos_check, dictionary[i].tag);
                    strcpy(new->base_form, dictionary[i].word);
                    new->form_check = check4;
                    new->dict_index = dictindex;
                    break;
                }
                // otherwise set at not found
                new->form_check = check4;
                new->dict_index = dictindex;
            }
        }
        /* and insert into tree */
        sentence = insert_in_order(sentence, new);
        print_check3(ocheck, dictionary, index, new);
    }
    print_head4();
    
    /* now print all the words */
    traverse_tree(sentence, print_then_free);
    /* and release the tree itself */
	free_tree(sentence);
	sentence = NULL;
	return 0;
}


//** Functions **//

//* Read in word from input
word_t read_word(void){
    word_t new_word;
    char c;
    int i = 0;
    int j = 0;

    scanf("%c %s", &new_word.start, new_word.word);
    getchar();

    // If new line is the sentence separator, return an empty word
    if (new_word.start == '*'){
        new_word.tag[0] = '\0';
        new_word.hash = 0;
        new_word.form[0] = '\0';
        return new_word;
    }
    
    // Read POS tag by characters
    while ((c = getchar())!= '\n'){
        new_word.tag[i] = c;
        i++;
    }
    // Add terminator at the end of POS 
    new_word.tag[i] = '\0';
    
    scanf("%c", &new_word.hash);

    // Read word forms by characters
    while ((c = getchar())!= '\n'){
        new_word.form[j] = c;
        j++;
    }
    // Add terminator at the end of form
    new_word.form[j] = '\0';

    return new_word;
}


//* Print output for stage 1
void print_head1(void){
    for (int i = 0; i < STAGE_FORMAT*2; i++){
        if (i == STAGE_FORMAT){
            printf ("Stage 1");
        }
        printf("=");
    }
}

void print_check1(word_t a_word){
    printf("\nWord 0: %s", a_word.word);
    printf("\nPOS: %s\n", a_word.tag);
    printf("Form: %s\n", a_word.form);
}

//* Count number of variations of a word
int variation(form_str_t form){
    int var_n = 0;
    int i = 0;

    // if encounter a number, count one variant
    while (form[i] != '\0'){
        if (isdigit(form[i])){
            var_n ++;
        }
        i++;
    }
    return var_n;
}

//* Print output for stage 2
void print_check2(double n_words, double total_vars){
    double avg_vars;

    for (int i = 0; i < STAGE_FORMAT*2; i++){
        if (i == STAGE_FORMAT){
            printf ("Stage 2");
        }
        printf("=");
    }

    printf("\nNumber of words: %.0f\n", n_words);

    avg_vars = total_vars/n_words;

    printf("Average number of variation forms per word: %.2f\n", avg_vars);

}

//* Extract a single word out of the standard input
//* Chapter 7 page 121
int getword(char W[], int limit){
	int c, len=0;
	/* first, skip over any non alphabetics */
	while ((c=getchar())!=EOF && !isalpha(c)){
		/* do nothing more */
	}
	if (c==EOF){
		return EOF;
	}
	/* ok, first character of next word has been found */
	W[len++] = c;
	while (len<limit && (c=getchar())!=EOF && isalpha(c)){
		/* another character to be stored */
		W[len++] = c;
	}
	/* now close off the string */
	W[len] = '\0';
	return 0;
}

// while getword, insert in order
// use search tree to find null location or check for frequency
//* Chapter 10

tree_t *make_empty_tree(int func(void*,void*)){
	tree_t *tree;
	tree = malloc(sizeof(*tree));
	assert(tree!=NULL);
	/* initialize tree to empty */
	tree->root = NULL;
	/* and save the supplied function pointer */
	tree->cmp = func;        
	return tree;
}

/* Returns a pointer to an altered tree that now includes
   the object "value" in its correct location. */
tree_t *insert_in_order(tree_t *tree, void *value){
	node_t *new;
	/* make the new node */
	new = malloc(sizeof(*new));
	assert(tree!=NULL && new!=NULL);
	new->data = value;
	new->left = new->right = NULL;
	/* and insert it into the tree */
	tree->root = recursive_insert(tree->root, new,
		tree->cmp);
	return tree;
}

int compare_string_parts(void *x1, void *x2){
	data_t *p1=x1, *p2=x2;
	return strcmp(p1->word, p2->word);
}

static void *recursive_search_tree(node_t *root, void *key, int cmp(void*,void*)){
	int outcome;
	if (!root){
		return NULL;
	}
	if ((outcome=cmp(key, root->data)) < 0){
		return recursive_search_tree(root->left, key, cmp);
	} 
    else if (outcome > 0){
		return recursive_search_tree(root->right, key, cmp);
	} 
    else{
		/* hey, must have found it! */
		return root->data;
	}
}

/* Returns a pointer to the tree node storing object "key",
   if it exists, otherwise returns a NULL pointer. */
void *search_tree(tree_t *tree, void *key){
	assert(tree!=NULL);
	return recursive_search_tree(tree->root, key, tree->cmp);
}

static node_t *recursive_insert(node_t *root, node_t *new, int cmp(void*,void*)){
	if (root==NULL){
		return new;
	} 
    else if (cmp(new->data, root->data) < 0){
		root->left = recursive_insert(root->left, new, cmp);
	} 
    else{
		root->right = recursive_insert(root->right, new, cmp);
	}
	return root;
}


static void recursive_traverse(node_t *root, void action(void*)){
	if (root){
		recursive_traverse(root->right, action);
		action(root->data);
		recursive_traverse(root->left, action);
	}
}

/* Applies the "action" at every node in the tree, in
   the order determined by the cmp function. */
void traverse_tree(tree_t *tree, void action(void*)){
	assert(tree!=NULL);
	recursive_traverse(tree->root, action);
}

//* Chapter 12 page 206
int binary_search(word_t A[], int lo, int hi, char *key, int *locn){
	int mid, outcome;
	/* if key is in A, it is between A[lo] and A[hi-1] */
	if (lo>=hi){
		return BS_NOT_FOUND;
	}
	mid = (lo+hi)/2;
	if ((outcome = strcmp(key, A[mid].word)) < 0){
		return binary_search(A, lo, mid, key, locn);
	} 
    else if (outcome > 0){
		return binary_search(A, mid+1, hi, key, locn);
	} 
    else{
		*locn = mid;
		return BS_FOUND;
	}
}

void print_head3(void){
    for (int i = 0; i < STAGE_FORMAT*2; i++){
        if (i == STAGE_FORMAT){
            printf ("Stage 3");
        }
        printf("=");
    }
    printf("\n");
}

void print_check3(int check, word_t *dict, int index, data_t * word){
    if (check == 0){
        printf("%-26s%s\n", word->word, dict[index].tag);
    }
    else{
        printf("%-26sNOT_FOUND\n", word->word);
    }

}


int call_return_word(char key[], char form[]){
    int size_form;
    size_form = strlen(form);
    char temp[MAXCHARS];
    int n = 0; // index in pos_tag array
    int j=0;
    int check;
    while (j< size_form){
        if (form[j] == '\0'){
            break;
        }
        memset(temp, '\0', sizeof(temp));
        n = return_word(form,temp,n);
        // compare key word and variant found 
        check = strcmp(key, temp);
        if (check == 0){
            return check;
        }
        j = n;
    }
    return check;
}



int return_word(char A[], char B[], int n){
    int i = n;
    char *ptr_a = A;
    
    // moving past variants already checked
    ptr_a+=n;
    
    // need to move pointer to the start of the new variant
    if (isdigit(*ptr_a)){
        ptr_a++;
    } 
    else {
        i++;
        ptr_a+=2;
    }
    
    // count number of chars in variant
    while (!isdigit(*ptr_a) && *ptr_a != '\0'){
        n++;
        ptr_a++;
    }
    
    // copy string (variant)
    int j = 0;
    while (i<n+1){
        if (isdigit(A[i+1])){
            break;
        }
        B[j] = A[i+1];
        j++;
        i++;
    }
    return i;
}

void print_head4(void){
    for (int i = 0; i < STAGE_FORMAT*2; i++){
        if (i == STAGE_FORMAT){
            printf ("Stage 4");
        }
        printf("=");
    }
    printf("\n");
}

void print_then_free(void *x){
	data_t *p=x;
	printf("%-26s%-26s%s\n", p->word, p->base_form, p->pos_check);
	free(p->word);
	free(p);
}

static void
recursive_free_tree(node_t *root){
	if (root){
		recursive_free_tree(root->left);
		recursive_free_tree(root->right);
		free(root);
	}
}


/* Release all memory space associated with the tree
   structure. */
void free_tree(tree_t *tree){
	assert(tree!=NULL);
	recursive_free_tree(tree->root);
	free(tree);
}