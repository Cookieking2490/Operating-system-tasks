#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define MAX_PROCESSES 100
#define MAX_RESOURCES 100
int P,R;
int E[MAX_RESOURCES];
int C[MAX_PROCESSES][MAX_RESOURCES];
int RQ[MAX_PROCESSES][MAX_RESOURCES];
int available[MAX_RESOURCES];
bool finish[MAX_PROCESSES];
void readInput(const char *filename) {
FILE *file = fopen(filename, "r");
if (!file) {
printf("Error opening file.\n");
exit(1);
}
fscanf(file, "%d %d", &P, &R);
for (int i = 0; i < R; i++)
fscanf(file, "%d", &E[i]);
for (int i = 0; i < P; i++)
for (int j = 0; j < R; j++)
fscanf(file, "%d", &C[i][j]);
for (int i = 0; i < P; i++)
for (int j = 0; j < R; j++)
fscanf(file, "%d", &RQ[i][j]);
fclose(file);
}
void calculateAvailable() {
for (int i = 0; i < R; i++) {
int allocated = 0;
for (int j = 0; j < P; j++) {
allocated += C[j][i];
}
available[i] = E[i] - allocated;
}
}
void detectDeadlock() {
int work[MAX_RESOURCES];
bool deadlocked[MAX_PROCESSES] = {false};
for (int i = 0; i < R; i++)
work[i] = available[i];
for (int i = 0; i < P; i++)
finish[i] = false;
bool progress = true;
while (progress) {
progress = false;
for (int i = 0; i < P; i++) {
if (!finish[i]) {
bool canFinish = true;
for (int j = 0; j < R; j++) {
if (RQ[i][j] > work[j]) {
canFinish = false;
break;
}
}
if (canFinish) {
for (int j = 0; j < R; j++)
work[j] += C[i][j];
finish[i] = true;
progress = true;
}
}
}
}
bool deadlockExists = false;
printf("Deadlocked processes: ");
for (int i = 0; i < P; i++) {
if (!finish[i]) {
deadlockExists = true;
printf("P%d ", i);
}
}
if (!deadlockExists)
printf("None (No Deadlock Detected)\n");
else
printf("\n");
}
int main() {
readInput("input1.txt");
calculateAvailable();
detectDeadlock();
return 0;
}

