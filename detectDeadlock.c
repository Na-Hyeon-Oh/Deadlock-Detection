#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX 10000

int* stack; // Stack for tracing sequence of process
int top; // Stack top

int numProcess; // # of Processes
int numRsrcType; // # of Resource Types
int* numRsrcUnit; // # of each resource types' Resource Unit
int** allocation; // Allocation Matrix
int** request; // Request Matrix
int* available; // Remained Available # of units
int* possibleProcess;
int deadlock=0; // If 1, deadlock states. If -1, not.
int* deadlockedProcess; // Deadlocked Process List // 0 : normal, 1 : block

int set(char* input);
void checkInput(void);
int detectDeadlock(void);
void push(int flag);
void printResult(void);
void delete(void);


int main(int argc, char* argv[]){
	// Set Process and Resource Information from Input File
	if(set(argv[1])<0) {
		perror("> Error in Input Setting");
		return 0;
	}
	checkInput();//^
	
	// find Deadlock
	if(numProcess!=0 && numRsrcType!=0){
		if(detectDeadlock()<0){
			perror("> Error in Detection");
			return 0;
		}
	}
	else{
		deadlock=-1;
	}
	
	// Print Output
	printResult();
	
	// Delete ALL Usage of Memory
	delete();
	
	return 0;
}

int detectDeadlock(){
	int i, j;
	int pushflag=0;

	puts("< S T A C K  Trace >");
	//until the end of detection
	while(deadlock==0){
		for(i=0; i<numProcess; i++){
			//find next process
			if(possibleProcess[i]!=0 || (top!=-1 && possibleProcess[i]==top+1)) continue;

			for(j=0; j<numRsrcType; j++){
				if(available[j] < request[i][j]){
					break;
				}
				else if(j == numRsrcType-1) {
					pushflag=1;
				}
			}

			if(pushflag==1){
				printf("push(top : %d, element : %d)  ", top+1, i);//^
				push(i);
				pushflag=0;
				break;
			}
		}

		if(i==numProcess) {
			// result1 : deadlock
			deadlock=1;
			
			/*
			//deadlock state processes
			for(j=0; j<numProcess; j++){
				if(deadlockedProcess[j]==1){
					for(k=0; k<numRsrcType; k++){
						if(allocation[j][k]!=0) {
							break;
						}
						else if(k==numRsrcType-1) deadlockedProcess[j]=0;
					}
				}
			}
			*/
			break;
		}

		// result2 : not deadlock
		if(top==(numProcess-1)) {
			deadlock=-1;
			break;
		}
	}

	puts("\n");//^

}

void push(int pid){
	// Stack is full
	if(top>=numProcess-1) {
		perror("Stack over.");
	}

	// Stack push
	else{
		int a;

		top++;
		stack[top]=pid;

		deadlockedProcess[pid]=0;
		possibleProcess[pid]=top+1;
		for(a=0;a<numRsrcType;a++){
			available[a] += allocation[pid][a];
		}
	}
}

