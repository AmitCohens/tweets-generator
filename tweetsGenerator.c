/**
//@author Created by Amit Cohen , id 315147330 .
//@date on 28/10/2021.
//@version 1.3.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORDS_IN_SENTENCE_GENERATION 20
#define MAX_WORD_LENGTH 100
#define MAX_SENTENCE_LENGTH 1000

typedef struct WordStruct {
    char * word;
    struct WordProbability * prob_list;
    int size_of_prob_list;

}WordStruct;

typedef struct WordProbability {
    struct WordStruct * word_struct_ptr;
    int number_of_time_word_repeated;
}WordProbability;

/**** LINKED LIST ****/
typedef struct Node {
    WordStruct * data;
    struct Node * next;
} Node;

typedef struct LinkList {
    Node * first;
    Node * last;
    int size;
}LinkList;

/*****The signatures of the functions.*****/
int add(LinkList * link_list, WordStruct * data);
int get_random_number(int max_number);
WordStruct * get_first_random_word(LinkList * dictionary);
WordStruct * get_next_random_word(WordStruct * word_struct_ptr);
int generate_sentence(LinkList * dictionary);
int add_word_to_probability_list(WordStruct * first_word, WordStruct * second_word);
void fill_dictionary(FILE * fp, int words_to_read, LinkList * dictionary);
void free_dictionary(LinkList * dictionary);

/**private functions**/
int amount_of_words_in_string(const char * sentence);
void find_the_minimum_sentence(char * sentence, int numOfWord, int * len);
void remove_space(char * sentence);
WordStruct * new_word_struct(char * word);
WordStruct * word_exist(LinkList * dic, char * word);
void allocation_failure(char * Error);
int find_word_in_prob_list(WordStruct * ws, char * word_to_check);
void read_from_file(int * words_counter, FILE * fp, char * buffer);
void build_the_dictionary(LinkList * dic, char * source);
void print_generate_sentences(LinkList * list,int amount_of_sentences);

/**
 * @param argc
 * @param argv 1) Seed
 *             2) Number of sentences to generate
 *             3) Path to file
 *             4) Optional - Number of words to read
 */
int main(int argc, char * argv[]) {
    if(argc>5||argc<4){
        fprintf(stdout,"Usage: Enter four parameters:\n1) Value of SEED.\n2) The number of numbers to create.\n3) The path of the readable file.\n4) Optional, the amount of words read from the text. ");
        exit(EXIT_FAILURE);
    }
    int seed=atoi(argv[1]),amount_of_words,amount_of_sentences=atoi(argv[2]);
    if(argc==4)
        amount_of_words=-1;
    else
        amount_of_words=atoi(argv[4]);
    srand(seed);
    char path[1000];
    strcpy(path,argv[3]);
    FILE * file = fopen(path, "r");
    if(file==NULL){
        fprintf(stdout,"Error: The file did not open properly.");
        exit(EXIT_FAILURE);
    }
    LinkList * linkList = (LinkList * ) malloc(sizeof(LinkList));
    if (linkList == NULL)
        allocation_failure("of linkList");
    fill_dictionary(file,amount_of_words, linkList);
    print_generate_sentences(linkList,amount_of_sentences);
    free_dictionary(linkList);
    fclose(file);
}
/**
 * Add data to new node at the end of the given link list.
 * @param link_list Link list to add data to
 * @param data pointer to dynamically allocated data
 * @return 0 on success, 1 otherwise
 */
int add(LinkList * link_list, WordStruct * data) {
    Node * new_node = malloc(sizeof(Node));
    if (new_node == NULL) {
        return 1;
    }
    * new_node = (Node) {
            data,
            NULL
    };

    if (link_list -> first == NULL) {
        link_list -> first = new_node;
        link_list -> last = new_node;
    } else {
        link_list -> last -> next = new_node;
        link_list -> last = new_node;
    }

    link_list -> size++;
    return 0;
}
/**
 * Get random number between 0 and max_number [0, max_number).
 * @param max_number
 * @return Random number
 */
int get_random_number(int max_number) {
    return rand() % max_number;
}
/**
 * Choose randomly the next word from the given dictionary, drawn uniformly.
 * The function won't return a word that end's in full stop '.' (point).
 * @param dictionary Dictionary to choose a word from
 * @return WordStruct of the chosen word
 */
