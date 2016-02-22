/* 
 * File:   main2.c
 * Author: RinesThaix
 *
 * Created on 21 February, 13:04
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define M 1000
#define M2 1000000

char filename[M];
FILE* file;

void error(char* msg) {
    printf("Error occured: %s\n", msg);
}

void debug(int id) {
    printf("Debug: %d\n", id);
}

void arc(void* dest, const void* src) {
    memcpy(dest, src, M * sizeof(char));
}

void arcPhone(char* dest, char* src) {
    memcpy(dest, src, M * sizeof(char));
    free(src);
}

int equals(const char* a, const char* b) {
    return !strcmp(a, b);
}

int contains(const char* a, const char* b) {
    char an[M], bn[M];
    int i;
    for(i = 0; i < M; ++i)
        an[i] = a[i] >= 'A' && a[i] <= 'Z' ? 'a' + a[i] - 'A' : a[i];
    for(i = 0; i < M; ++i)
        bn[i] = b[i] >= 'A' && b[i] <= 'Z' ? 'a' + b[i] - 'A' : b[i];
    return strstr(an, bn) != NULL;
}

char* getTruePhone(const char* phone) {
    char* true = malloc(M * sizeof(char));
    int i, j = 0;
    for(i = 0; i < M; ++i)
        true[i] = 0;
    for(i = 0; i < M; ++i) {
        if(phone[i] >= '0' && phone[i] <= '9')
            true[j++] = phone[i];
    }
    return true;
}

typedef struct {
    int id;
    char name[M];
    char phone[M];
    char truePhone[M];
} human;

human* all[M2];

void rewrite() {
    fclose(file);
    file = fopen(filename, "w+");
    int i;
    for(i = 0; i < M2; ++i) {
        if(all[i] != NULL) {
            human* h = all[i];
            fprintf(file, i == 0 ? "%d %s %s" : "\n%d %s %s", h->id, h->name, h->phone);
        }else
            break;
    }
    fclose(file);
    file = fopen(filename, "a");
}

void printAll() {
    int i;
    for(i = 0; i < M2; ++i)
        if(all[i] != NULL) {
            human* h = all[i];
            printf("%d. %d %s %s %s\n", i, h->id, h->name, h->phone, h->truePhone);
        }else
            break;
}

void addHuman(human* target) {
    if(target->id != -1) {
        all[target->id] = target;
        return;
    }
    int i;
    for(i = 0; i < M2; ++i) {
        if(all[i] == NULL) {
            all[i] = target;
            target->id = i;
            return;
        }
    }
}

int change(int id, const char* value, int name) {
    if(id >= M2 || all[id] == NULL)
        return 0;
    if(name)
        arc(all[id]->name, value);
    else {
        arc(all[id]->phone, value);
        arcPhone(all[id]->truePhone, getTruePhone(value));
    }
    return 1;
}

human** find(char* str) {
    human** result = malloc(M2 * sizeof(human));
    int i, j = 0;
    for(i = 0; i < M2; ++i) {
        if(all[i] == NULL)
            break;
        else {
            human* h = all[i];
            int a = contains(h->name, str);
            char* true = getTruePhone(str);
            int b = equals(h->truePhone, true);
            free(true);
            if(a || b)
                result[j++] = h;
        }
    }
    return result;
}

int removeHuman(int id) {
    if(id >= M2 || all[id] == NULL)
        return 0;
    free(all[id]);
    all[id] = NULL;
    int i;
    for(i = id + 1; i < M2; ++i)
        if(all[i] != NULL) {
            all[i - 1] = all[i];
            all[i] = NULL;
            all[i - 1]->id = i - 1;
        }else
            break;
    return 1;
}

void read() {
    int id;
    while(!feof(file)) {
        char name[M], phone[M];
        int i;
        for(i = 0; i < M; ++i) {
            name[i] = 0;
            phone[i] = 0;
        }
        fscanf(file, "%d %s %s", &id, name, phone);
        if(!strlen(name) || !strlen(phone))
            continue;
        human* toAdd = malloc(sizeof(human));
        toAdd->id = id;
        arc(toAdd->name, name);
        arc(toAdd->phone, phone);
        arcPhone(toAdd->truePhone, getTruePhone(phone));
        addHuman(toAdd);
    }
}

int main(int argc, char** argv) {
    arc(filename, argv[1]);
    file = fopen(filename, "a+");
    rewind(file);
    read();
    
    char cmd[M];
    scanf("%s", cmd);
    while(1) {
        if(equals(cmd, "create")) {
            char name[M], phone[M];
            int i;
            for(i = 0; i < M; ++i) {
                name[i] = 0;
                phone[i] = 0;
            }
            scanf("%s %s", name, phone);
            human* h = malloc(sizeof(human));
            h->id = -1;
            arc(h->name, name);
            arc(h->phone, phone);
            arcPhone(h->truePhone, getTruePhone(phone));
            addHuman(h);
            rewrite();
        }else if(equals(cmd, "delete")) {
            int id;
            scanf("%d", &id);
            if(removeHuman(id))
                rewrite();
            else
                error("There's no record with given id!");
        }else if(equals(cmd, "exit")) {
            break;
        }else if(equals(cmd, "change")) {
            int id;
            scanf("%d", &id);
            char mode[M], value[M];
            int i;
            for(i = 0; i < M; ++i) {
                mode[i] = 0;
                value[i] = 0;
            }
            scanf("%s %s", mode, value);
            if(equals(mode, "name")) {
                if(change(id, value, 1))
                    rewrite();
                else
                    error("There's no record with given id!");
            }else if(equals(mode, "number")) {
                if(change(id, value, 0))
                    rewrite();
                else
                    error("There's no record with given id!");
            }else
                error("Unknown subcommand for 'change'!");
        }else if(equals(cmd, "find")) {
            char str[M];
            int i;
            for(i = 0; i < M; ++i)
                str[i] = 0;
            scanf("%s", str);
            human** found = find(str);
            for(i = 0; i < M2; ++i) {
                human* h = found[i];
                if(h == NULL)
                    break;
                printf("%d %s %s\n", h->id, h->name, h->phone);
            }
            free(found);
        }else if(equals(cmd, "print")) {
            printAll();
        }else {
            error("There's no such command!");
        }
        scanf("%s", cmd);
    }
    
    int i;
    for(i = 0; i < M2; ++i)
        free(all[i]);
    fclose(file);
    return (EXIT_SUCCESS);
}