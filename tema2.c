#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <stdarg.h>
#include <limits.h>

#define START_QUEUE(a)          (((AQ) a)->start)
#define END_QUEUE(a)            (((AQ) a)->end)
#define TOP(a)                  (((AStack)a)->top)

// #define DEBUG_MODE 

#ifdef DEBUG_MODE 
    #define DEBUG_PRINT(msg, ...) logDebugInfo(__LINE__, ' ', msg, ##__VA_ARGS__)
    #define DEBUG_PRINTL(msg, ...) logDebugInfo(__LINE__, '\n', msg, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(msg, ...) (void)0
    #define DEBUG_PRINTL(msg, ...) (void)0
#endif

typedef enum operation_type {
    E_ADD_OPERATION,
    E_DEL_OPERATION,
    E_SET_OPERATION,
    E_IPMIN_OPERATION,
    E_SHOW_OPERATION,
    E_NO_OPERATION
} operation_type;

typedef enum switch_mode {
    E_SWMODE_STACK,
    E_SWMODE_SINGLE
} switch_mode;

typedef struct command {
    operation_type operation;
    union {
        struct {
            int switchID;
            char *switchName;
            char *IPv4;
            switch_mode switchMode;
            int stackID;
            bool isPrincipal;
        } add_args;

        struct {
            int switchID;
        } del_args;

        struct {
            int switchID;
            switch_mode switchMode;
            int stackID;
            bool isPrincipal;
        } set_args;
    };
} command;

typedef struct switch_type {
    int switchID;
    int stackID;
    char *IPv4;
    unsigned int decimal_IPv4;
    char *switchName;
    switch_mode switchMode;
    bool isPrincipal;
} switch_type;

typedef struct cel {
    void *info;
    struct cel *next;
} TCel, *TLDI, **ALDI;

typedef struct stack {
    int elementSize;
    TLDI top;
} TStack, *AStack;

typedef struct coada { 
    int elementSize;
    TLDI start;
    TLDI end;
} TQueue, *AQ;

typedef struct lan_network {
    TQueue individualSwitches;
    AStack standardSwitchStacks;
    int numberOfStacks;   
} lan_network;


lan_network*            CreateLAN(int numberOfStacks);
command*                ReadCommand(FILE *inputFile);
operation_type          ParseOperationType(char *Word);
TLDI                    ReadLine(FILE *inputFile);
AStack                  CreateArrayOfStacks(int numberOfStacks);
int                     ParseSwitchID(char *word);
unsigned int            ParseIPv4(char *word);
switch_mode             ParseSwitchMode (char *word);
int                     ParseStackID(char *word);
bool                    ParseIsPrincipal(char *word);
void                    ParseAddArguments(command *Command, TLDI arguments);
void                    ParseSetArguments(command *Command, TLDI words);
void                    DestroyCommand(command *Command);
void                    DestroyLAN(lan_network *LAN);
void                    DestroySwitch(switch_type *Switch);
void                    DestroyList(TLDI list);
void                    DestroyListOfSwitches(TLDI list);
void                    DestroyQueue(AQ aqueue);
void                    DestroyStack(AStack astack);
void                    logDebugInfo(int line, char delimeter, char *message, ...);
void                    AddSwitchInLAN(lan_network *LAN, TLDI newSwitch);
void                    InsertSwitchInQueue(AQ aqueue, command Command);
void                    InsertSwitchInStack(AStack aStacks, command Command);
switch_type*            CreateSwitch(command *Command);
void                    ShowQueue(TQueue queue, FILE *outputFile);
void                    DeleteSwitchFromLAN(lan_network *LAN, command Command);
void                    SetSwitchInLAN(lan_network *LAN, command *Command);
void                    IPMinOfSwitch(TQueue queue, FILE *outputFile);
void                    ShowLAN(lan_network *LAN, FILE *outputFile);
void                    ParseDelArguments(command *Command, TLDI words);
int                     ConvertIpToDecimal();
void                    ShowStack(AStack aStacks, int numberOfStacks, FILE *outputFile);
void                    PushStack(AStack aStack, TLDI new_switch);
TLDI                    PopStack(AStack aStack);
TLDI                    PeekStack(AStack aStack);
TLDI                    CloneSwitchNode(TLDI new_switch);
void                    SortStack(AStack aStack);
void                    SortedInsertInStack(AStack aStack, TLDI new_switch);
TStack                  CopySwitchesToTempStack(AStack aStack, bool isPrincipal);
TLDI                    CreateSwitchNode(command *Command);
bool                    SetFieldIsPrincipal(bool isPrincipal); 
int                     IsQueueEmpty(AQ queue);
void                    PushSwitchInStack(AStack stack, TLDI switchNode);
int                     CompareSwitches(switch_type *A, switch_type *B);
int                     IsStackEmpty(AStack aStack);
int                     CompareSwitchesID(switch_type *A, switch_type *B);
void                    DeleteSwhitchFromQueue(AQ aqueue, int switchID);
void                    DeleteSwhitchFromStack(AStack aStack, int numberOfStacks, int switchID);
void                    SetSwitchFromQueue(AQ aqueue, AStack aStack, TLDI foundSwitch, command Command);
void                    SetSwitchFromStack(lan_network *LAN, TLDI foundSwitch, command Command);
void                    CopyQueue(AQ queue, AQ auxQueue);
void                    CopyStack(AStack auxStack, AStack stack);
TLDI                    ExtractSwitchFromStack(AStack aStack, int switchID);
TLDI                    ExtractSwitchFromQueue(AQ aqueue, int switchID);
void                    DestroySwitchNode(TLDI switchInUse);
void                    SetNewArgsForSwitch(TLDI foundSwitch, command *Command);

