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

// void runReaderProcess(int writerType, int fd[3][2]) {
//     char buff[3], polygonBuffer[80];
//     unsigned int polygonParts[2];
//     bool writeVal = false;
//     int64 nextPolygon;
//     if (writerType == 32) {
//         close(fd[0][1]);
//         close(fd[1][0]);
//         close(fd[1][1]);
//         close(fd[2][0]);

//     } else {
//         close(fd[0][1]);
//         close(fd[0][0]);
//         close(fd[1][1]);
//         close(fd[2][0]);
//     }
//     if(writerType == 32) {
//         read(fd[0][0], buff, 1);
//         scanf("%x", &polygonParts[0]);
//         scanf("%x", &polygonParts[1]);
//         nextPolygon = polygonParts[1];
//         nextPolygon = nextPolygon << 32;
//         nextPolygon += polygonParts[0];
//     } else {
//         printf("%d\n", writerType); 
//         read(fd[1][0], buff, 1);
//         scanf("%16llx", &nextPolygon);
//     }
//     sprintf(polygonBuffer, "%16llx", nextPolygon);
//     write(fd[2][1], polygonBuffer, strlen(polygonBuffer) + 1);
//     exit(0);
// }
 

void runWriterProcess(char *outFile, int writerFd[2]) {
    close(writerFd[1]);
    close(0);
    dup(writerFd[0]);
    close(writerFd[0]);
    char *const params[] = {"./writer", outFile, NULL};
    execv("./writer", params);
}

void seekNext(int nextNum) {
    bool done = false;
    int in;
    int64 dummy;
    long unsigned dummya, dummyb;
    while (!done) {
        scanf("%d", &in);
        if (in != nextNum) {
            if (in == 32) {
                scanf("%lx %lx", &dummya, &dummyb);
            } else {
                scanf("%16llx", &dummy);
            }
        } else {
            done = true;
        }
    }
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
        for(;;) {
            read(fd32[0], runSignal, 2);
            if (runSignal[0] != '1') {
                break;
            }
            seekNext(32);
            scanf("%lx", &polygonParts[0]);
            scanf("%lx", &polygonParts[1]);
            nextPolygon = polygonParts[1];
            nextPolygon = nextPolygon << 32;
            nextPolygon += polygonParts[0];
            sprintf(polygonBuffer, "%16llx", nextPolygon);
            write(fdBoth[1], polygonBuffer, (int)strlen(polygonBuffer));
        }
        exit(EXIT_SUCCESS);
    }
    if ((*reader64pid = fork()) == 0) {
        close(fd32[0]);
        close(fd32[1]);
        close(fd64[1]);
        close(fdBoth[0]);
        for(;;) {
            read(fd64[0], runSignal, 2);
            if (runSignal[0] != '1') {
                break;
            }
            seekNext(64);
            scanf("%16llx", &nextPolygon);
            sprintf(polygonBuffer, "%16llx", nextPolygon);
            write(fdBoth[1], polygonBuffer, 16);
        }
        exit(EXIT_SUCCESS);
    }
    close(fd32[0]);
    close(fd64[0]);
    close(fdBoth[1]);
    for(int i = 0 ; i < 3; ++i) {
        scanf("%d", &readerToRun);
        printf("next is %d\n", readerToRun);
        if (readerToRun == 32) {
            write(fd32[1], "1\0", 2);
            scanf("%lx %lx", &polygonParts[0], &polygonParts[1]);
        } else {
            write(fd64[1], "1\0", 2);
            scanf("%16llx", &nextPolygon);
        }
        int readAm = read(fdBoth[0], polygonBuffer, 16);
        printf("%s was read which was %d long\n", polygonBuffer, readAm);
    }
    write(fd32[1], "0\n", 2);
    write(fd64[1], "0\n", 2);
}

int main(int argc, const char *argv[])
{   

    pid_t writerPid, reader32pid, reader64pid;
    int writerFd[2];
    char outFile[11];
    scanf("%s", outFile);
    pipe(writerFd);
    if (fork() == 0) {
        runWriterProcess(outFile, writerFd);
    } else {
        close(writerFd[0]);
        close(1);
        dup(writerFd[1]);
        close(writerFd[1]);
        createReaders(&reader32pid, &reader64pid);
    }

//     for (int i = 0; i < 3; ++i) //one is with 32, two is with 64 and three is for both
//         pipe(fd[i]);
//     if ((childpid[1] = fork()) == 0) {
//         runReaderProcess(32, fd);
//     }
//     if ((childpid[2] = fork()) == 0) {
//         runReaderProcess(64, fd);
//     }
//     pipe(writerFd);

//     int nextInputSize;
//     unsigned int polygonParts[2];
//     char input[3];
//     int64 nextWholePolygon;
//     close(fd[0][0]);
//     close(fd[1][0]);
//     close(fd[2][1]);
//     int amountRead;
//     for(int i = 0 ; i < 2; ++i) {
//         read(0, input, 2);
//         if (atoi(input) == 32) {
//             write(fd[0][1], "1", 1);
//         } else if (nextInputSize == 64) {
//             write(fd[1][1], "1", 1);
//         }
//         read(fd[2][0], buff, 17);
//         sscanf(buff, "%16llx", &nextWholePolygon);
//         buff[0] = '\0';
//         printf("%16llx\n", nextWholePolygon);
//    }
//     while(wait(NULL)>0);
    // int64 input, mask = 0b1;
    // char x,y, runOn;
    // polygonList.head = NULL;
    // polygonList.tail = NULL;
    // int numVertices, i = 0;
    // bool done = false;
    // while (!done) {
    //     scanf("%16llx", &input);
    //     runOn = getWhomRunOn(input);
    //     if (isNewPolygon(input)) {
    //         runFunction(ADD_POLYGON, input);
    //     }
    //     if (runOn == THIS_POLYGON) {
    //         runFunction(DO_CURRENT, input);
    //     } else {
    //         runFunction(DO_ALL, input);
    //     }
        
    //     done = isLastIteration(input);
    // }
    // freeList();
    return 0;
}