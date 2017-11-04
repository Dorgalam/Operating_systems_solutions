#include "main.h"

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
    while (head) {
        tmp = head;
        head = head->next;
        free(tmp);
    }
}


int main(int argc, const char *argv[])
{
    int64 input, mask = 0b1;
    char x,y, runOn;
    polygonList.head = NULL;
    polygonList.tail = NULL;
    int numVertices, i = 0;
    bool done = false;

    while (!done) {
        scanf("%16llx", &input);
        if (isNewPolygon(input)) {
            runFunction(ADD_POLYGON, input);
        }
        runOn = ALL_TRIANGLES;
        if (runOn == THIS_POLYGON) {
            runFunction(DO_CURRENT, input);
        } else {
            runFunction(DO_ALL, input);
        }
        
        done = isLastIteration(input);
    }
    freeList();
    return 0;
}