// argv[1] = input file, argv[2] = output file
int main(int argc, char const *argv[]) {
    FILE *inputFile =  fopen(argv[1], "rt");
    if (!inputFile) {
        DEBUG_PRINT("Error! Can't open input file\n");
        return -1;
    }

    FILE *outputFile =  fopen(argv[2], "wt");
    if (!outputFile) {
        DEBUG_PRINT("Error! Can't open output file\n");
        return -1;
    }

    int numberOfStacks;
    fscanf(inputFile, "%d ", &numberOfStacks);
    lan_network *LAN = CreateLAN(numberOfStacks);

    do {
        DEBUG_PRINTL("Citeste comanda...");
        command *Command = ReadCommand(inputFile);
        if (!Command) {
            break;
        }

        DEBUG_PRINTL("Proceseaza comanda...\n");
        switch (Command->operation) {
            case E_ADD_OPERATION: {
                TLDI newSwitch = CreateSwitchNode(Command);
                AddSwitchInLAN(LAN, newSwitch);
            } break;
            case E_DEL_OPERATION: {
                DeleteSwitchFromLAN(LAN, *Command);
            } break;
            case E_SET_OPERATION: {
                SetSwitchInLAN(LAN, Command);
            } break;
            case E_IPMIN_OPERATION: {
                IPMinOfSwitch(LAN->individualSwitches, outputFile);
            } break;
            case E_SHOW_OPERATION: {
                ShowLAN(LAN, outputFile);
            } break;
            default: {
                DEBUG_PRINTL("Default case at line:");
            } break;
        }

        DestroyCommand(Command);
    } while (!feof(inputFile));

    fclose(inputFile);
    fclose(outputFile);
    DestroyLAN(LAN);
    return 0;
}


lan_network * CreateLAN(int numberOfStacks) {
    lan_network * LAN = (lan_network*) malloc(sizeof(lan_network));
    if (!LAN) {
        DEBUG_PRINT("Allocation of LAN failed!");
    }

    LAN->standardSwitchStacks = (AStack) malloc(sizeof(TStack) * numberOfStacks);
    if (!LAN->standardSwitchStacks) {
        DEBUG_PRINT("Allocation of LAN stacks failed!");
    }

    LAN->standardSwitchStacks->elementSize = sizeof(TCel);
    for (int i = 0; i < numberOfStacks; i++) {
        LAN->standardSwitchStacks[i].top = NULL;
    }

    LAN->individualSwitches.elementSize = sizeof(TCel);
    LAN->individualSwitches.start = NULL;
    LAN->individualSwitches.end = NULL;

    LAN->numberOfStacks = numberOfStacks;

    return LAN;
}

