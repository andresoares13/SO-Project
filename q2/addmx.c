#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>

int main(int argc, char* argv[]){
  if (argc<3  || argc>=4 || (argc==3 && (!strstr(argv[1],".txt")||!strstr(argv[2],".txt")))){
    printf("%s","usage: addmx file1 file2\n");
    return 1;
  }
  char l1[10];
  char l2[10];
  char lines[10];
  char processes[10];
  int nProcesses; //equal to the number of columns since every process will work on one
  int nLines;
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
      if (ch==10){ //new line
          break;
      }
      if (afterX){
        processes[index]=ch; //number of columns
        index++;
      }
      if (ch==120){ //x
          afterX=true;
      }
      
  }
  index=0;
  rewind(ptr1);
  while(!feof(ptr1)){
    ch= fgetc(ptr1);
    if (ch==120){ //x
        break;
    }
    lines[index]=ch; //number of lines
    index++;
  }
  nLines=atoi(lines);
  nProcesses=atoi(processes);
  memsize=nLines*nProcesses;
  int *partials = mmap(NULL, memsize, PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS, 0, 0);
  if(partials == MAP_FAILED){
    perror("mmap");
    exit(EXIT_FAILURE);
  }
  for(int i = 0; i < memsize; i++){
      partials[i] = 0;
  }

  rewind(ptr1);
  for(int i = 0; i < nProcesses; i++) {
    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if(pid == 0) {
      
      for (int j=0;j<nLines;j++){
        int x,y;
        int counter = j;
        int emptySkips = i; //if i=0 you dont wanna skip to the 3rd character, if the i=1 you wanna get to the column 3rd character and so on
        bool skipLine=true;
        while(!feof(ptr1)){
          ch= fgetc(ptr1);
          if (ch!=10&&skipLine){ // while characters are different than new line but we want to skip that line, we skip
            continue;
          }
          else{
            if (ch==10){ //when it is the new line, we want to not skip line anymore if we already skipped the ammount of lines we needed to (varies with j)
              if (counter==0){
                skipLine=false;
                counter=j;
                continue;
              }
              counter--;
              continue;
            }
            if (emptySkips==0 && ch!=32){ //varies with i, if we are in the line we want, we need to make sure to acess the right character, basically its the amount of times we skip the empy char separating the columns and it is due to the process number
              x=ch - '0';
              break;
            }
            if (!skipLine && ch==32){ //32=space
              emptySkips--;
            }
          }
        }
        emptySkips=i;
        skipLine=true;
        while(!feof(ptr2)){
          ch= fgetc(ptr2);
          if (ch!=10&&skipLine){
            continue;
          }
          else{
            if (ch==10){
              if (counter==0){
                skipLine=false;
                counter=j;
                continue;
              }
              counter--;
              continue;
            }
            if (emptySkips==0 && ch!=32){
              y=ch - '0';
              break;
            }
            if (!skipLine && ch==32){
              emptySkips--;
            }
          }
        }
        
        rewind(ptr1); //goes back to the beggining of both files before starting next process
        rewind(ptr2);
        partials[(i*nLines)+j]=x+y; // store in the shared space the sum of both characters in the line and column corresponding to the process, process 1 takes care of 4 lines in the 1st column in the example
      }
      exit(EXIT_SUCCESS);
      
    }        
  }

  for(int i = 0; i < nProcesses; i++) { //waits for the processes to end
    if ( waitpid(-1, NULL, 0) < 0) {
      perror("waitpid");
      exit(EXIT_FAILURE);
    }
  }
  
  
  printf("%d%s%d\n",nLines,"x",nProcesses); //prints out the result in the correct format
  for (int j=0;j<nLines;j++){
    for (int i =0; i<nProcesses;i++){
      printf("%d ",partials[j+(nLines*i)]);
    }
    printf("\n");
  }

  fclose(ptr1);
  fclose(ptr2);
     
  return 0;
}