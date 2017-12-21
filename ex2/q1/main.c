#include "main.h" 
List polygonList;

void runFunction(char func, int64 funcInput) {
    (*polygon_functions[func])(funcInput);
}


bool isLastIteration(int64 input) {
    return input & 0x1;
}

bool isNewPolygon(int64 input) {
    return (input & 0x2) > 0;
}

char getPolygonType(int64 input) {
    return (input & 0x4);
}

char getAction(int64 input) {
    return (input & 0x38);
}

char getWhomRunOn(int64 input) {
    return (input & 0xc0) >> 6;
}

float distanceBetweenXAndY(char x1, char y1, char x2, char y2) {
    return sqrt(
        pow(x2 - x1, 2) + pow(y2 - y1, 2)
    );
} 

void getXAndY(int64 input, char *_x, char *_y, int vertixNumber) {
    char x,y;
    int64 xMask = 0x3f00;
    int64 yMask = 0xfc000;
    x = (input & (xMask << vertixNumber * 12)) >> (8 + vertixNumber * 12);
    y = (input & (yMask << vertixNumber * 12)) >> (14 + vertixNumber * 12);
    *_x = x < 32 ? x : x ^ 0xc0; // number above 32 -> sign bit should be on across all number
    *_y = y < 32 ? y : y ^ 0xc0;
}

void add_polygon(int64 input) {
    listNode *newItem = (listNode*)malloc(sizeof(listNode));
    newItem->polygon = input;
    newItem->next = NULL;
    if (polygonList.head == NULL) {
        polygonList.head = polygonList.tail = newItem;
    } else {
        polygonList.tail->next = newItem;
        polygonList.tail = newItem;
    }
}

void perimeter(int64 input) {
    char x1, x2, x3, y1, y2, y3, type = getPolygonType(input);
    float perimeter;
    getXAndY(input, &x1, &y1, 0);
    getXAndY(input, &x2, &y2, 1);
    if (type == SQUARE) {
        perimeter = 4 * distanceBetweenXAndY(x1, y1, x2, y2);
    } else {
        getXAndY(input, &x3, &y3, 2);
        perimeter = distanceBetweenXAndY(x1, y1, x2, y2) + distanceBetweenXAndY(x2, y2, x3, y3) + distanceBetweenXAndY(x3, y3, x1, y1);
    }
    printf("\t perimeter = %.1f\n", perimeter);
}

void area(int64 input) {
    // Ax  ( By − Cy ) + Bx  ( Cy − Ay ) + Cx  ( Ay − By )  
    char x1, x2, x3, y1, y2, y3, type = getPolygonType(input);
    float area;
    getXAndY(input, &x1, &y1, 0);
    getXAndY(input, &x2, &y2, 1);
    getXAndY(input, &x3, &y3, 2);
    if (type == SQUARE) {
        area = pow(distanceBetweenXAndY(x1, y1, x2, y2), 2);
    } else {
        area = abs(
            (x1 * (y2 - y3)) + 
            (x2 * (y3 - y1)) + 
            (x3 * (y1 - y2))
        ) / 2.0;
    }
    printf("\t area = %.1f\n", area);
    
}
void print_polygon(int64 input) {
    char x, y, i, numVertices, type;
    type = getPolygonType(input);
    numVertices = type == SQUARE ? 4 : 3;
    printf("%s", type == SQUARE ? "square" : "triangle");
    for (i = 0; i < numVertices; ++i) {
        getXAndY(input, &x, &y, i);
        printf(" {%d, %d}", x, y);
    }
    printf("\n");

}
void do_current(int64 input) {
    char action = getAction(input);
    runAction(action, input);
}

void runAction(char action, int64 input) {
    if ((action & WRITE_VERTICES) > 0) {
        runFunction(PRINT_POLYGON, input);
    }
    if ((action & CALC_PERIMETER) > 0) {
        runFunction(PERIMETER, input);
    }
    if ((action & CALC_AREA) > 0) {
        runFunction(AREA, input);
    }
}

void do_all(int64 input) {
    char action, type, runOn = getWhomRunOn(input);
    bool shouldRunOnThisPolygon;
    action = getAction(input);
    listNode *itr = polygonList.head;
    while (itr != NULL) {
        type = getPolygonType(itr->polygon);
        shouldRunOnThisPolygon = 
            (runOn == ALL_POLYGONS) || 
            (runOn == ALL_TRIANGLES && type == TRIANGLE) ||
            (runOn == ALL_SQUARES && type == SQUARE);
        if (shouldRunOnThisPolygon) {
            runAction(action, itr->polygon);
        }
        itr = itr->next;
    }
}

