/* DUKH Attack 
 * COMP10002 Foundations of Algorithms, Semester 1, 2021
 * Skeleton code written by Shaanan Cohney, April 2021
 * 
 * Date: 01/05/2021           
 */

/****** Include libraries ******/

#include <stdio.h>
#include <stdlib.h>
/* Do NOT use the following two libraries in stage 1! */
#include <string.h>
#include <ctype.h>

/* Provides functions AES_encrypt and AES_decrypt (see the assignment spec) */
#include "aes.h"
/* Provides functions to submit your work for each stage.
 * See the definitions in a1grader.h, they are all available to use.
 * But don't submit your stages more than once... that's weird! */
#include "a1grader.h"

/****** Definitions of constants ******/

#define BOOK_LENGTH 1284         /* The maximum length of a cipher book */
#define MAX_MSG_LENGTH 1024      /* The maximum length of an encrypted message */
#define BLOCKSIZE 16             /* The length of a block (key, output) */
#define N_TIMESTEPS 20           /* number of timesteps */
#define N_OUTPUT_BLOCKS 2        /* number of output blocks */

// TODO Add your own #defines here, if needed



/****** Type definitions ******/
/* Recall that these are merely aliases, or shortcuts to their underlying types.
 * For example, block_t can be used in place of an array, length 16 (BLOCKSIZE)
 * of unsigned char, and vice versa. */

typedef char book_t[BOOK_LENGTH];     /* A cipherbook (1284 bytes) */
typedef unsigned char byte_t;         /* A byte (8 bits) */
typedef byte_t block_t[BLOCKSIZE];    /* A cipher bitset (block) (16 bytes) */
typedef byte_t msg_t[MAX_MSG_LENGTH]; /* An encrypted message (l bytes) */

// TODO Add your own type definitions here, if needed



/****** Function Prototypes ******
 * There are more functions defined in aes.h and grader.h */
// Scaffold

int read_hex_line(byte_t output[], int max_count, char *last_char);

// Hint: Variables passed by pointers should be modified in your stages' implementation!

void stage0(msg_t ciphertext, int *ciphertext_length, 
            block_t outputs[N_OUTPUT_BLOCKS], block_t timesteps[N_TIMESTEPS], 
            book_t cipherbook);
void stage1(book_t cipherbook, int *book_len);
void stage2(byte_t codebook[], int book_len, block_t outputs[N_OUTPUT_BLOCKS], 
            block_t timesteps[N_TIMESTEPS], block_t key2);
void stage3(block_t key2, block_t outputs[N_OUTPUT_BLOCKS], 
            block_t timesteps[N_TIMESTEPS], byte_t key1[], int cipher_length);
void stage4(byte_t key1[], byte_t ciphertext[], int cipher_length, 
            byte_t plaintext[]);

// TODO: Put your own function prototypes here! Recommended: separate into stages.
int match(block_t b1, block_t b2);
int xor_res(block_t a, block_t b, block_t c, int n);

/* The main function of the program */
// It is strongly suggested you do NOT modify this function.
int main(int argc, char *argv[])
{   
    //// Stage 0
    /* These will store our input from the input file */
    msg_t ciphertext;                  // encrypted message, to be decrypted in the attack
    int ciphertext_length = 0;         // length of the encrypted message
    book_t cipherbook;                 // book used to make key k2
    block_t timesteps[N_TIMESTEPS];    // timesteps used to generate outputs (hex)
    block_t outputs[N_OUTPUT_BLOCKS];  // outputs from the random number generator (hex)

    // Run your stage 0 code
    stage0(ciphertext, &ciphertext_length, outputs, timesteps, cipherbook);
    // And submit the results.  Don't delete this...
    submit_stage0(ciphertext_length, ciphertext, outputs, timesteps, cipherbook);
    
    //// Stage 1
    int book_len = 0;    // length of the cipher book after having removed punctuation
    stage1(cipherbook, &book_len);
    submit_stage1(cipherbook, book_len);

    //// Stage 2
    block_t key2;        // the key k2 (hexadecimal)
    stage2((byte_t *) cipherbook, book_len, outputs, timesteps, key2);
    submit_stage2(key2);

    //// Stage 3
    byte_t key1[MAX_MSG_LENGTH];       // the key k2 (hexadecimal)
    stage3(key2, outputs, timesteps, key1, ciphertext_length);    
    submit_stage3(key1);

    //// Stage 4
    byte_t plaintext[MAX_MSG_LENGTH];  // the plaintext output
    stage4(key1, ciphertext, ciphertext_length, plaintext);
    submit_stage4(plaintext);

    return 0;
}

