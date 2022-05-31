#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

int main(int argc, char* argv[]){
  if (argc<2  || argc>3 || (argc==3 && strcmp(argv[1],"-l")!=0) || (argc==2 && !strstr(argv[1],".txt"))){
    printf("%s","usage: phrases [-l] file \n");
    return 1;
  }
  FILE* ptr;
  bool flag=false;
  if (strcmp(argv[1],"-l")==0){
    flag=true;
    ptr = fopen(argv[2], "r");
  }
  else{
    ptr = fopen(argv[1], "r");
  }
  if (ptr==NULL){
    printf("%s","file not found \n");
    return 1;
  }
  int index=0;
  int count=0;
  char ch;
  bool point=false;
  while(!feof(ptr)){
    if (index==0){ //start
      count++; //1
      if (flag){
        printf("[%d] ",count);
      }
    }

    index++;
    ch = fgetc(ptr);
    if (ch!=EOF && point && ch!=32 && ch!=10){ // if the previous character was a point this prevents the scenarios where there is a newline after the point with no sentece and where the last sentence ends with a point
      printf("\n[%d] ",count);
      point=false;
    }
    if (flag && ch!=EOF && ch!=10 && !point){ //prevents the printing of newlines
      printf("%c",ch); 
    }

    if (ch==46|| ch==63 || ch==33){  // . or ! or ?
      count++; 
      if (flag){
        point=true;
      }
    }
    if (ch==EOF && flag){ //end of file
      printf("\n");
    }
  }
  fclose(ptr);
  if (!flag){
    printf("%d",count);
    printf("\n");
  }
  
  return 0;
}