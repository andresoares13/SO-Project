#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/mman.h>

int main(int argc, char* argv[]){
  if (argc<3  || argc>4 || (argc==3 && (!strstr(argv[1],".txt")||!strstr(argv[1],".txt")))){
    printf("%s","usage: addmx file1 file2\n");
    return 1;
  }
  char l1[10];
  char l2[10];
  char collmuns[10];
  char processes[10];
  int nProcesses;
  int nCollumns;
  int memsize;
  int index=0;
  char ch;
  bool afterX=false;
  FILE* ptr1=fopen(argv[1],"r");
  FILE* ptr2=fopen(argv[2],"r");

  if (ptr1==NULL){
    printf("%s","file not found \n");
    return 1;
  }
  if (ptr2==NULL){
    printf("%s","file not found \n");
    return 1;
  }
  fscanf(ptr1,"%[^\n]",l1);
  fscanf(ptr2,"%[^\n]",l2);
  if (strcmp(l1,l2)){
      printf("%s","the matrices have different sizes \n");
    return 1;
  }
  rewind(ptr1);
  while(!feof(ptr1)){
      ch= fgetc(ptr1);
      if (ch==10){
          break;
      }
      if (afterX){

        processes[index]=ch;
        index++;
      }
      if (ch==120){
          afterX=true;
      }
      
  }
  index=0;
  rewind(ptr1);
  while(!feof(ptr1)){
    ch= fgetc(ptr1);
    if (ch==120){
        break;
    }
    collmuns[index]=ch;
    index++;
  }
  nCollumns=atoi(collmuns);
  nProcesses=atoi(processes);
  memsize=nCollumns*nProcesses;
  int *partials = mmap(NULL, memsize, PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS, 0, 0);
  if(partials == MAP_FAILED){
    perror("mmap");
    exit(EXIT_FAILURE);
  }
  for(int i = 0; i < nProcesses; i++){
      partials[i] = 0;
  }

  for(i = 0; i < nProcesses; i++) {
    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if(pid == 0) {
        for(int j = 0; j < n; j++){
            if(j % nProcesses == i){
                for(int k = 0; k < n; k++){
                    if(matrix[j][k] > threshold){
                        partials[i]++;
                        exit(EXIT_SUCCESS);
                    }
                }
            }
        }
            
                
                    
                        
                    

  
  return 0;
}