command *ReadCommand(FILE *inputFile) {
    TLDI words = ReadLine(inputFile);
    if (words == NULL) {
        return NULL;
    }

    command *Command = malloc(sizeof(command));
    if (!Command) {
        DEBUG_PRINT("Allocation of Command failed\n");
        return NULL;
    }
    
    Command->operation = ParseOperationType(words->info);
    TLDI nextWord = words->next;
    switch(Command->operation) {
        case E_ADD_OPERATION: {
            ParseAddArguments(Command, nextWord);
        } break;
        
        case E_DEL_OPERATION: {
            ParseDelArguments(Command, nextWord);
        } break;
        
        case E_SET_OPERATION: {
            ParseSetArguments(Command, nextWord);
        } break;
        
        default: {
        } break;
    }

    DestroyList(words);
    return Command;
}


TLDI ReadLine(FILE *inputFile) {
    size_t LineLength = 10;
    char *ReadLine = calloc(LineLength, sizeof(char));
    getline(&ReadLine, &LineLength, inputFile);
    if (!ReadLine || strlen(ReadLine) == 0) {
        if (ReadLine) {
            free(ReadLine);
        }
        DEBUG_PRINTL("Reading line failed");
        return NULL;
    }

    char *parsedWord = strtok(ReadLine, " \n");
    TLDI previous = NULL;
    TLDI words = NULL;
    while (parsedWord != NULL) {
        TLDI word = (TLDI) malloc(sizeof(TCel));        
        word->info = (char*) malloc(strlen(parsedWord) + 1);
        word->next = NULL;
        memcpy(word->info, parsedWord, strlen(parsedWord) + 1);
        if (!words) {
            words = word;
        } else {
            previous->next = word;
        }

        previous = word;
        parsedWord = strtok(NULL, " \n");
    }

    free(ReadLine);
    return words;
}

void ParseDelArguments(command *Command, TLDI words) {
    Command->del_args.switchID = ParseSwitchID(words->info);
} 

void ParseSetArguments(command *Command, TLDI words) {
    Command->set_args.switchID = ParseSwitchID(words->info);
    words = words->next;
    Command->set_args.switchMode = ParseSwitchMode(words->info);
    words = words->next;
    if (Command->set_args.switchMode == E_SWMODE_STACK) {
        Command->set_args.stackID = ParseStackID(words->info);
        words = words->next;
        Command->set_args.isPrincipal = ParseIsPrincipal(words->info);
    } 
}

void ParseAddArguments(command *Command, TLDI words) {
    DEBUG_PRINT("Parse [ADD] command: ");
    Command->add_args.switchID = ParseSwitchID(words->info);
    DEBUG_PRINT("switchID= %d ", Command->add_args.switchID);
    words = words->next;
    
    Command->add_args.switchName = (char*) malloc(sizeof(char) * (strlen(words->info) + 1));
    memcpy(Command->add_args.switchName, words->info, strlen(words->info) + 1);
    DEBUG_PRINT("switchName= %s ", Command->add_args.switchName);
    words = words->next;
    
    Command->add_args.IPv4 = (char*) malloc(sizeof(char) * (strlen(words->info) + 1));
    memcpy(Command->add_args.IPv4, words->info, strlen(words->info) + 1);
    DEBUG_PRINT("IPv4= %s ", Command->add_args.IPv4);
    words = words->next;
    
    Command->add_args.switchMode = ParseSwitchMode(words->info);
    DEBUG_PRINT("switchMode= %s ", Command->add_args.switchMode == E_SWMODE_SINGLE ? "SINGLE" : "STACK");
    words = words->next;
    
    if (Command->add_args.switchMode == E_SWMODE_STACK) {
        Command->add_args.stackID = ParseStackID(words->info);
        DEBUG_PRINT("stackID= %d ", Command->add_args.stackID);
        words = words->next;
        
        Command->add_args.isPrincipal = ParseIsPrincipal(words->info);
        DEBUG_PRINT("isPrincipal= %s ", Command->add_args.isPrincipal ? "BASE" : "NON_BASE");
    }
}

int ParseSwitchID(char *word) {
    int Id = atoi(word);
    return Id;
}

unsigned int ParseIPv4(char * word) {
    unsigned int IPv4 = ConvertIpToDecimal(word);
    return IPv4;
}

switch_mode ParseSwitchMode(char *word) {
    if (strcmp(word, "SINGLE") == 0) {
        return E_SWMODE_SINGLE;
    } else {
        return E_SWMODE_STACK;
    }
}

