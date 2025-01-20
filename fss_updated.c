#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct File {
    char name[50];
    struct File *next;
} File;

typedef struct Directory {
    char name[50];
    struct Directory *subdirs;
    File *files;
    struct Directory *next;
} Directory;

Directory *root = NULL;  // Root directory
Directory *current_dir = NULL;  // Current working directory

void print_prompt() {
    printf("> ");
}

Directory *find_directory(const char *dir_name) {
    Directory *temp = current_dir->subdirs;
    while (temp != NULL) {
        if (strcmp(temp->name, dir_name) == 0) {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

File *find_file(const char *file_name) {
    File *temp = current_dir->files;
    while (temp != NULL) {
        if (strcmp(temp->name, file_name) == 0) {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

void create_directory(char *dir_name) {
    if (find_directory(dir_name)) {
        printf("Directory '%s' already exists.\n", dir_name);
        return;
    }

    Directory *new_dir = (Directory *)malloc(sizeof(Directory));
    strcpy(new_dir->name, dir_name);
    new_dir->subdirs = NULL;
    new_dir->files = NULL;
    new_dir->next = current_dir->subdirs;
    current_dir->subdirs = new_dir;

    printf("Directory '%s' created.\n", dir_name);
}

void create_file(char *file_name) {
    if (find_file(file_name)) {
        printf("File '%s' already exists.\n", file_name);
        return;
    }

    File *new_file = (File *)malloc(sizeof(File));
    strcpy(new_file->name, file_name);
    new_file->next = current_dir->files;
    current_dir->files = new_file;

    printf("File '%s' created.\n", file_name);
}

void remove_file(char *file_name) {
    File *temp = current_dir->files;
    File *prev = NULL;

    while (temp != NULL) {
        if (strcmp(temp->name, file_name) == 0) {
            if (prev == NULL) {
                current_dir->files = temp->next;
            } else {
                prev->next = temp->next;
            }
            free(temp);
            printf("File '%s' removed.\n", file_name);
            return;
        }
        prev = temp;
        temp = temp->next;
    }
    printf("File '%s' not found.\n", file_name);
}

void list_files() {
    File *temp = current_dir->files;
    if (!temp) {
        printf("No files found.\n");
        return;
    }
    while (temp != NULL) {
        printf("File: %s\n", temp->name);
        temp = temp->next;
    }
}

void list_directories() {
    Directory *temp = current_dir->subdirs;
    if (!temp) {
        printf("No directories found.\n");
        return;
    }
    while (temp != NULL) {
        printf("Directory: %s\n", temp->name);
        temp = temp->next;
    }
}

void change_directory(char *dir_name) {
    if (strcmp(dir_name, "..") == 0) {
        if (current_dir != root) {
            current_dir = current_dir->next;
            printf("Moved to parent directory.\n");
        } else {
            printf("Already at root directory.\n");
        }
    } else {
        Directory *dir = find_directory(dir_name);
        if (dir) {
            current_dir = dir;
            printf("Moved to directory '%s'.\n", dir_name);
        } else {
            printf("Directory '%s' not found.\n", dir_name);
        }
    }
}

void exit_simulator() {
    printf("Exiting simulator...\n");
    exit(0);
}

// New features:

// Rename a file or directory
void rename_item(char *old_name, char *new_name) {
    File *file = find_file(old_name);
    if (file) {
        strcpy(file->name, new_name);
        printf("File renamed to '%s'.\n", new_name);
        return;
    }

    Directory *dir = find_directory(old_name);
    if (dir) {
        strcpy(dir->name, new_name);
        printf("Directory renamed to '%s'.\n", new_name);
        return;
    }

    printf("Item '%s' not found.\n", old_name);
}

// Copy a file to another directory (simulated by creating a new file with the same name in the target directory)
void copy_file(char *src_name, char *dest_name) {
    File *src_file = find_file(src_name);
    if (!src_file) {
        printf("File '%s' not found.\n", src_name);
        return;
    }

    if (find_file(dest_name)) {
        printf("File '%s' already exists in the destination directory.\n", dest_name);
        return;
    }

    // Create a new file in the current directory
    create_file(dest_name);
    printf("File '%s' copied to '%s'.\n", src_name, dest_name);
}

// Move a file to another directory (simulated by removing the file and recreating it in the destination)
void move_file(char *src_name, char *dest_name) {
    File *src_file = find_file(src_name);
    if (!src_file) {
        printf("File '%s' not found.\n", src_name);
        return;
    }

    // Remove the source file
    remove_file(src_name);

    // Create the file in the destination directory
    create_file(dest_name);
    printf("File '%s' moved to '%s'.\n", src_name, dest_name);
}

void execute_command(char *command) {
    char *token = strtok(command, " ");
    if (token == NULL) return;

    if (strcmp(token, "mkdir") == 0) {
        token = strtok(NULL, " ");
        if (token) create_directory(token);
        else printf("Directory name missing.\n");
    } else if (strcmp(token, "touch") == 0) {
        token = strtok(NULL, " ");
        if (token) create_file(token);
        else printf("File name missing.\n");
    } else if (strcmp(token, "rm") == 0) {
        token = strtok(NULL, " ");
        if (token) remove_file(token);
        else printf("File name missing.\n");
    } else if (strcmp(token, "ls") == 0) {
        printf("Listing directories:\n");
        list_directories();
        printf("Listing files:\n");
        list_files();
    } else if (strcmp(token, "cd") == 0) {
        token = strtok(NULL, " ");
        if (token) change_directory(token);
        else printf("Directory name missing.\n");
    } else if (strcmp(token, "rename") == 0) {
        char *old_name = strtok(NULL, " ");
        char *new_name = strtok(NULL, " ");
        if (old_name && new_name) rename_item(old_name, new_name);
        else printf("Old and new names missing.\n");
    } else if (strcmp(token, "cp") == 0) {
        char *src_name = strtok(NULL, " ");
        char *dest_name = strtok(NULL, " ");
        if (src_name && dest_name) copy_file(src_name, dest_name);
        else printf("Source and destination names missing.\n");
    } else if (strcmp(token, "mv") == 0) {
        char *src_name = strtok(NULL, " ");
        char *dest_name = strtok(NULL, " ");
        if (src_name && dest_name) move_file(src_name, dest_name);
        else printf("Source and destination names missing.\n");
    } else if (strcmp(token, "exit") == 0) {
        exit_simulator();
    } else {
        printf("Unknown command: %s\n", token);
    }
}

void initialize_root() {
    root = (Directory *)malloc(sizeof(Directory));
    strcpy(root->name, "/");
    root->subdirs = NULL;
    root->files = NULL;
    root->next = NULL;
    current_dir = root;
}

int main() {
    char command[100];

    initialize_root();

    printf("Welcome to File System Simulator!\n");

    while (1) {
        print_prompt();
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0;  // Remove newline character
        execute_command(command);
    }

    return 0;
}

