/* 
 * File:   main2.c
 * Author: RinesThaix
 *
 * Created on 21 February, 13:04
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ENDC '\0'

char* filename;
FILE* file;
int lastId = 0;

void error(char* msg) {
    printf("[Error] %s\n", msg);
    fflush(stdout);
}

void debug(int id) {
    printf("[Debug] #%d\n", id);
    fflush(stdout);
}

typedef struct {
    int id;
    char *name, *phone, *true;
} human;

typedef struct {
    int size;
    human* ppl;
} all;

int checkForNull(const char* value) {
    if(equals(value, "")) {
        error("That string could not be empty!");
        return 0;
    }
    return 1;
}

char* getTruePhone(const char* phone) {
    int size = 0;
    while(phone[size++] != ENDC);
    char* true = malloc(size * sizeof(char));
    int i, j = 0;
    for (i = 0; i < size; ++i)
        true[i] = 0;
    for (i = 0; i < size; ++i)
        if (phone[i] >= '0' && phone[i] <= '9')
            true[j++] = phone[i];
    return true;
}

int equals(const char* a, const char* b) {
    return !strcmp(a, b);
}

int contains(const char* a, const char* b) {
    int sizea = 0, sizeb = 0;
    while(a[sizea++] != ENDC);
    while(b[sizeb++] != ENDC);
    char an[sizea], bn[sizeb];
    int i;
    for (i = 0; i < sizea; ++i)
        an[i] = a[i] >= 'A' && a[i] <= 'Z' ? 'a' + a[i] - 'A' : a[i];
    for (i = 0; i < sizeb; ++i)
        bn[i] = b[i] >= 'A' && b[i] <= 'Z' ? 'a' + b[i] - 'A' : b[i];
    return strstr(an, bn) != NULL;
}

char* read0(FILE* is) {
    getc(is);
    char* w = malloc(0), c = fgetc(is);
    int i = 0, j = 0, block = 1 << 12;
    while(c != EOF && c != ' ' && c != '\n') {
        if(i % block == 0)
            w = realloc(w, ++j * block * sizeof(char));
        w[i++] = c;
        c = fgetc(is);
    }
    w[i] = ENDC;
    ungetc(' ', is);
    return w;
}

char* readF() {
    return read0(file);
}

char* readC() {
    return read0(stdin);
}

all set;

void rewrite() {
    fclose(file);
    file = fopen(filename, "w+");
    int i;
    for(i = 0; i < set.size; ++i) {
        human current = set.ppl[i];
        fprintf(file, i == 0 ? "%d %s %s" : "\n%d %s %s", current.id, current.name, current.phone);
    }
    fclose(file);
    file = fopen(filename, "a");
}

void add(int id, char* name, char* phone, int write) {
    int i = set.size++;
    if(id == -1)
        id = lastId;
    set.ppl = realloc(set.ppl, set.size * sizeof(human));
    set.ppl[i].id = id;
    set.ppl[i].name = name;
    set.ppl[i].phone = phone;
    set.ppl[i].true = getTruePhone(phone);
    if(write) {
        fprintf(file, "%d %s %s\n", id, name, phone);
        fflush(file);
    }
    if(lastId <= id)
        lastId = id + 1;
}

int getTrueIndex(int id) {
    int i;
    for(i = 0; i < set.size; ++i)
        if(set.ppl[i].id == id)
            return i;
    return -1;
}

void delete(int id) {
    id = getTrueIndex(id);
    if(id == -1) {
        error("There's no record with given id!");
        return;
    }
    for(int i = id + 1; i < set.size; ++i)
        set.ppl[i - 1] = set.ppl[i];
    set.ppl = realloc(set.ppl, --set.size * sizeof(human));
    rewrite();
}

void change(int id, int name, char* value) {
    id = getTrueIndex(id);
    if(id == -1) {
        error("There's no record with given id!");
        return;
    }
    human current = set.ppl[id];
    if(name)
        strcpy(current.name, value);
    else {
        strcpy(current.phone, value);
        char* true = getTruePhone(value);
        strcpy(current.true, true);
        free(true);
    }
    rewrite();
}

void find(char* value) {
    int i, anyone = 0;
    for(i = 0; i < set.size; ++i) {
        human current = set.ppl[i];
        if(contains(current.name, value)) {
            anyone = 1;
            printf("%d %s %s\n", current.id, current.name, current.phone);
            continue;
        }
        char* true = getTruePhone(value);
        if(equals(current.true, true)) {
            anyone = 1;
            printf("%d %s %s\n", current.id, current.name, current.phone);
        }
        free(true);
    }
    if(!anyone)
        error("No one could be found!");
    else
        fflush(stdout);
}

void read() {
    int id;
    while((fscanf(file, "%d", &id)) == 1)
        add(id, readF(), readF(), 0);
}

int main(int argc, char** argv) {
    filename = argv[1];
    file = fopen(filename, "a+");
    rewind(file);
    read();

    char* cmd = malloc(sizeof(char) << 5);
    while(1) {
        scanf("%s", cmd);
        if(equals(cmd, "create")) {
            char* name = readC(), *phone = readC();
            if(checkForNull(name) && checkForNull(phone))
                add(-1, name, phone, 1);
        }else if(equals(cmd, "delete")) {
            int id;
            scanf("%d", &id);
            delete(id);
        }else if(equals(cmd, "change")) {
            int id;
            scanf("%d", &id);
            char* option = readC(), *value = readC();
            if(!checkForNull(value))
                continue;
            if(equals(option, "name"))
                change(id, 1, value);
            else if(equals(option, "number"))
                change(id, 0, value);
            else
                error("Unknown subcommand for 'change'!");
            free(option);
            free(value);
        }else if(equals(cmd, "find")) {
            char* value = readC();
            find(value);
            free(value);
        }else if(equals(cmd, "exit")) {
            break;
        }else {
            error("Unknown command!");
        }
    }
    
    free(cmd);
    for(int i = 0; i < set.size; ++i) {
        human current = set.ppl[i];
        free(current.name);
        free(current.phone);
        free(current.true);
    }
    free(set.ppl);
    fclose(file);
    return (EXIT_SUCCESS);
}