int ParseStackID(char *word) {
    int stackId = atoi(word);
    return stackId;
}

bool ParseIsPrincipal(char *word) {
    if (strcmp(word, "BASE") == 0) {
        return true;
    } else {
        return false;
    }
}

operation_type ParseOperationType(char *Word) {
    if (strcmp("add", Word) == 0) {
        return E_ADD_OPERATION;
    } else if (strcmp("del", Word) == 0) {
        return E_DEL_OPERATION;
    } else if (strcmp("set", Word) == 0) {
        return E_SET_OPERATION;
    } else if (strcmp("show", Word) == 0) {
        return E_SHOW_OPERATION;
    } else if (strcmp("ipmin", Word) == 0) {
        return E_IPMIN_OPERATION;
    } else {
        DEBUG_PRINTL("Can't parse operation %s\n", Word);
    }
    return E_NO_OPERATION;
}

switch_type * CreateSwitch(command *Command) {
    switch_type *new_switch = (switch_type*) malloc(sizeof(switch_type));
    if (!new_switch) {
        DEBUG_PRINTL("Allocation of the new switch failed!");
        return NULL;
    }

    new_switch->switchName = calloc(strlen(Command->add_args.switchName) + 1, sizeof(char));
    if (!new_switch->switchName){
        DEBUG_PRINTL("Allocation of the new switchName failed!");
        return NULL;
    }

    new_switch->IPv4 = calloc(strlen(Command->add_args.IPv4) + 1, sizeof(char));
    if (!new_switch->IPv4) {
        DEBUG_PRINTL("Allocation of the new switchIPv4 failed!");
        return NULL;
    }

    new_switch->switchID = Command->add_args.switchID;
    strcpy(new_switch->switchName, Command->add_args.switchName);
    strcpy(new_switch->IPv4, Command->add_args.IPv4);
    new_switch->decimal_IPv4 = ConvertIpToDecimal(Command->add_args.IPv4);
    new_switch->switchMode = Command->add_args.switchMode;
    if (new_switch->switchMode == E_SWMODE_STACK) {
        new_switch->stackID = Command->add_args.stackID;
        new_switch->isPrincipal = Command->add_args.isPrincipal;
    }

    return new_switch;
}

TLDI CreateSwitchNode(command *Command) {
    TLDI node = (TLDI) malloc(sizeof(TCel));
    if (!node) {
        DEBUG_PRINT("Allocation of new node failed");
        return NULL;
    }

    node->next = NULL;
    node->info = CreateSwitch(Command);
    return node;
}

int IsQueueEmpty(AQ queue) {
    if (!queue->start || !queue->end) {
        return 1;
    }

    return 0;
}

void PushQueue(AQ queue, TLDI new_switch) {
    new_switch->next = NULL;
    if (IsQueueEmpty(queue)) {
        queue->start = new_switch;
    } else {
        queue->end->next = new_switch;
    }

    queue->end = new_switch;
}

TLDI PopQueue(AQ queue) {
    if (IsQueueEmpty(queue)) {
        return NULL;
    }

    TLDI aux = queue->start;
    queue->start = aux->next;
    return aux;
}

TLDI PeekQueue(AQ queue) {
    if (IsQueueEmpty(queue)) {
        return NULL;
    }

    TLDI aux = queue->start;
    return aux;
}

void PushSwitchInQueue(AQ aqueue, TLDI new_switch) {
    TQueue auxQueue = {0};
    switch_type *new_switch_info = new_switch->info;
    TLDI switchInUse = PeekQueue(aqueue);

    while (!IsQueueEmpty(aqueue) && (new_switch_info->decimal_IPv4 > ((switch_type *) (switchInUse->info))->decimal_IPv4)) {
        switchInUse = PopQueue(aqueue);
        PushQueue(&auxQueue, switchInUse);
        switchInUse = PeekQueue(aqueue);
    }

    PushQueue(&auxQueue, new_switch);
    while (!IsQueueEmpty(aqueue)) {
        TLDI start = PopQueue(aqueue);
        PushQueue(&auxQueue, start);
    }

    while (!IsQueueEmpty(&auxQueue)) {
        TLDI start = PopQueue(&auxQueue);
        PushQueue(aqueue, start);
    }
}

