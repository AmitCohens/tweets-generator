Author id : 315147330

Personal details : 
Name : Amit Cohen
id : 315147330

Exercise name : tweets generator

Submitted files : 
1)tweetsGenerator.c : the code.
2)README : this file, which explains the code.


Remarks :   
1)in the struct "wordStruct" I added an integer variable, called size_of_prob_list, that holds the length of the array in the same "wordStruct".
2)in the struct "WordProbability" I added an integer variable,called number_of_time_word_repeated.

In addition to the functions I received, I wrote 10 more functions:

1)  amount_of_words_in_string-The function gets a string and returns the amount of the word it has.
2)  find_the_minimum_sentence-The function receives a line from text and returns it shorter according to the number of sentences and words in that line.
3)  remove_space-The function receives a string and returns it without duplicates of spaces.
4)  new_word_struct-The function receives a word and returns a pointer to a new wordstruct.
5)  word_exist-The function searches for a word within the dictionary and if it finds returns a pointer to its wordstruct, otherwise returns null.
6)  allocation_failure-The function prints an error to stdout and exits with EXIT_FAILURE.
7)  find_word_in_prob_list-A function that searches for a word in an array and returns its index if it is found, otherwise it returns -1.
8)  read_from_file-The function reads a line of text into a buffer.
9)  build_the_dictionary-The function receives a line of text and inserts the words into the dictionary.
10) print_generate_sentences-The function prints the tweets according to the quantity requested.


more remarks:

1)  In the "Get First Word" function I actually created an array of pointers for all the wordStructs, which have a word that does not end in a period. Then I drew a number along the length of the array and returned the word of the wordStruct found there.
2)In the "get next word" function, I counted the number of times each word appeared in the prob list, and I created an array of the same length as the "Fibonacci series" only from the number of times each word was so far and then I drew a number from 0 to the total words (including repetitions) and then checked The number I got, if it is in the range between two indexes is the right index. And the word that was there I returned.

 
