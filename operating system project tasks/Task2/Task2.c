#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>

#define MAX_WORD_LENGTH 100
#define MAX_SEGMENTS 10
#define MAX_WORDS 1000


typedef struct{
char word[MAX_WORD_LENGTH];
int frequency;
} WordCount;

WordCount global_word_counts[MAX_WORDS];
int global_word_count = 0;


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


void to_lowercase(char *str){
for (int i = 0; str[i]; i++){
str[i] = tolower(str[i]);
}
}


void *count_words(void *arg){
char *segment = (char *)arg;
char *token = strtok(segment, " ,.!?\n");
WordCount local_word_counts[MAX_WORDS];
int local_word_count = 0;

while (token != NULL){
to_lowercase(token);
int found = 0;
for (int i = 0; i < local_word_count; i++){
if (strcmp(local_word_counts[i].word, token) == 0){
local_word_counts[i].frequency++;
found = 1;
break;
}
}

if (!found){
strcpy(local_word_counts[local_word_count].word, token);
local_word_counts[local_word_count].frequency = 1;
local_word_count++;
}

token = strtok(NULL, " ,.!?\n");
}

    
pthread_mutex_lock(&mutex);
for (int i = 0; i < local_word_count; i++){
int found = 0;
for (int j = 0; j < global_word_count; j++){
if (strcmp(global_word_counts[j].word, local_word_counts[i].word) == 0){
global_word_counts[j].frequency += local_word_counts[i].frequency;
found = 1;
break;
}
}
if (!found){
strcpy(global_word_counts[global_word_count].word, local_word_counts[i].word);
global_word_counts[global_word_count].frequency = local_word_counts[i].frequency;
global_word_count++;
}
}
pthread_mutex_unlock(&mutex);

return NULL;
}

int main(){
FILE *file = fopen("Testfile.txt", "r");
if (!file){
perror("Failed to open file");
return 1;
}

fseek(file, 0, SEEK_END);
long file_size = ftell(file);
fseek(file, 0, SEEK_SET);
char *content = malloc(file_size + 1);
fread(content, 1, file_size, file);
content[file_size] = '\0';
fclose(file);

char *segments[MAX_SEGMENTS];
int num_segments = 0;
char *token = strtok(content, ",");
while (token != NULL && num_segments < MAX_SEGMENTS){
segments[num_segments++] = token;
token = strtok(NULL, ",");
}
pthread_t threads[num_segments];
for (int i = 0; i < num_segments; i++){
pthread_create(&threads[i], NULL, count_words, (void *)segments[i]);
}

for (int i = 0; i < num_segments; i++){
pthread_join(threads[i], NULL);
}

printf("Word Frequency:\n");
for (int i = 0; i < global_word_count; i++){
printf("%s: %d\n", global_word_counts[i].word, global_word_counts[i].frequency);
}

free(content);
pthread_mutex_destroy(&mutex);

return 0;
}