//queue-source, auxQueue-destination
void CopyQueue(AQ queue, AQ copy) {
    while (!IsQueueEmpty(queue)) {
        TLDI switchInUse = PopQueue(queue);
        PushQueue(copy, switchInUse);
    }
}

//stack-source, stack-destination
void CopyStack(AStack stack, AStack copy) {
    while (!IsStackEmpty(stack)) {
        TLDI switchInUse = PopStack(stack);
        PushStack(copy, switchInUse);
    }
}

void PushSwitchInStack(AStack stack, TLDI switchNode) {
    TStack aux = {0};
    TLDI top = PeekStack(stack); 
    TLDI oldBase = NULL;
    while (top != NULL && CompareSwitches(top->info, switchNode->info) == -1) {
        TLDI switchInUse = PopStack(stack);
        if (((switch_type*) switchInUse->info)->isPrincipal) {
            ((switch_type*) switchInUse->info)->isPrincipal = false;
            oldBase = switchInUse;
        } else {
            PushStack(&aux, switchInUse);
        }

        top = PeekStack(stack);
    }

    if (IsStackEmpty(stack))
        ((switch_type*) switchNode->info)->isPrincipal = true;

    PushStack(stack, switchNode);
    while (!IsStackEmpty(&aux)) {
        TLDI switchInUse = PopStack(&aux);
        PushStack(stack, switchInUse);
    }

    if (oldBase != NULL) {
        PushSwitchInStack(stack, oldBase);
    }
}

int IsStackEmpty(AStack aStack) {
    if (aStack->top == NULL)
        return 1;
    return 0;
}

void PushStack(AStack aStack, TLDI new_switch) { 
    if (new_switch == NULL) {
        return;
    }

    new_switch->next = NULL;
    if (IsStackEmpty(aStack)) {
        aStack->top = new_switch;
    } else {
        new_switch->next = aStack->top;
        aStack->top = new_switch;
    }
}

TLDI PopStack(AStack aStack) {
    if (IsStackEmpty(aStack)) {
        return NULL;
    }

    TLDI tempNode = aStack->top;
    aStack->top = tempNode->next;
    return tempNode;    
}

TLDI PeekStack(AStack aStack) {
    if (IsStackEmpty(aStack)) {
        return NULL;
    }

    TLDI node = aStack->top;
    return node;
}

// return -1 A < B, 1 A = B
int CompareSwitchesID(switch_type *A, switch_type *B) {
    if (A->switchID < B->switchID) {
        return -1;
    } else if (A->switchID == B->switchID) {
        return 1;
    }

    return 0;
}

// return 1 - A > B, 0 A == B, -1 A < B
int CompareSwitches(switch_type *A, switch_type *B) {
    if (A->isPrincipal && B->isPrincipal) {
        return -1;
    }

    if (A->isPrincipal) {
        return 1;
    } else if (A->isPrincipal == B->isPrincipal) {
        return CompareSwitchesID(A, B);
    } else {
        return -1;
    }
}

void AddSwitchInLAN(lan_network *LAN, TLDI newSwitch) {
    if (((switch_type*) (newSwitch->info))->switchMode == E_SWMODE_SINGLE) {
        PushSwitchInQueue(&LAN->individualSwitches, newSwitch);
    } else {
        PushSwitchInStack(&LAN->standardSwitchStacks[((switch_type*) (newSwitch->info))->stackID], newSwitch);
    }
}

void DeleteSwitchFromLAN(lan_network *LAN, command Command) {
    TLDI foundSwitchInQueue = ExtractSwitchFromQueue(&LAN->individualSwitches, Command.del_args.switchID);
    if (foundSwitchInQueue != NULL) {
        DestroySwitchNode(foundSwitchInQueue);
        return;
    } 

    for (int i = 0; i < LAN->numberOfStacks; i++) {
        TLDI foundSwitchInStack = ExtractSwitchFromStack(&LAN->standardSwitchStacks[i], Command.del_args.switchID);

        if (foundSwitchInStack != NULL) {
            DestroySwitchNode(foundSwitchInStack);
            return;
        }
    }
}