WordStruct * get_first_random_word(LinkList * dictionary) {
    Node * pointer = NULL;
    WordProbability * firstWords = (WordProbability * ) malloc(sizeof(WordProbability));
    if (firstWords == NULL)
        allocation_failure("of WordProbability.");
    pointer = dictionary -> first;
    int i = 0;
    while (pointer != NULL) {
        if (pointer -> data -> word[strlen(pointer -> data -> word) - 1] != '.') {
            firstWords = realloc(firstWords, sizeof(WordProbability) * (i + 1));
            firstWords[i].word_struct_ptr = pointer -> data;
            i++;
        }
        pointer = pointer -> next;
    }
    int random = get_random_number(i);
    pointer = dictionary -> first;
    while (pointer != NULL) {
        if (strcmp(firstWords[random].word_struct_ptr -> word, pointer -> data -> word) == 0) {
            free(firstWords);
            firstWords=NULL;
            return pointer -> data;
        }
        pointer = pointer -> next;
    }
    free(firstWords);
    firstWords = NULL;
    return NULL;
}
/**
 * Choose randomly the next word. Depend on it's occurrence frequency
 * in word_struct_ptr->WordProbability.
 * @param word_struct_ptr WordStruct to choose from
 * @return WordStruct of the chosen word
 */
WordStruct * get_next_random_word(WordStruct * word_struct_ptr) {
    int *a=(int *) malloc(sizeof (int )*word_struct_ptr -> size_of_prob_list);
    if(a==NULL)
        allocation_failure("Error");
    for (int i = 0; i < word_struct_ptr -> size_of_prob_list; i++) {
        a[i] = word_struct_ptr -> prob_list[i].number_of_time_word_repeated;
        if (i > 0)
            a[i] = a[i] + a[i - 1];
    }
    int random = get_random_number(a[word_struct_ptr -> size_of_prob_list - 1]);
    for (int i = 0; i < word_struct_ptr -> size_of_prob_list; i++) {
        if (i == 0) {
            if (random < a[i]) {
                free(a);
                a=NULL;
                return word_struct_ptr->prob_list[i].word_struct_ptr;
            }
        }
        if (random < a[i] && random >= a[i - 1]) {
            free(a);
            a=NULL;
            return word_struct_ptr->prob_list[i].word_struct_ptr;
        }
    }
    free(a);
    a=NULL;
    return word_struct_ptr -> prob_list[0].word_struct_ptr;
}
/**
 * Receive dictionary, generate and print to stdout random sentence out of it.
 * The sentence most have at least 2 words in it.
 * @param dictionary Dictionary to use
 * @return Amount of words in printed sentence
 */
int generate_sentence(LinkList * dictionary) {
    WordStruct * wordStruct = get_first_random_word(dictionary);
    printf("%s ", wordStruct -> word);
    int i = 1;
    for (; i < MAX_WORDS_IN_SENTENCE_GENERATION; i++) {
        wordStruct = get_next_random_word(wordStruct);
        printf("%s ", wordStruct -> word);
        if (wordStruct -> word[strlen(wordStruct -> word) - 1] == '.')
            break;
    }
    return i;
}
/**
 * Gets 2 WordStructs. If second_word in first_word's prob_list,
 * update the existing probability value.
 * Otherwise, add the second word to the prob_list of the first word.
 * @param first_word
 * @param second_word
 * @return 0 if already in list, 1 otherwise.
 */
