/* 
 * File:   main2.c
 * Author: RinesThaix
 *
 * Created on 21 February, 13:04
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int defPhone = 32;

void error(char* message) {
    printf("Error occured: %s\n", message);
}

char* mem(int size) {
    return malloc(size * sizeof(char));
}

char* filename;
FILE* file;

int equals(char* a, char* b) {
    return !strcmp(a, b);
}

int findNumbers(const char* a, const char* b) {
    char an[defPhone], bn[defPhone];
    int i, ind1 = 0, ind2 = 0;
    for(i = 0; i < defPhone; ++i)
        if(a[i] >= '0' && a[i] <= '9')
            an[ind1++] = a[i];
    for(i = 0; i < defPhone; ++i)
        if(b[i] >= '0' && b[i] <= '9')
            bn[ind2++] = b[i];
    if(ind1 != ind2)
        return 0;
    for(i = 0; i < ind1; ++i)
        if(an[i] != bn[i])
            return 0;
    return 1;
}

int findNames(const char* a, const char* b) {
    char an[defPhone], bn[defPhone];
    int i;
    for(i = 0; i < defPhone; ++i)
        if(a[i] >= 'A' && a[i] <= 'Z') {
            an[i] = 'a' + a[i] - 'A';
        }else {
            an[i] = a[i];
        }
    for(i = 0; i < defPhone; ++i)
        if(b[i] >= 'A' && b[i] <= 'Z') {
            bn[i] = 'a' + b[i] - 'A';
        }else {
            bn[i] = b[i];
        }
    return strstr(an, bn) != NULL;
}

typedef struct {
    int id;
    char* name;
    char* phone;
} human;

void _killHuman(human* target) {
    free(target->name);
    free(target->phone);
}

human* getHuman(char* name, char* phone) {
    human* h = malloc(sizeof(human));
    h->id = -1;
    h->name = name;
    h->phone = phone;
    return h;
}

human* getFullHuman(int id, char* name, char* phone) {
    human* h = getHuman(name, phone);
    h->id = id;
    return h;
}

typedef struct {
    human** humans;
    int size;
    void (*add)(human*);
    human** (*find)(char*);
    void (*kill)();
    int (*change)(int, char*, int);
    int (*delete)(int);
} storage;

storage ppl;

void rewriteEverything() {
    fclose(file);
    file = fopen(filename, "w+");
    int i;
    for(i = 0; i < ppl.size; ++i) {
        if(ppl.humans[i] != NULL) {
            human* h = ppl.humans[i];
            if(i == 0)
                fprintf(file, "%d %s %s", h->id, h->name, h->phone);
            else
                fprintf(file, "\n%d %s %s", h->id, h->name, h->phone);
        }
    }
    fclose(file);
    file = fopen(filename, "a");
}

void _killStorage() {
    int i;
    for(i = 0; i < ppl.size; ++i)
        free(ppl.humans[i]);
    free(ppl.humans);
}

void _addHuman(human* target) {
    if(target->id != -1) {
        int id = target->id;
        if(id < ppl.size)
            ppl.humans[id] = target;
        else {
            while(id >= ppl.size)
                ppl.size <<= 1;
            ppl.humans = realloc(ppl.humans, ppl.size * sizeof(human));
            if(!ppl.humans)
                error("Couldn't do realloc!");
            ppl.humans[id] = target;
        }
        return;
    }
    int i;
    for(i = 0;; ++i) {
        int add = 0;
        if(i == ppl.size) {
            ppl.humans = realloc(ppl.humans, (ppl.size <<= 1) * sizeof(human));
            if(!ppl.humans)
                error("Couldn't do realloc!");
            add = 1;
        }else
            add = ppl.humans[i] == NULL;
        if(add) {
            ppl.humans[i] = target;
            target->id = i;
            break;
        }
    }
}

human** _findHuman(char* prefix) {
    human** found = malloc(sizeof(ppl.humans));
    int i, index = 0;
    for(i = 0; i < ppl.size; ++i)
        found[i] = NULL;
    for(i = 0; i < ppl.size; ++i)
        if(ppl.humans[i] != NULL &&
                (findNames(ppl.humans[i]->name, prefix) || findNumbers(ppl.humans[i]->phone, prefix)))
            found[index++] = ppl.humans[i];
    return found;
}

int _delete(int id) {
    human* target;
    if(id >= ppl.size || (target = ppl.humans[id]) == NULL)
        return 0;
    _killHuman(target);
    ppl.humans[id] = NULL;
    int i;
    for(i = id + 1; i < ppl.size; ++i) {
        human* current = ppl.humans[i];
        if(current == NULL)
            break;
        ppl.humans[i - 1] = current;
        ppl.humans[i] = NULL;
        current->id = i - 1;
    }
    rewriteEverything();
    return 1;
}

int _change(int id, char* value, int number) {
    human* target;
    if(id >= ppl.size || (target = ppl.humans[id]) == NULL)
        return 0;
    if(number)
        target->phone = value;
    else
        target->name = value;
    rewriteEverything();
    return 1;
}

storage getStorage() {
    storage* s = malloc(sizeof(storage));
    s->size = 2;
    s->humans = malloc(sizeof(human) << 1);
    int i;
    for(i = 0; i < 2; ++i)
        s->humans[i] = NULL;
    s->add = _addHuman;
    s->kill = _killStorage;
    s->find = _findHuman;
    s->change = _change;
    s->delete = _delete;
    return *s;
}

void printAll() {
    int i;
    for(i = 0; i < ppl.size; ++i)
        if(ppl.humans[i] != NULL)
            printf("%d, %d %s %s\n", i, ppl.humans[i]->id, ppl.humans[i]->name, ppl.humans[i]->phone);
}

void read() {
    int id;
    while(!feof(file)) {
        char* name = mem(defPhone), *phone = mem(defPhone);
        fscanf(file, "%d %s %s", &id, name, phone);
        if(!strlen(name) || !strlen(phone))
            continue;
        ppl.add(getFullHuman(id, name, phone));
    }
}

int main(int argc, char** argv) {
    filename = argv[1];
    file = fopen(filename, "a+");
    rewind(file);
    ppl = getStorage();
    read();
    
    char* cmd = mem(64);
    scanf("%s", cmd);
    while(!equals(cmd, "exit")) {
        if(equals(cmd, "create")) {
            char* name = mem(defPhone), *phone = mem(defPhone);
            scanf("%s %s", name, phone);
            human* toAdd = getHuman(name, phone);
            ppl.add(toAdd);
            fprintf(file, "\n%d %s %s", toAdd->id, name, phone);
            fclose(file);
            file = fopen(filename, "a");
        }else if(equals(cmd, "find")) {
            char prefix[defPhone];
            scanf("%s", prefix);
            human** found = ppl.find(prefix);
            if(found[0] == NULL)
                error("No one could be found!");
            else {
                int i;
                for(i = 0; i < ppl.size; ++i)
                    if(found[i] != NULL)
                        printf("%d %s %s\n", found[i]->id, found[i]->name, found[i]->phone);
                    else
                        break;
            }
            free(found);
        }else if(equals(cmd, "change")) {
            int id;
            char* subcmd = mem(defPhone), *value = mem(defPhone);
            scanf("%d %s %s", &id, subcmd, value);
            if(equals(subcmd, "number")) {
                if(!ppl.change(id, value, 1))
                    error("There's no record with given id!");
            }else if(equals(subcmd, "name")) {
                if(!ppl.change(id, value, 0))
                    error("There's no record with given id!");
            }else {
                error("It's not a subcommand for 'change'!");
            }
        }else if(equals(cmd, "delete")) {
            int id;
            scanf("%d", &id);
            if(!ppl.delete(id))
                error("There's no record with given id!");
        }else {
            error("Unknown command!");
        }
        free(cmd);
        cmd = mem(64);
        scanf("%s", cmd);
    }
    printf("Good bye, sir.");
    free(cmd);
    ppl.kill();
    fclose(file);
    return (EXIT_SUCCESS);
}