/********* Scaffold Functions *********/

/* Reads a line in from stdin, converting pairs of hexadecimal (0-F) chars to
 * byte_t (0-255), storing the result into the output array, 
 * stopping after max_count values are read, or a newline is read.
 *
 * Returns the number of *bytes* read.
 * The last char read in from stdin is stored in the value pointed to by last_char.
 * If you don't need to know what last_char is, set that argument to NULL
 */


int read_hex_line(byte_t output[], int max_count, char *last_char)
{
    char hex[2];
    int count;
    for (count = 0; count < max_count; count++)
    {
        /* Consider the first character of the hex */
        hex[0] = getchar();
        if (hex[0] == '\n')
        {
            if (last_char)
            {
                *last_char = hex[0];
            }
            break;
        }
        /* Now the second */
        hex[1] = getchar();
        if (last_char)
        {
            *last_char = hex[0];
        }
        if (hex[1] == '\n')
        {
            break;
        }

        /* Convert this hex into an int and store it */
        output[count] = hex_to_int(hex); // (defined in aes.h)
    }

    return count - 1;
}

/********* Stage 0 Functions *********/

void stage0(msg_t ciphertext, int *ciphertext_length, block_t outputs[N_OUTPUT_BLOCKS], 
            block_t timesteps[N_TIMESTEPS], book_t cipherbook) 
{
    // TODO: Implement stage 0!
    
    /* !! Submission Instructions !! Store your results in the variables:
     *      ciphertext, ciphertext_length, outputs, timesteps, cipherbook
     * These are passed to submit_stage0 for some useful output and submission. */

    // Read in length of ciphertext
    scanf("%d", &*ciphertext_length);

    // getchar() used to dispose of "\n"
    getchar();

    // Read in line ciphertext
    read_hex_line(ciphertext ,*ciphertext_length, NULL);
    getchar();

    // Read in known outputs
    read_hex_line(*outputs, N_OUTPUT_BLOCKS*BLOCKSIZE, NULL);
    getchar();

    // Read in time steps
    read_hex_line(*timesteps, N_TIMESTEPS*BLOCKSIZE, NULL);

    // Read in cipherbook
    while (getchar() != EOF){
        for (int i = 0; i< BOOK_LENGTH; i++ ){
            cipherbook[i] = getchar();
        }
    }
}

// TODO: Add functions here, if needed.

/********* Stage 1 Functions *********/
// Reminder: you *cannot* use string.h or ctype.h for this stage!

void stage1(book_t cipherbook, int *book_len) 
{
    // TODO: Implement stage 1!

    /* !! Submission Instructions !! Store your results in the variables:
     *      cipherbook, book_len
     * These are passed to submit_stage1 for some useful output and submission. */
    
    // Initialise two pointers to cipherbook
    char* first = cipherbook;
    char* second = cipherbook;

    // Loop cipherbook until the end of original string
    while(*second != '\0'){

        // If second pointer finds a non alphanumeric, moves one forward
        while(*second == '\n' || *second == ' ' || (((*second < 'A') || (*second > 'Z')) &&
            ((*second < 'a') || (*second > 'z')))){
            if(*second == '\0'){
                break;
            }
            second++; 
        }

        // Copy the element of the second pointer to the first pointer
        *first = *second;
      
        // Move both pointers forward by 1
        first++;
        second++;
    }
    
    // Non alphanumeric removed, change ending of string
    first++;
    *first = 0;

    // Count the length of new cipherbook
    while (cipherbook[*book_len] != '\0'){
        *book_len = *book_len +1;
    }
  
}

// TODO: Add functions here, if needed.

/********* Stage 2 Functions *********/

