#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>
#include <signal.h>
#include <vector>
#include <thread>
#include <chrono>
using namespace std;
std::vector<pid_t> kids;
int rgen(int argc, char **argv)
{
    int val = execv("./rgen", argv);
    if (val == -1)
    {
        std::cerr << "Error: from rgen" << '\n';
        exit(1);
    }
    return 0;
}
int a1(void)
{
    char *new_argv[4];
    new_argv[0] = "/usr/bin/python3";
    new_argv[1] = "-u";
    new_argv[2] = "ece650-a1.py";
    new_argv[3] = 0;

    int value = execv(new_argv[0], new_argv);

    if (value == -1)
    {
        std::cerr << "Error: from a1" << '\n';
        exit(1);
    }
    return 0;
}

void handleSigterm(int signum)
{
    for (pid_t k : kids)
    {
        kill(k, SIGTERM);
    }
    exit(1);
}
int a2(void)
{
    int value2 = 0;

    char *arg_list[] = {nullptr};

    value2 = execv("./ece650-a2", arg_list);

    if (value2 == -1)
    {
        std::cerr << "Error: from a2" << '\n';
        exit(1);
    }
    return 0;
}

int main(int argc, char **argv)
{
    signal(SIGTERM, handleSigterm);
    int RgentoA1[2];
    pipe(RgentoA1);
    int A1toA2[2];
    pipe(A1toA2);

    int status;
    string input;

    pid_t child_pid;
    child_pid = fork();

    if (child_pid < 0)
    {
        std::cout << "Error: unable to fork" << '\n';
        return 1;
    }

    /* First Child */
    else if (child_pid == 0)
    {
        /*write into the pipe RgentoA1*/
        dup2(RgentoA1[1], 1);
        close(RgentoA1[0]);
        close(RgentoA1[1]);
        /* return to rgen function */
        return rgen(argc, argv);
    }
    // else if (child_pid > 0)
    // { // In the parent process
    //     char buffer[2560000];
    //     close(RgentoA1[1]); // Close the write end of the pipe in the parent

    //     // Continue reading until there is nothing left to read
    //     while (true)
    //     {
    //         ssize_t bytesRead = read(RgentoA1[0], buffer, sizeof(buffer) - 1);

    //         if (bytesRead <= 0)
    //         {
    //             break; // Exit the loop if no more data is available or on error
    //         }

    //         // Null-terminate the buffer to ensure it’s a valid string
    //         buffer[bytesRead] = '\0'; // Use bytesRead to null-terminate

    //         // Print the output
    //         cout << buffer;
    //     }

    //     close(RgentoA1[0]); // Close the read end of the pipe when done
    // }
    kids.push_back(child_pid);
    /* Forking inorder to Execute A1 */
    child_pid = fork();
    if (child_pid < 0)
    {
        std::cout << "Error: unable to fork" << '\n';
        return 1;
    }

    /* Second Child */
    if (child_pid == 0)
    {
        /* redirect stdin to the pipe */
        dup2(RgentoA1[0], 0);
        dup2(A1toA2[1], 1);
        close(RgentoA1[0]);
        close(RgentoA1[1]);
        close(A1toA2[0]);
        close(A1toA2[1]);

        /* redirect stdout to the pipe */
        // dup2(A1toA2[1], 1);
        // close(A1toA2[0]);
        // close(A1toA2[1]);

        return a1();
    }

    // else
    // {                       // Parent process
    //     close(RgentoA1[0]); // Close read end of cppTopython
    //     close(A1toA2[1]);   // Close write end of pythonTocpp

    //     // Optionally, you can send input from the parent to the child process.
    //     // For example, you can read input from a file or user input at runtime.
    //     // In this case, you don't need to send any hardcoded data.
    //     // However, if your child process doesn't need input, you can skip this part.

    //     // Instead, we will read the output of the child process
    //     char buffer[256000];
    //     while (true)
    //     {
    //         ssize_t n = read(A1toA2[0], buffer, sizeof(buffer) - 1);
    //         if (n > 0)
    //         {
    //             buffer[n] = '\0';                                 // Null-terminate the string
    //             std::cout << "Python process output: " << buffer; // Print output from Python
    //         }
    //         else if (n == 0)
    //         {
    //             break; // End of output
    //         }
    //         else
    //         {
    //             perror("read failed");
    //             break;
    //         }
    //     }

    //     close(A1toA2[0]); // Close the read end of the pipe when done
    //                       // wait(nullptr); // Wait for the child process to finish
    // }
    kids.push_back(child_pid);
    /* Fork to launch A2 */
    child_pid = fork();
    if (child_pid < 0)
    {
        std::cout << "Error: unable to fork" << '\n';
        return 1;
    }

    /* Third Child */
    if (child_pid == 0)
    {
        /* redirect stdin to the pipe */
        dup2(A1toA2[0], 0);
        close(A1toA2[0]);
        close(A1toA2[1]);

        return a2();
    }

    /* redirect stdout to the pipe */
    dup2(A1toA2[1], 1);

    while (!std::cin.eof())
    {
        // read a line of input until EOL and store in a string
        string line;
        std::getline(std::cin, line);
        std::cout << line << std::endl;
    }
    close(A1toA2[0]);
    close(A1toA2[1]);

    kids.push_back(child_pid);
    for (pid_t k : kids)
    {
        kill(k, SIGTERM);
        waitpid(k, &status, 0);
    }

    return 0;
}
