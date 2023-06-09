/* nftw_dir_tree.c
   Demonstrate the use of nftw(3). Walk though the directory tree specified
   on the command line (or the current working directory if no directory
   is specified on the command line), displaying an indented hierarchy
   of files in the tree. For each file, display:
      * a letter indicating the file type (using the same letters as "ls -l")
      * a string indicating the file type, as supplied by nftw()
      * the file's i-node number.
    
    https://github.com/kddnewton/tree/blob/main/tree.c#L17 
*/
#define _XOPEN_SOURCE 600 /* Get nftw() */
#include <ftw.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>

static int file = 0;
static int dir = 0;
static int levelFileCount[10];
static int currFileInLevel[10];

static int count_files(const char *path, const struct stat *statbuf, int typeflag, struct FTW *ftwbuf) {
    // if (typeflag == FTW_F) {
        // printf("Level %d: %s\n", ftwbuf->level, path);
    //     return 0;
    // }
    levelFileCount[ftwbuf->level] += 1;
    return 0;
}

static int process_file(const char *path, const struct stat *statbuf, int typeflag, struct FTW *ftwbuf) {
    if (typeflag == FTW_F) {
        printf("Last file in directory '%d': %s\n", ftwbuf->base, path);
    }
    return 0;
}

static int dirTree(const char *pathname, const struct stat *sbuf, int type, struct FTW *ftwb)
{
     if (type == FTW_NS) {
        printf("?");
    } else {
        char* token = strtok(pathname, "/");
        token = strtok(NULL, "/");
 
        while (token != NULL) {
            if(token[0] == '.'){
                return 0; 
            }
            token = strtok(NULL, "/");
        }
        if( ftwb->level ==0){
            printf(".\n");
        }
        else{
            for(int i=0; i< ftwb->level-1; i++){
                printf("│  ");
                currFileInLevel[ftwb->level]++;
            
            }
            if(currFileInLevel[ftwb->level] == levelFileCount[ftwb->level]-1){
                printf("└──");
            }
            else{
                printf("├──");
                currFileInLevel[ftwb->level]++;
            }
            

            switch (sbuf->st_mode & S_IFMT) {
                case S_IFREG:  printf("[-"); break;
                case S_IFDIR:  printf("[d"); break;
                case S_IFCHR:  printf("[c"); break;
                case S_IFBLK:  printf("[b"); break;
                case S_IFLNK:  printf("[l"); break;
                case S_IFIFO:  printf("[p"); break;
                case S_IFSOCK: printf("[s"); break;
                default:       printf("[?????????? "); break;
            }

            printf((sbuf->st_mode & S_IRUSR) ? "r" : "-");
            printf((sbuf->st_mode & S_IWUSR) ? "w" : "-");
            printf((sbuf->st_mode & S_IXUSR) ? "x" : "-");
            printf((sbuf->st_mode & S_IRGRP) ? "r" : "-");
            printf((sbuf->st_mode & S_IWGRP) ? "w" : "-");
            printf((sbuf->st_mode & S_IXGRP) ? "x" : "-");
            printf((sbuf->st_mode & S_IROTH) ? "r" : "-");
            printf((sbuf->st_mode & S_IWOTH) ? "w" : "-");
            printf((sbuf->st_mode & S_IXOTH) ? "x" : "-");

            struct passwd *user = getpwuid(sbuf->st_uid);
            struct group *group = getgrgid(sbuf->st_gid);

            printf(" %s ", user ? user->pw_name : "");
            printf("%s ", group ? group->gr_name : "");
            printf(" %8ld]", (long) sbuf->st_size);
            printf(" %s\n", &pathname[ftwb->base]);

            if ((S_IFMT & sbuf->st_mode) == S_IFDIR){
                dir++;
            }
            else{
                file++;
            }

 
        }

    }
  
    return 0;
}

int main(int argc, char *argv[])
{
    int flags = FTW_PHYS;  /* Perform a physical walk instead of following symbolic links */
    memset(currFileInLevel, 0, 10);

    if (argc != 2) {
        fprintf(stderr, "Usage: %s directory-path\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int result = nftw(argv[1], count_files, 10, flags);

    int results = nftw(argv[1], process_file, 10, flags);

    if (nftw(argv[1], dirTree, 10, flags) == -1) {
        perror("nftw");
        exit(EXIT_FAILURE);
    }
    printf("\n");
    printf("%d directories", dir);
    printf(", %d files\n", file);


    exit(EXIT_SUCCESS);
}
