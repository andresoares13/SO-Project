#include <sys/wait.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#define READ_END 0
#define WRITE_END 1
#define LINESIZE 10000


void str_replace(char *target, const char *needle, const char *replacement)
{
    char buffer[10000] = { 0 };
    char *insert_point = &buffer[0];
    const char *tmp = target;
    size_t needle_len = strlen(needle);
    size_t repl_len = strlen(replacement);

    while (1) {
        const char *p = strstr(tmp, needle);

        // walked past last occurrence of needle; copy remaining part
        if (p == NULL) {
            strcpy(insert_point, tmp);
            break;
        }

        // copy part before needle
        memcpy(insert_point, tmp, p - tmp);
        insert_point += p - tmp;

        // copy replacement string
        memcpy(insert_point, replacement, repl_len);
        insert_point += repl_len;

        // adjust pointers, move on
        tmp = p + needle_len;
    }

    // write altered string back to target
    strcpy(target, buffer);
}


int main(int argc, char* argv[]) {

    int nbytes, fd[2],fd2[2];
    pid_t pid;

    char ch;

    char line[LINESIZE];

    int counter=0;
    while(read(STDIN_FILENO, &ch, 1) > 0){
       line[counter]=ch;
       counter++;
    }
        

    if (pipe(fd) < 0) {
        perror("pipe error");
        exit(EXIT_FAILURE); 
    }
    if (pipe(fd2) < 0) {
        perror("pipe error");
        exit(EXIT_FAILURE); 
    }
    if ((pid = fork()) < 0) {
        perror("fork error");
        exit(EXIT_FAILURE);
    }
    else if (pid > 0) {
        /* parent */
        close(fd[READ_END]);


        //writes to child
        
        
        if ((nbytes = write(fd[WRITE_END], line, strlen(line))) < 0) { //sends it to the child
            fprintf(stderr, "Unable to write to pipe: %s\n", strerror(errno));
        }
        close(fd[WRITE_END]);


        /* wait for child and exit */
        if ( waitpid(pid, NULL, 0) < 0) {
            fprintf(stderr, "Cannot wait for child: %s\n", strerror(errno));
        }
        
        // parent process writes what he got from child
        close(fd2[WRITE_END]);
        if ((nbytes = read(fd2[READ_END], line, LINESIZE)) < 0 ) {
            fprintf(stderr, "Unable to read from pipe: %s\n", strerror(errno));
        }
        close(fd2[READ_END]);
        /* write message from child */
        write(STDOUT_FILENO, line, nbytes);
        exit(EXIT_SUCCESS);
    }
    else {
        /* child */
        close(fd[WRITE_END]);
        //reads message from parent
        if ((nbytes = read(fd[READ_END], line, LINESIZE)) < 0 ) {
            fprintf(stderr, "Unable to read from pipe: %s\n", strerror(errno));
        }
        close(fd[READ_END]);
        FILE *ptr2 = fopen("cypher.txt","r");
        int CypherCount=0;
        while (!feof(ptr2)){
            if (fgetc(ptr2)==10){
                CypherCount ++;
            }
        }
        rewind(ptr2);
        char w1[256];
        char w2[256];
        bool second = false;
        for (int i=0; i<CypherCount;i++){ //for every cypher word, we store the word and its replacement and then swap it in the string line containing the whole text
            int index=0;
            while(!feof(ptr2)){
                ch=fgetc(ptr2);
                if (ch==32){
                    second = true;
                    index=0;
                }
                else{
                    if (ch==10){
                        break;
                    }
                    else if (!second){
                        w1[index]=ch;
                        index++;
                    }
                    else{
                        w2[index]=ch;
                        index++;
                    }
                }
                
            }
            second=false;
            if (strstr(line,w1)){
                str_replace(line,w1,w2); //function that replaces every instance of the w1 with w2 on line
            }
            else if (strstr(line,w2)){
                str_replace(line,w2,w1);
            }

            memset(w1, 0, sizeof(w1)); //reset both strings containing the words
            memset(w2, 0, sizeof(w2));

            
        }
        fclose(ptr2);
        close(fd[WRITE_END]);

        //writes to parent, by this point the changes have been made already
        close(fd2[READ_END]);
        if ((nbytes = write(fd2[WRITE_END], line, strlen(line))) < 0) {
            fprintf(stderr, "Unable to write to pipe: %s\n", strerror(errno));
        }
        close(fd2[WRITE_END]);

        
        /* exit gracefully */
        exit(EXIT_SUCCESS);
    }
}


