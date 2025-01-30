#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>

#define Max_input 1024
#define Max_ARGSIZE 100

void display_prompt();
void parse_input(char* input, char **args, int *is_background, char ** input_file ,char** output_file , int *append);
void execute_command(char **args, int is_background, char *input_file, char *output_file, int append);
void handle_internal_commands(char **args,char *output_file,int append);
void list_directory(char *directory);
void list_enviroment();
void set_environment_variable(char * variable,char *value);
void display_help();
void pause_Shell();

int main(int argc, char *argv[]){
 char input[Max_input];
 char *args[Max_ARGSIZE];
 int is_background;
 char *input_file= NULL;
 char *output_file=NULL;
 int append=0;
 
FILE *input_str=stdin;
if(argc ==2){
input_str=fopen(argv[1],"r");
if(input_str== NULL){
perror("Error opening the batch file");
exit(1);
}
}
while(1){
if(input_str== stdin){
display_prompt();
}
if(fgets(input,Max_input,input_str)== NULL){
break;
}
input[strcspn(input,"\n")]= '\0';


parse_input(input,args,&is_background,&input_file,&output_file,&append);

if(args[0] !=NULL){
 execute_command(args,is_background,input_file,output_file,append);
}
}
if(input_str !=stdin){
fclose(input_str);
}
return 0;
}

void display_prompt(){
char cwd[1024];
getcwd(cwd,sizeof(cwd));
printf("%s>",cwd);
fflush(stdout);
}

void parse_input(char *input, char **args, int *is_background,char **input_file, char **output_file, int *append){
char *token;
int i=0;
*is_background =0;
*input_file =NULL;
*output_file= NULL;
*append=0;

token= strtok(input," ");
while(token !=NULL){
if(strcmp(token,"&")==0){
*is_background =1;
}else if(strcmp(token,"<")==0){
token= strtok(NULL," ");
*input_file= token;
}else if(strcmp(token,">")==0){
token= strtok(NULL," ");
*output_file=token;
*append= 0;
}else if(strcmp(token,">>")==0){
token=strtok(NULL," ");
*output_file=token;
*append=1;
}else{
args[i++]=token;
}
token = strtok(NULL," ");
}
args[i]= NULL;
}

void execute_command(char ** args, int is_background,char *input_file,char *output_file ,int append){
if(strcmp (args[0],"cd") ==0 ||strcmp(args[0],"dir")== 0 || strcmp(args[0],"environ") ==0 || strcmp(args[0],"pause") == 0|| strcmp(args[0],"quit") == 0||strcmp(args[0],"help")==0||strcmp(args[0],"set")==0){
handle_internal_commands(args,output_file,append);
return;
}

pid_t pid= fork();
if(pid <0){
perror("Fork failed");
return;
}
if (pid ==0){
if(input_file){
int fd= open(input_file,O_RDONLY);
if (fd <0){
perror("Error opening input file");
exit(1);
}
dup2(fd,STDIN_FILENO);
close(fd);
}
if(output_file){
int fd;
if(append){
fd= open(output_file,O_WRONLY|O_CREAT|O_APPEND,0644);
}else{
fd=open(output_file,O_WRONLY| O_CREAT|O_TRUNC,0644);
}
if(fd<0){
perror("Error opening output file");
exit(1);
}
dup2(fd, STDOUT_FILENO);
close(fd);
}
execvp(args[0],args);
perror("Command failed to execute");
exit(1);
}else{
if(!is_background){
waitpid(pid,NULL,0);
}
}
}

void handle_internal_commands(char **args, char *output_file, int append) {
int stdout_backup = dup(STDOUT_FILENO); 
int fd = -1;

if (output_file) { 
if (append) {
fd = open(output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
} else {
fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
}
if (fd < 0) {
perror("Error opening output file");
return;
}
dup2(fd, STDOUT_FILENO);
close(fd);
}

if (strcmp(args[0], "cd") == 0) {
if (args[1] == NULL) {
printf("%s\n", getenv("PWD"));
} else {
if (chdir(args[1]) == 0) {
setenv("PWD", getcwd(NULL, 0), 1);
} else {
perror("cd");
}
}
} else if (strcmp(args[0], "dir") == 0) {
list_directory(args[1] ? args[1] : ".");
} else if (strcmp(args[0], "environ") == 0) {
list_enviroment();
} else if (strcmp(args[0], "echo") == 0) {
for (int i = 1; args[i]; i++) {
printf("%s ", args[i]);
}
printf("\n");
} else if (strcmp(args[0], "help") == 0) {
display_help();
} else if (strcmp(args[0], "pause") == 0) {
pause_Shell();
} else if (strcmp(args[0], "quit") == 0) {
exit(0);
}
}
void list_directory(char *directory){
DIR *dir=opendir(directory);
if(dir== NULL){
perror("dir");
return;
}
struct dirent *entry;
while((entry= readdir(dir))!=NULL){
printf("%s\n",entry->d_name);
}
closedir(dir);
}

void list_enviroment(){
extern char **environ;
for(char **env=environ; *env; env++){
printf("%s\n",*env);
}
}
void set_environment_variable(char *variable, char *value){
if(setenv(variable, value,1)==0){
	printf("Environment variable '%s' set to '%s'\n",variable,value);
}else{
	perror("Failed to set enviroment variable");
}
}
void display_help(){
printf("Shell Help:\n");
printf("cd [DIR]- change the directory \n");
printf("dir[DIR]-List directory content\n");
printf("environ - show environment variables \n");
printf("set VAR VALUE - set environment variable \n");
printf("ehco[TEXT]- Display the text\n");
printf("pause - Pause until Enter is pressed \n");
printf("quit- Exit shell\n");
}
void pause_Shell(){
printf("Press Enter to unpause...");
getchar();
}