int set(char* string){
	/* Set input file */
	char* input_file_name=string;
	FILE* input=fopen(input_file_name, "r");
	if(!input){
		perror("Failed to open input file. Terminate");
		exit(1);
	}
	
	/* read input file to get process&resource information */
	numProcess=0;
	numRsrcType=0;
	int i=0, j=0;
	int errCheck;
	char line[MAX];
	char* ptr;
	int flag=0;
	
	memset(line, 0, sizeof(char)*MAX);
	if(fgets(line, MAX, input)==NULL){
		puts("Insufficient Input.");
		return -1;
	}

	//# of Processes
	if((ptr=strtok(line, " "))!=NULL){
		numProcess=atoi(ptr);
		if(numProcess<0) {
			puts("Input number of Process is wrong.");
			return -1;
		}
		else if(numProcess==0) {
			puts("No Process.");
		}
	}
	else{
	       puts("Insufficient Input.");
       	       return -1;
	}

	//# of Resource Type
	if((ptr=strtok(NULL," "))!=NULL){
		numRsrcType=atoi(ptr);
		if(numRsrcType<0) {
			puts("Input number of Resource Type is wrong.");
			return -1;
		}
		else if(numRsrcType==0) {
			puts("No Resource.");
		}
	}
	else {
		puts("Insufficient Input.");
		return -1;
	}

	//# of each Resourc types' Resource Unit
	if(numRsrcType==0 && (ptr=strtok(NULL, " "))!=NULL) {
		puts("Input number of Resource Unit SHOULD NOT exist.");
		return -1;
	}
	else{	
		numRsrcUnit=(int*)malloc(sizeof(int)*numRsrcType);
		for(i=0;i<numRsrcType;i++){
			if(numRsrcType!=0){ 
				if((ptr=strtok(NULL," "))==NULL){
		       			puts("Insufficient Input.");  
					free(numRsrcUnit); 
					return -1;        
			       	}
		       	}

			numRsrcUnit[i]=atoi(ptr);
			if(numRsrcUnit[i]<=0){
				puts("Input number of Resource Unit is wrong.");
				free(numRsrcUnit);
				return -1;
			}
		}
	}

	if((ptr=strtok(NULL, " "))!=NULL) {
		puts("Too much Input.");
		return -1;
	}

	if(numProcess==0 || numRsrcType==0) return 0;


	//Allocation Matrix
	allocation=(int**)malloc(numProcess*sizeof(int*));
	for(i=0;i<numProcess;i++){
		allocation[i]=(int*)calloc(numRsrcType, sizeof(int));
	}
	for(i=0;i<numProcess;i++){
		memset(line, 0, sizeof(char)*MAX);

		if(fgets(line, MAX, input)==NULL){
			puts("Insufficient Input in Allocation Matrix.");
			free(numRsrcUnit);
			for(j=0;j<numProcess;j++){
				free(allocation[j]);
			}
			free(allocation);
			return -1;
		}

		for(j=0;j<numRsrcType;j++){
			if(j==0){
				if((ptr=strtok(line, " "))==NULL){
					puts("Insufficient Input in Allocation Matrix.");
					free(numRsrcUnit);
					for(int k=0;k<numProcess;k++){
						free(allocation[k]);
					}
					free(allocation);
					return -1;
				}
			}
			else{
				if((ptr=strtok(NULL, " "))==NULL){
					puts("Insufficient Input in Allocation Matrix.");
					free(numRsrcUnit);
					for(int k=0;k<numProcess;k++){
						free(allocation[k]);
					}
					free(allocation);
					return -1;
				}
			}
			
			allocation[i][j]=atoi(ptr);
			if(allocation[i][j]<0){
					puts("Input number of Allocation Matrix is wrong.");
					free(numRsrcUnit);
					for(int k=0;k<numProcess;k++){
						free(allocation[k]);
					}
					free(allocation);
					return -1;
			}
		}

		if((ptr=strtok(NULL, " "))!=NULL){
			puts("Too much Input in Allocation Matrix.");
			free(numRsrcUnit);
			for(int k=0;k<numProcess;k++){
				free(allocation[k]);
			}
			free(allocation);
			return -1;
		}
	}

	//Request Matrix
	request=(int**)malloc(numProcess*sizeof(int*));
	for(i=0;i<numProcess;i++){
		request[i]=(int*)calloc(numRsrcType, sizeof(int));
	}
	for(i=0;i<numProcess;i++){
		if(fgets(line, sizeof(line), input)==NULL){
			puts("Insufficient Input in Request Matrix.");
			free(numRsrcUnit);
			for(int k=0;k<numProcess;k++){
				free(allocation[k]);
				free(request[k]);
			}
			free(allocation);
			free(request);
			return -1;
		}
		for(j=0;j<numRsrcType;j++){
			if(j==0){
				if((ptr=strtok(line, " "))==NULL){
					puts("Insufficient Input in Request Matrix.");
					free(numRsrcUnit);
					for(int k=0;k<numProcess;k++){
						free(allocation[k]);
						free(request[k]);
					}
					free(allocation);
					free(request);
					return -1;
				}
			}
			else{
				if((ptr=strtok(NULL, " "))==NULL){
					puts("Insufficient Input in Request Matrix.");
					free(numRsrcUnit);
					for(int k=0;k<numProcess;k++){
						free(allocation[k]);
						free(request[k]);
					}
					free(allocation);
					free(request);
					return -1;
				}
			}

			request[i][j]=atoi(ptr);
			if(request[i][j]<0){
				puts("Input number of Request Matrix is wrong.");
				free(numRsrcUnit);
				for(int k=0;k<numProcess;k++){ 
					free(allocation[k]); 
					free(request[k]);
				}
				free(allocation);
				free(request);
				return -1;
			}
		}
		
		if((ptr=strtok(NULL, " "))!=NULL){
			puts("Too much Input in Request Matrix.");
			free(numRsrcUnit);
			for(int k=0;k<numProcess;k++){
				free(allocation[k]);
				free(request[k]);
			}
			free(allocation);
			free(request);
			return -1;
		}
	}

	if(fscanf(input, "%d", &errCheck)!=EOF) {
       		puts("Too much Input for matrix.");
		free(numRsrcUnit);
		for(int k=0;k<numProcess;k++){
			free(allocation[k]);
			free(request[k]);
		}
		free(allocation);
		free(request);
       		return -1;
	}


	//Stack
	stack=(int*)malloc(numProcess*sizeof(int));
	top=-1;

	//Available resource unit
	available=(int*)malloc(numRsrcType*sizeof(int));
	int tmpSum;

	//Possible Process
	possibleProcess=(int*)malloc(numProcess*sizeof(int));
	
	//Deadlocked process list
	deadlockedProcess=(int*)malloc(sizeof(int)*numProcess); 

	//initialization
	for(i=0;i<numProcess;i++){
		stack[i]=-1;
		possibleProcess[i]=0;
		deadlockedProcess[i]=1;         
	}

	for(i=0;i<numRsrcType;i++){
		tmpSum=0;
		for(j=0;j<numProcess;j++){
			tmpSum+=allocation[j][i];
		}
		available[i]=numRsrcUnit[i]-tmpSum;
	}


	return 0;
}

