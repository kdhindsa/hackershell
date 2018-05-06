/*
 * Thanks to Stephen Brennan's tutorial on writing a shell in c:
 * https://brennan.io/2015/01/16/write-a-shell-in-c/
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int hs_help(char**);
int hs_exit(char**);
int hs_execute(char**);
int hs_launch(char**);
char *read_line(void);
char **split_line(char*);
int num_builtins(void);

char *builtin_cmds[] = {
    "help",
    "exit"
};

int (*builtin_func[]) (char **) = {
    &hs_help,
    &hs_exit
};

int main(int argc, char** argv) {
    char *line;
    char **args;
    int status;

    do {
        printf("HS> ");
        line = read_line();
        args = split_line(line);
        status = hs_execute(args);

        free(line);
        free(args);
    } while(status);
    
    return EXIT_SUCCESS;
}

int hs_execute(char **args) {
    int i;

    if (args[0] == NULL) {
        return 1;
    }

    // Check if command matches one of the builtin commands..
    for (i = 0; i < num_builtins(); i++) {
        if (strcmp(args[0], builtin_cmds[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }
    
    // return hs_launch(args);
    printf("Command '%s' not supported.\n", args[0]);
    
    return hs_help(args);
}


# define RL_BUFSIZE 1024

/*
 *  Read line from stdin
 */
char *read_line(void) {
    int bufsize = RL_BUFSIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char) * bufsize);
    int c;

    if (!buffer) {
        fprintf(stderr, "hsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while(1) {
        // read a character
        c = getchar();

        if (c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = c;
        }

        position++;

        if (position >= bufsize) {
            bufsize += RL_BUFSIZE;
            buffer = realloc(buffer, bufsize);
            if (!buffer) {
                fprintf(stderr, "hsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }

}

#define TOK_BUFSIZE 64
#define TOK_DELIM " \t\r\n\a"

char** split_line(char *line) {
    int bufsize = TOK_BUFSIZE;
    int position = 0;
    char **tokens = malloc(sizeof(char*) * bufsize);
    char *token;
    
    if (!tokens) {
        fprintf(stderr, "hsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, TOK_DELIM);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= bufsize) {
            bufsize += TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                fprintf(stderr, "hsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, TOK_DELIM);
    }

    tokens[position] = NULL;
    return tokens;
}


int num_builtins(void) {
    return sizeof(builtin_cmds) / sizeof(char *);
}

int hs_exit(char **args) {
    return 0;
}

int hs_help(char **args) {
    int i;
    printf("~~~Hacker Shell~~~The following commands are available:\n");
    for (i = 0; i < num_builtins(); i++) {
        printf(" %s\n", builtin_cmds[i]);
    }
    return 1;
}

int hs_launch(char **args) {
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            perror("hsh");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("hsh");
    } else {
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}