void stage2(byte_t codebook[], int book_len, block_t outputs[N_OUTPUT_BLOCKS], 
            block_t timesteps[N_TIMESTEPS], block_t key2) 
{
    // TODO: Implement stage 2!
    block_t temp1; // AES(T_9)
    block_t temp2; // AES(T_10)
    block_t temp3; // AES-1(O_10)
    block_t temp4; // AES-1(xor_left)
    block_t xor_right; // O_9 ^ AES(T_9)
    block_t xor_left; // AES-1(O_10) ^ AES(T_9)


    /* !! Submission Instructions !! Store your results in the variable:
     *      key2
     * These will be passed to submit_stage2 to let you see some useful output! */
 
    for (int i = 0; i < book_len; i+=BLOCKSIZE){
        // Result of comparing the LHS vs RHS
        int res;

        // RHS
        AES_encrypt(timesteps[9], &codebook[i], temp1);
        xor_res(outputs[0], temp1, xor_right, BLOCKSIZE);

        //LHS
        AES_encrypt(timesteps[10], &codebook[i], temp2);
        AES_decrypt(outputs[1], &codebook[i], temp3);
        xor_res(temp3, temp2, xor_left,BLOCKSIZE);
        AES_decrypt(xor_left, &codebook[i], temp4);
            
        
        // Check for matching block  
        res = match(xor_right, temp4);

        // Generate key2

        // Key2 is generate if LHS == RHS
        if (res == 1){
            for (int j = 0; j<BLOCKSIZE; j++){
                key2[j] = codebook[i];
                i++;
            }
        }
    }

}

// TODO: Add functions here, if needed.\

// Comparing two blocks (array[16])

int match(block_t b1, block_t b2){
    int i = 0;

    while (i <BLOCKSIZE){

        // If pointer to array A is the same as pointer to array B
        // Move both forward
        if (*b1 == *b2){
            b1++;
            b2++;
        }

        // If different, return ) and exist the loop
        else if (*b1 != *b2){
            return 0;
        }

        i++;
    }
    return 1;
}


// XOR function
int xor_res(block_t a, block_t b, block_t c, int n){

    // Xor each character of two blocks, a and b
    for (int i = 0; i<n; i++){
        c[i] = a[i]^b[i];
    }
    return 0;
}


/********* Stage 3 Functions *********/


void stage3(block_t key2, block_t outputs[N_OUTPUT_BLOCKS], 
            block_t timesteps[N_TIMESTEPS], byte_t key1[], int ciphertext_length) 
{
    // TODO: Implement stage 3!
    
    block_t vi; // V_i
    block_t ii; // I_i
    block_t output1; // O_11+
    block_t temp1; // AES(T_10)
    block_t temp2; // O_10 ^ AES(T_10)
    block_t temp_xor1; // I_i ^ V_i-1
    block_t temp_xor2; // O_i ^ I_i
    int n = 0; // index of key1

    /* !! Submission Instructions !! Store your results in the variable:
     *      key1
     * These will be passed to submit_stage3 to let you see some useful output! */

    // Calculate V_10
    AES_encrypt(timesteps[10], key2, temp1);
    xor_res(outputs[1], temp1, temp2, BLOCKSIZE);
    AES_encrypt(temp2, key2, vi);

    // Loop through ciphertext length/ block size 
    // Generate output of block size each iteration
    
    for (int i = 0; i<(ciphertext_length/BLOCKSIZE); i++){
        AES_encrypt(timesteps[11+i], key2, ii);
        xor_res(ii, vi, temp_xor1, BLOCKSIZE);
        AES_encrypt(temp_xor1, key2, output1);
        xor_res(output1, ii, temp_xor2, BLOCKSIZE);
        AES_encrypt(temp_xor2, key2, vi);
        
        // Store outputs in key1
        // n keeps track of key1 index where previous outputs are stored in
        for(int j =0; j<BLOCKSIZE;j++){
            key1[n] = output1[j];
            n++;
        }
    } 
    
}

// TODO: Add functions here, if needed.


/********* Stage 4 Functions *********/
void stage4(byte_t key1[], byte_t ciphertext[], int cipher_length, byte_t plaintext[])
{
    // TODO: Implement stage 4!

    /* !! Submission Instructions !! Store your results in the variable:
     *      plaintext
     * These will be passed to submit_stage4 to let you see some useful output! */
    
    // Xor key1 with ciphertext to generate plain text, i.e. message
    xor_res(key1, ciphertext, plaintext, cipher_length);
}

// TODO: Add functions here, if needed.

/********* END OF ASSIGNMENT! *********/
/* If you would like to try the bonus stage, attempt it in a new file, bonus.c */
// Feel free to write a comment to the marker or the lecturer below...

//algorithms are awesome