TLDI ExtractSwitchFromStack(AStack aStack, int switchID) {
    TLDI foundSwitch = NULL;
    TStack aux = {0};
    TLDI switchInUse = NULL;
    TLDI lastSwitch = NULL;
    while (!IsStackEmpty(aStack)) {
        switchInUse = PopStack(aStack);
        if (((switch_type*)(switchInUse->info))->switchID == switchID) {
            foundSwitch = switchInUse;                    
        } else {
            lastSwitch = switchInUse;
            PushStack(&aux, switchInUse);
        }
    }

    while (!IsStackEmpty(&aux)) {
        switchInUse = PopStack(&aux);
        if (CompareSwitchesID(switchInUse->info, lastSwitch->info) == 1 && !((switch_type*)(switchInUse->info))->isPrincipal) {
            ((switch_type*)(switchInUse->info))->isPrincipal = true;
        }

        PushStack(aStack, switchInUse);
    }
        
    return foundSwitch;
}

TLDI ExtractSwitchFromQueue(AQ aqueue, int switchID) {
    TQueue aux = {0};
    TLDI switchInUse = NULL;
    TLDI foundSwitch = NULL;
    while (!IsQueueEmpty(aqueue)) {
        switchInUse = PopQueue(aqueue);
        if (((switch_type*) (switchInUse->info))->switchID == switchID) {
            foundSwitch = switchInUse;
        } else {
            PushQueue(&aux, switchInUse);
        }
    }

    CopyQueue(&aux, aqueue);
    return foundSwitch;
}

void SetNewArgsForSwitch(TLDI foundSwitch, command *Command) {
    if (Command->set_args.switchMode == E_SWMODE_STACK) {
        ((switch_type*) (foundSwitch->info))->switchMode = Command->set_args.switchMode;
        ((switch_type*) (foundSwitch->info))->stackID = Command->set_args.stackID;
        ((switch_type*) (foundSwitch->info))->isPrincipal = Command->set_args.isPrincipal;
    } else {
        ((switch_type*) (foundSwitch->info))->switchMode = Command->set_args.switchMode;
    }
}

void SetSwitchInLAN(lan_network *LAN, command *Command) {
    TLDI foundSwitchInQueue = ExtractSwitchFromQueue(&LAN->individualSwitches, Command->set_args.switchID);
    if (foundSwitchInQueue != NULL) {
        SetNewArgsForSwitch(foundSwitchInQueue, Command);
        AddSwitchInLAN(LAN, foundSwitchInQueue);
        return;
    } 

    for (int i = 0; i < LAN->numberOfStacks; i++) {
        TLDI foundSwitchInStack = ExtractSwitchFromStack(&LAN->standardSwitchStacks[i], Command->set_args.switchID);

        if (foundSwitchInStack != NULL) {
            SetNewArgsForSwitch(foundSwitchInStack, Command);
            AddSwitchInLAN(LAN, foundSwitchInStack);
            return;
        }
    }
}

void IPMinOfSwitch(TQueue queue, FILE *outputFile) {
    if (IsQueueEmpty(&queue)) {
        fprintf(outputFile, "ipmin=0\n");
        return;
    }

    TQueue aux = {0};
    unsigned int IPMin = UINT_MAX;
    while (!IsQueueEmpty(&queue)) {
        TLDI switchInUse = PopQueue(&queue);
        if (((switch_type*)(switchInUse->info))->decimal_IPv4 < IPMin) {
            IPMin = ((switch_type*)(switchInUse->info))->decimal_IPv4;
        }

        PushQueue(&aux, switchInUse);
    }

    CopyQueue(&aux, &queue);
    fprintf(outputFile, "ipmin=%u\n", IPMin);
}

void ShowLAN(lan_network *LAN, FILE *outputFile) {
    ShowQueue(LAN->individualSwitches, outputFile);
    ShowStack(LAN->standardSwitchStacks, LAN->numberOfStacks, outputFile);
}

void ShowQueue(TQueue queue, FILE *outputFile) {
    fprintf(outputFile, "{");
    if (IsQueueEmpty(&queue)) {
        DEBUG_PRINT("Empty queue\n\n");
        fprintf(outputFile, "}\n");
        return;
    }

    TQueue aux = {0};
    switch_type *switch_info = NULL;
    while (!IsQueueEmpty(&queue)) {
        TLDI switchInUse = PopQueue(&queue);
        switch_info = switchInUse->info;
        fprintf(outputFile, "%d ", switch_info->switchID);
        PushQueue(&aux, switchInUse);
        DEBUG_PRINT("(id: %d name: %s %u) -> ", switch_info->switchID, switch_info->switchName, switch_info->decimal_IPv4);
    }

    fprintf(outputFile, "}\n");
    CopyQueue(&aux, &queue);
}