int add_word_to_probability_list(WordStruct * first_word, WordStruct * second_word) {

    if (first_word -> prob_list == NULL) {
        first_word -> prob_list = (WordProbability * ) malloc(sizeof(WordProbability));
        if (first_word -> prob_list == NULL)
            allocation_failure("of probability list.");
        first_word -> prob_list[0].word_struct_ptr = second_word;
        first_word -> prob_list[0].number_of_time_word_repeated = 1;
        first_word -> size_of_prob_list = 1;
        return 1;
    } else {
        int check = find_word_in_prob_list(first_word, second_word -> word);
        if (check != -1) {
            first_word -> prob_list[check].number_of_time_word_repeated++;
            return 0;
        }
        first_word -> prob_list = realloc(first_word -> prob_list, (sizeof(WordProbability)) * (first_word -> size_of_prob_list + 1));
        if (first_word -> prob_list == NULL)
            allocation_failure("of Resize the probability list.");
        first_word -> size_of_prob_list++;
        first_word -> prob_list[first_word -> size_of_prob_list - 1].word_struct_ptr = second_word;
        first_word -> prob_list[first_word -> size_of_prob_list - 1].number_of_time_word_repeated++;
        return 1;
    }
}
/**
 * Read word from the given file. Add every unique word to the dictionary.
 * Also, at every iteration, update the prob_list of the previous word with
 * the value of the current word.
 * @param fp File pointer
 * @param words_to_read Number of words to read from file.
 *                      If value is bigger than the file's word count,
 *                      or if words_to_read == -1 than read entire file.
 * @param dictionary Empty dictionary to fill
 */
void fill_dictionary(FILE * fp, int words_to_read, LinkList * dictionary) {
    char sentence[MAX_SENTENCE_LENGTH];
    int counter1 = 0, sizeOfSentence = 0, counter2 = 0, size;
    while (!feof(fp)) {
        read_from_file( & sizeOfSentence, fp, sentence);
        counter1 += sizeOfSentence;
        if (counter1 >= words_to_read && words_to_read != -1) {
            if (counter1 > words_to_read) {
                words_to_read -= counter2;
                find_the_minimum_sentence(sentence, words_to_read, & size);
            }
            build_the_dictionary(dictionary, sentence);
            break;
        }
        counter2 = counter1;
        build_the_dictionary(dictionary, sentence);
    }
}
/**
 * Free the given dictionary and all of it's content from memory.
 * @param dictionary Dictionary to free
 */
void free_dictionary(LinkList * dictionary) {
    Node *current = NULL, *next = NULL;
    current = dictionary->first;
    while (current != NULL) {
        next = current->next;
        free(current->data->word);
        free(current->data->prob_list);
        free(current->data);
        free(current);
        current = next;
    }
    free(dictionary);
    dictionary = NULL;
}
/**
 * The function receives an array of characters and returns the amount of words.
 * @param sentence The string to perform the test.
 * @return The amount of words in the string.
 */
int amount_of_words_in_string(const char * sentence) {
    int counter = 0;
    if (sentence[0] != ' ' && sentence[0] != '\0')
        counter++;
    for (int i = 0; sentence[i] != '\0'; i++)
        if (sentence[i] == ' ' && sentence[i + 1] != '\0' && sentence[i + 1] != ' ')
            counter++;
    return counter;
}
/**
 * The function gets a line and shortens it to the minimum amount of sentences needed from it.
 * @param sentence The line from which the sentences are supposed to be read.
 * @param numOfWord The amount of words required to read
 * @param len Up-to-date word count.
 */
void find_the_minimum_sentence(char * sentence, int numOfWord, int * len) {
    char b[MAX_SENTENCE_LENGTH + 1], a[MAX_SENTENCE_LENGTH];
    strcpy(a, sentence);
    b[0] = '\0';
    int count = 0;
    char * ptr = NULL;
    const char s[2] = ".";
    ptr = strtok(a, s);
    strcat(b, ptr);
    strcat(b, ".");
    count += amount_of_words_in_string(ptr);
    while (ptr != NULL) {
        if (count >= numOfWord) {
            b[strlen(b)] = '\0';
            break;
        } else {
            ptr = strtok(NULL, s);
            strcat(b, ptr);
            count += amount_of_words_in_string(ptr);
            strcat(b, ".");
        }
    }
    ( * len) = (int) strlen(b);
    strcpy(sentence, b);
}
/**
 * Deleting a double space in a sentence.
 * @param sentence The sentence on which we will perform the deletion.
 */
void remove_space(char * sentence) {
    char a[MAX_SENTENCE_LENGTH];
    int indexR = 0, indexW = 0;
    if (sentence[strlen(sentence) - 1]=='\n')
         sentence[strlen(sentence) - 1] = '\0';
    else
        sentence[strlen(sentence)] = '\0';
    while (sentence[indexR] == ' ') indexR++;
    for (; sentence[indexR] != '\0'; indexR++) {
        if (sentence[indexR] == ' ' && sentence[indexR - 1] == ' ')
            continue;
        a[indexW] = sentence[indexR];
        indexW++;
    }
    a[indexW] = '\0';
    strcpy(sentence, a);
}
/**
 * Create a new word struct and insert the word into it.
 * @param word The word we enter.
 * @return The address of  the new word struct.
 */
