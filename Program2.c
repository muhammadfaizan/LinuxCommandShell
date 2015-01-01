// This program is for making Kernel.. from slide "0.Linux File System"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h> 
#include <malloc.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void copyfile(char source[], char target[])
{
    // character buffer
    char block[1024];
    // file input output reference of fidles from inode
    int in, out;
    int nread;
    in = open(source, O_RDONLY);
    out = open(target, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    while ((nread = read(in, block, sizeof(block))) > 0)
    {
        write(out, block, nread);
    }
}

//ls program to list all directory files 
void ls()
{
    DIR *dp = NULL;
    struct dirent *dptr = NULL;
    unsigned int count = 0;
    struct stat *file_stat;
    // Open the current directory 
    dp = opendir((const char*)getenv("PWD"));

    if (dp == NULL)
    {
        printf("\n ERROR : Could not open the working directory\n");
        
    }
    printf("\n");
    
    for (count = 0; (dptr = readdir(dp)) != NULL; count++)
    {
        if (dptr->d_name[0] != '.')
        {
            
            printf("%s  \n", dptr->d_name); 

        }
    }
    closedir(dp);
    printf("\n %u", count);
}

// remove function
void rm(char filepath[])
{
    short errno = remove(filepath);
    if (errno != 0)
    {
        perror("remove()");
    }
}
// remove directory
int remove_dir(char *path)
{
    return rmdir(path);
}

//for invalid commands
void invalid()
{
    perror("invalid_command");
}

void cd(char dir[])
{
    chdir(dir);
}

void pwd()
{
    char * buffer;
    char * cwd;
    size_t size_of_dir = sizeof(char)* 1024;
    buffer = (char *)malloc(size_of_dir);
    if ((cwd = getcwd(buffer, size_of_dir)) != NULL){
        printf("\n%s\n", cwd);
    }
    else {
        perror("getcwd(): ");
    }
}
void dir_in_depth(char *dir, int depth)
{
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;

    if((dp = opendir(dir)) == NULL) {
        fprintf(stderr,"cannot open directory: %s\n", dir);
        return;
    }
    chdir(dir);
    while((entry = readdir(dp)) != NULL) {
        lstat(entry->d_name,&statbuf);
        if(S_ISDIR(statbuf.st_mode)) {
            /* Found a directory, but ignore . and .. */
            if(strcmp(".",entry->d_name) == 0 || 
                strcmp("..",entry->d_name) == 0)
                continue;
            printf("%*s%s/\n",depth,"",entry->d_name);
            /* Recurse at a new indent level */
            dir_in_depth(entry->d_name,depth+4);
        }
        else printf("%*s%s\n",depth,"",entry->d_name);
    }
    chdir("..");
    closedir(dp);
}

int makedir(char *filename)
{
    return mkdir(filename, 0777);
}

int _rename(char *filename, char *newname)
{
    return rename(filename, newname);
}
void split(char command[300],char param[3][100],char splitter)
{
     int i=0,j=0, index,
        n = strlen(command);
    for (index=0; index< n; index++)
    {
        if ( command[index] == splitter)
        {
            param[i][j] = '\0';
            
            i++;
            j=0;
            continue;
        }
        param[i][j] = command[index];
        j++;
    }
    while ( i <= 3)
    {
        param[i-1][j] = '\0';
        j=0;
        i++;        
    }  
}

int main(int argc, char *argv[])
{
    
    char command[300];
    char param[3][100];
    int err,i=0,j=0, index, n;
    char splitter = ' ';
    do {
    printf("\nFaizan $ ");
    scanf("%[^\n]s",command);
    
    // SPLIT START
    n = strlen(command);
    printf("n= %d\n",n);
    
    for (index=0; index< n; index++)
    {
        
        if ( command[index] == splitter)
        {
            param[i][j] = '\0';
            printf("\nparam[%d] = %s\n",i,param[i]);
            i++;
            j=0;
            continue;
        }
        param[i][j] = command[index];
        j++;
    }
    while ( i <= 3)
    {
        param[i][j] = '\0';
        j=0;
        i++;        
    }  
    //*/
    // SPLIT END
    printf("command = %s\nparam[0] = %s\nparam[1] = %s\nparam[2] = %s\n",command,param[0],param[1],param[2]);
    switch (param[0][0]) {
        case 'l': // case for ls
        {
            // ls case will be handled 
            if (strcmp(param[0], "ls") == 0)
            {
                // ls -d case will be handled here
                if (strlen(param[1]) && strcmp(param[1], "-d") == 0) 
                {
                    char pwd[2] = ".";
                    dir_in_depth(pwd,0);
                }
                else {
                    // ls calling

                    ls();  
                }
            }
            else {
                invalid();
            }
            break;
        }
        case 'e':
        {   
            if (strcmp(param[0],"exit") == 0 )
            {
                exit(1);
            }
            break;
        }
        case 'm': // case for mkdir, mv
        {
            if ( strcmp(param[0],"mkdir") == 0)
            {
                if (strlen(param[1]) != 0)
                {
                    err = makedir(param[1]);
                    if (err!=0) {
                        perror("makedir");
                    }
                } 
                else {
                    perror("mkdir: too few arguments");
                }
            }
            else if ( strcmp(param[0],"mv") == 0 )
            {
                if((strlen(param[1]) != 0) && (strlen(param[2]) != 0))
                {
                    copyfile(param[1],param[2]);
                    rm(param[1]);
                } 
            }
            break;
        }
        case 'p': // case for pwd
        {
            if (strcmp(param[0], "pwd")==0)
            {
                pwd();
            }
            else {
                invalid();
                
            }
            break;
        }            
        
        case 'c': // case for cd, cp, chmod, chown, chgrp
        {
            if (strcmp(param[0],"cp") ==0)
            {
                copyfile(param[1],param[2]);
            }
            else  if (strcmp(param[0],"cd") == 0) 
            {
                cd(param[1]);
                pwd();                
            }
            printf("\nWorking on cd, cp, chmod, chown, chgrp");
            break;
        }
        case 'r': // case for rmdir, rm, rename
        {
            if (strcmp(param[0], "rm")==0)
            {
                rm(param[1]);
            }
            else if (strcmp(param[0], "rmdir") ==  0)    
            {
                if (strlen(param[1]) != 0)
                {
                    err = remove_dir(param[1]);
                    if (err!=0) {
                        perror("rmdir");
                    }
                } 
                else {
                    perror("rmdir: too few arguments");
                }
            }
            else if (strcmp(param[0], "rename") ==  0)    
            {
                
               if (strlen(param[1]) != 0 && strlen(param[2]) != 0)
               {
                    err = _rename(param[1],param[2]);
                    if (err!=0) {
                        perror("rename");
                    }
                } 
                else {
                    perror("rename: too few arguments");
                }
            }
            else {
                invalid(); 
            }
            break;
        }
        case 'u': // case for umask
        {
            break;
        }
    }
    gets(command);
    } while(1);
    //system("./kernel1.out");
    return 1;
}