void ShowStack(AStack aStacks, int numberOfStacks, FILE *outputFile) {
    for (int i = 0; i < numberOfStacks; i++) {
        fprintf(outputFile, "%d:\n", i);
        if (!IsStackEmpty(&aStacks[i])) {   
            TStack auxStack = {0};
            while (!IsStackEmpty(&aStacks[i])) {
                TLDI switchAux = PopStack(&aStacks[i]);
                switch_type *switchInfo = switchAux->info;
                PushStack(&auxStack, switchAux);
                fprintf(outputFile, "%d %s %s\n", switchInfo->switchID, switchInfo->IPv4, switchInfo->switchName);
            }

            CopyStack(&auxStack, &aStacks[i]);
        }
    }

    fprintf(outputFile, "\n");
}

void DestroyCommand(command *Command) {
        switch (Command->operation) {
        case E_ADD_OPERATION: {
            free(Command->add_args.switchName);
            free(Command->add_args.IPv4);
        } break;
        case E_DEL_OPERATION: {
                
        } break;
        case E_SET_OPERATION: {
                
        } break;
        case E_IPMIN_OPERATION: {

        } break;
        case E_SHOW_OPERATION: {

        } break;
        default: {
            DEBUG_PRINT("Default case reached at line %d!!!!\n", __LINE__);
        } break;
    }

    free(Command);
    Command = NULL;;
}

void DestroyList(TLDI list) {
    for (TLDI aux = list; aux != NULL;) {
        TLDI temp = aux;
        aux = aux->next;
        free(temp->info);
        free(temp);
    }

    list = NULL;
}

void DestroyQueue(AQ aqueue) {
    DestroyListOfSwitches(aqueue->start);
    aqueue = NULL;
}

void DestroyStack(AStack astack) {
    DestroyListOfSwitches(astack->top);
    // free(astack);
}

void DestroyListOfSwitches(TLDI list) {
    for (TLDI aux = list; aux != NULL;) {
        TLDI temp = aux;
        aux = aux->next;
        DestroySwitch(temp->info);
        free(temp);
    }

    list = NULL;
}

void DestroySwitchNode(TLDI switchInUse) {
    DestroySwitch(switchInUse->info);
    free(switchInUse);
}

void DestroySwitch(switch_type *Switch) {
    free(Switch->switchName);
    free(Switch->IPv4);
    free(Switch);
    Switch = NULL;
}

void DestroyLAN(lan_network *LAN) {
    if (LAN->individualSwitches.start != NULL) {
        DestroyQueue(&LAN->individualSwitches);
    }
    
    for (int i = 0; i < LAN->numberOfStacks; i++) {
        DestroyStack(&LAN->standardSwitchStacks[i]);
    }

    free(LAN->standardSwitchStacks);
    free(LAN);
    LAN = NULL;
}

void logDebugInfo(int line, char delimeter, char *message, ...) {
    va_list valist;
    va_start(valist, message);
    vprintf(message, valist);
    if (delimeter == '\n') {
        printf(" %d%c", line, delimeter);
    }

    va_end(valist);
}


int ConvertIpToDecimal(char *ip) {
    char c;
    c = *ip;
    unsigned int integer = 0;
    int val;
    int i,j;

    for (j = 0; j < 4; j++) {
        if (!isdigit(c)) { 
            return 0;
        }

        val = 0;
        for (i = 0; i < 3; i++) {
            if (isdigit(c)) {
                val = (val * 10) + (c - '0');
                c = *++ip;
            } else {
                break;
            }
        }

        if (val < 0 || val > 255) {
            return 0;   
        }  

        if (c == '.') {
            integer = (integer<<8) | val;
            c = *++ip;
        } else if (j == 3 && c == '\0') {
            integer = (integer<<8) | val;
            break;
        }
    }

    if (c != '\0') {
        return 0;   
    }

    return (integer);
}