WordStruct * new_word_struct(char * word) {
    WordStruct * wordStruct = (WordStruct * ) malloc(sizeof(WordStruct));
    if (wordStruct == NULL)
        allocation_failure("of wordStruct.");
    wordStruct -> word = (char * ) malloc(strlen(word) + 1);
    if (wordStruct -> word == NULL)
        allocation_failure("of word in wordStruct.");
    wordStruct -> prob_list = NULL;
    strcpy(wordStruct -> word, word);
    wordStruct -> size_of_prob_list = 0;
    wordStruct -> word[strlen(word)] = '\0';
    return wordStruct;
}
/**
 * Look up the word in the dictionary, if you find it we will return the word struct otherwise we will return null
 * @param dic The dictionary(link list)
 * @param word The word for dictionary search
 * @return  The address of the word struct or NULL.
 */
WordStruct * word_exist(LinkList * dic, char * word) {
    Node * ptr = dic -> first;
    while (ptr != NULL) {
        if (strcmp(ptr -> data -> word, word) == 0)
            return ptr -> data;
        ptr = ptr -> next;
    }
    return NULL;
}
/**
 * Print error and exit 1.
 * @param Error The string for printing.
 */
void allocation_failure(char * Error) {
    fprintf(stdout, "Allocation failure:%s", Error);
    exit(EXIT_FAILURE);
}
/**
 * Find a word in the list of probabilities.
 * @param ws The Word Struct where we will search for the word.
 * @param word_to_check The word we are looking for.
 * @return The index of the word within the list of probabilities, if the word is not found, we return -1.
 */
int find_word_in_prob_list(WordStruct * ws, char * word_to_check) {
    for (int i = 0; i < ws -> size_of_prob_list; i++) {
        if (strcmp(word_to_check, ws -> prob_list[i].word_struct_ptr -> word) == 0)
            return i;
    }
    return -1;
}
/**
 * Reading a line from the file.
 * @param words_counter A variable that will return the amount of words written.
 * @param fp Path of the file.
 * @param buffer A string to which the line called will be written.
 */
void read_from_file(int * words_counter, FILE * fp, char * buffer) {
    ( * words_counter) = 0;
    fgets(buffer, MAX_SENTENCE_LENGTH, fp);
    remove_space(buffer);
    ( * words_counter) = amount_of_words_in_string(buffer);
}
/**
 * Build the linked list, by breaking the strings into words and sending them to the NEW WORD STRUCT function.
 * @param dic The address of the linked list.
 * @param source The string from which the words will be disassembled.
 */
void build_the_dictionary(LinkList * dic, char * source) {
    WordStruct * wordStruct = NULL;
    WordStruct * prev = NULL;
    char * word = NULL;
    word = strtok(source, " ");
    wordStruct = word_exist(dic, word);
    if (wordStruct == NULL) {
        wordStruct = new_word_struct(word);
        add(dic, wordStruct);
    }
    prev = wordStruct;
    word = strtok(NULL, " ");
    while (word != NULL) {
        wordStruct=NULL;
        wordStruct = word_exist(dic, word);
        if (wordStruct == NULL) {
            wordStruct = new_word_struct(word);
            add(dic, wordStruct);
        }
        if (prev -> word[strlen(prev -> word) - 1] != '.')
            add_word_to_probability_list(prev, wordStruct);
        prev = wordStruct;
        word = strtok(NULL, " ");
    }
}
/**
 *Create and print the sentences / tweets from the dictionary according to the requested quantity.
 * @param list The dictionary from which the words are read.
 * @param amount_of_sentences The amount of sentences that need to be produced.
 */
void print_generate_sentences(LinkList * list,int amount_of_sentences){
    for (int i = 0; i<amount_of_sentences; i++) {
        printf("Tweet %d: ", i + 1);
        generate_sentence(list);
        printf("\r\n");
    }
}