void checkInput(){
	puts("");
	puts("< I N P U T >");

	int i=0, j=0;
	printf("# of Processes : %d\n", numProcess);
	printf("# of Resource Types : %d\n", numRsrcType);
	if(numRsrcType!=0) printf("# of Resource Unit :\n");
	for(i=0;i<numRsrcType;i++){
		printf("   t%-10d | %d\n", i, numRsrcUnit[i]);
	}

	if(numProcess!=0 && numRsrcType!=0){
		printf("Allocation matrix :\n");
		printf("                | ");
		for(i=0;i<numRsrcType;i++){
			printf("R%-10d", i);
		}
		puts("");
		printf("   _____________|");
		for(i=0;i<numRsrcType;i++){
			printf("____________");
		}
		puts("");
		for(i=0;i<numProcess;i++){
			printf("    P%-10d | ", i);
			for(j=0;j<numRsrcType;j++){
				printf("%-10d ", allocation[i][j]);
			}
			puts("");
		}
	
		printf("Request matrix :\n");
		printf("                | ");
		for(i=0;i<numRsrcType;i++){
			printf("R%-10d", i);
		}
		puts("");
		printf("   _____________|");
		for(i=0;i<numRsrcType;i++){
			printf("____________");
		}
		puts("");
		for(i=0;i<numProcess;i++){
			printf("    P%-10d | ", i);
	       		for(j=0;j<numRsrcType;j++){
				printf("%-10d ", request[i][j]);
			}
			puts("");
		}

		printf("# of Available remained request units :\n");
		for(i=0;i<numRsrcType;i++){
			printf("   R%-10d | %d\n", i, available[i]);
		}
	}

	puts("");
}

void printResult(void){
	puts("---------------------------------------------------------------------------------------------------------------------------\n");
	puts("< R E S U L T >");
	printf("Is there Deadlock?   ");

	if(deadlock==-1){
		printf("NO\n");
	}
	else if(deadlock==1) {
		printf("YES  // Blocked Processes {");
		
		//print deadlocked process list
		for(int i=0;i<numProcess;i++){
			if(deadlockedProcess[i]!=0){
				printf(" P%d", i);
			}
		}
		puts(" }");
	}

	puts("");
}

void delete(){
	free(numRsrcUnit);
	if(numProcess !=0 && numRsrcType !=0){
		for(int i=0;i<numProcess;i++){
			free(allocation[i]);
			free(request[i]);
		}
		free(allocation);
		free(request);
		free(stack);
		free(available);
		free(possibleProcess);
		free(deadlockedProcess);
	}
}
