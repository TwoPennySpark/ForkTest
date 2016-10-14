#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

#define BUFFER_SIZE 64

int main(void)
{
    pid_t Child_pid;
    int fd1, fd2;
    ssize_t buffer_read = 0, buffer_written = 0;
    char buffer[BUFFER_SIZE];
    char child_message[] = "Child process was here\n";
    char parent_message[] = "Parent process was here too\n";

    printf("-------------------------------------------------------------------------------\n");

    //Creating a new process
    Child_pid = fork();
    if (Child_pid < 0)
    {
        printf("[-]ERROR: Cannot create process\n");
        exit(-1);
    }

    if (Child_pid == 0)
    {   //CHILD
        printf("I am a child process my pid is %d\nMy parent pid is %d\n\n", Child_pid, getppid());
        fd1 = open("forkFile1", O_WRONLY | O_CREAT | O_TRUNC, 0744);

        if (fd1 < 0)
        {
            printf("[-]ERROR: Cannot open file\n");
            exit(-1);
        }

        //Adding child message in the 1st file
        write(fd1, child_message, strlen(child_message));

        close(fd1);
        exit(0);
    }
    else
    {   //PARENT
        printf("\nI am a parent process my pid is %d\nI have a child which pid is %d\n\n", getpid(), Child_pid);
        wait(NULL);

        fd1 = open("forkFile1", O_RDONLY);
        fd2 = open("forkFile2", O_RDWR | O_CREAT | O_TRUNC, 0744);

        //Copy file1 into file2
        while ((buffer_read = read(fd1, buffer, BUFFER_SIZE))>0)
        {
            buffer_written = write (fd2, buffer, buffer_read);

            if (buffer_written != buffer_read)
            {
                printf("\n[-]ERROR: Cannot write1\n");
                exit (-1);
            }
        }
        //Adding parent message in the 2nd file
        write(fd2, parent_message, strlen(parent_message));

        //Sets the current position to the beginning of the file
        lseek(fd2, 0, SEEK_SET);

        //Dislplays the content of the 2nd file
        while ((buffer_read = read(fd2, buffer, BUFFER_SIZE))>0)
        {
            buffer_written = write (1, buffer, buffer_read);

            if (buffer_written != buffer_read)
            {
                printf("\n[-]ERROR: Cannot write2\n");
                exit (-1);
            }
        }
        close(fd1);
        close(fd2);
    }
    execl("/bin/echo","echo","[+]Both processes were completed successfully", NULL);
    return 0;
}