void freeList() {
    listNode* tmp, *head = polygonList.head;
    while (head != NULL) {
        tmp = head;
        head = head->next;
        free(tmp);
    }
}

bool runOnPolygon(int64 polygon) {
    char runOn;
    runOn = getWhomRunOn(polygon);
    if (isNewPolygon(polygon)) {
        runFunction(ADD_POLYGON, polygon);
    }
    if (runOn == THIS_POLYGON) {
        runFunction(DO_CURRENT, polygon);
    } else {
        runFunction(DO_ALL, polygon);
    }
    bool res =  isLastIteration(polygon);
    return res;
}


    void runWriterProcess(char *outFile, int writerFd[2]) {
        close(writerFd[1]);
        close(STDIN_FILENO);
        dup(writerFd[0]);
        close(writerFd[0]);
        char *const params[] = {"./writer", outFile, NULL};
        execv("./writer", params);
    }
    void reader32(int INSIG, int OUTPUT) {
        long unsigned polygonParts[2];
        char runSignal[2], polygonBuffer[17];
        int64 nextPolygon;
        while(read(INSIG, runSignal, 1) > 0) {
            scanf("%lx", &polygonParts[0]);
            scanf("%lx", &polygonParts[1]);
            nextPolygon = polygonParts[1];
            nextPolygon = nextPolygon << 32;
            nextPolygon += polygonParts[0];
            sprintf(polygonBuffer, "%16llx", nextPolygon);
            write(OUTPUT, polygonBuffer, (int)strlen(polygonBuffer));
        }
        finishError("reader32\0");
        exit(EXIT_SUCCESS);
    }

    void reader64(int INSIG, int OUTPUT) {
        char runSignal[2], polygonBuffer[17];
        int64 nextPolygon;
        while(read(INSIG, runSignal, 1) > 0) {
            scanf("%16llx", &nextPolygon);
            sprintf(polygonBuffer, "%16llx", nextPolygon);
            write(OUTPUT, polygonBuffer, 16);
        }
        finishError("reader64\0");
        exit(EXIT_SUCCESS);
    }

    void runMainLoop(int reader32, int reader64, int readPipe) {
        long unsigned dummy[2];
        int64 bigDummy, nextPolygon;
        char polygonBuffer[17];
        int readerToRun;
        for(;;) {
            scanf("%d", &readerToRun);
            if (readerToRun == 32) {
                write(reader32, "1", 1);
            } else {
                write(reader64, "1", 1);
            }
            read(readPipe, polygonBuffer, 16);
            sscanf(polygonBuffer, "%16llx", &nextPolygon);
            if(runOnPolygon(nextPolygon)) break;
        }
        write(reader64, "0\n", 2);
    }

    void createReaders(pid_t *reader32pid, pid_t *reader64pid) {
        int fd32[2], fd64[2], fdBoth[2], readerToRun;
        long unsigned polygonParts[2];
        int64 nextPolygon, dummy;
        char runSignal[2], polygonBuffer[17];
        polygonBuffer[16] = '\0';
        pipe(fd32);
        pipe(fd64);
        pipe(fdBoth);
        if ((*reader32pid = fork()) == 0) {
            close(fd32[1]);
            close(fd64[0]);
            close(fd64[1]);
            close(fdBoth[0]);
            reader32(fd32[0], fdBoth[1]);
        }
        if ((*reader64pid = fork()) == 0) {
            close(fd32[0]);
            close(fd32[1]);
            close(fd64[1]);
            close(fdBoth[0]);
            reader64(fd64[0], fdBoth[1]);
        }
        close(fd32[0]);
        close(fd64[0]);
        close(fdBoth[1]);
        runMainLoop(fd32[1], fd64[1], fdBoth[0]);
        close(fd32[1]);
        close(fd64[1]);
        wait(NULL);
        wait(NULL);
        finishError("main_process\0");
    }

    void finishError(char processName[13]) {
        char output[50];
        sprintf(output, "%s pid=%d is going to exit\n", processName, getpid());
        write(STDERR_FILENO, output, strlen(output) + 1);
    }

    int main(int argc, const char *argv[])
    {   
        setvbuf(stdin, "\n", _IONBF, 0); // this line is a must for using the program with output redirection
        polygonList.head = NULL;
        polygonList.tail = NULL;
        pid_t writerPid, reader32pid, reader64pid;
        int writerFd[2];
        char outFile[11];
        scanf("%s", outFile);
        pipe(writerFd);
        if (fork() == 0) {
            runWriterProcess(outFile, writerFd);
        } else {
            close(STDOUT_FILENO);
            dup(writerFd[1]);
            close(writerFd[0]);
            close(writerFd[1]);
            createReaders(&reader32pid, &reader64pid);
        }
        freeList();
        return 0;
    }

