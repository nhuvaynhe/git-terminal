#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    char buffer[128];
    FILE* pPipe;

    if ((pPipe = _popen("dir *.c /on /p", "rt")) == NULL)
    {
        exit(1);
    }

    /* Read pipe until end of file, or an error occurs. */

    while (fgets(buffer, 128, pPipe))
    {
        puts(buffer);
    }

    int endOfFileVal = feof(pPipe);
    int closeReturnVal = _pclose(pPipe);

    if (endOfFileVal)
    {
        printf("\nProcess returned %d\n", closeReturnVal);
    }
    else
    {
        printf("Error: Failed to read the pipe to the end.\n");
    }
}