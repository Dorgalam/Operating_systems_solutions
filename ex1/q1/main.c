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

float edgeLength(char x1, char y1, char x2, char y2) {
	return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
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
    char x1, x2, x3, y1, y2, y3,x4,y4, type = getPolygonType(input);
    float perimeter;
    getXAndY(input, &x1, &y1, 0);
    getXAndY(input, &x2, &y2, 1);
	getXAndY(input, &x3, &y3, 2);
    if (type == SQUARE) {
		getXAndY(input, &x4, &y4, 3);
        perimeter = edgeLength(x1, y1, x2, y2) + edgeLength(x2, y2, x3, y3) + edgeLength(x3, y3, x4, y4) + edgeLength(x4, y4, x1, y1);
    } else {
        perimeter = edgeLength(x1, y1, x2, y2) + edgeLength(x2, y2, x3, y3) + edgeLength(x3, y3, x1, y1);
    }
    printf("\t perimeter = %.1f\n", perimeter);
}

void area(int64 input) {
    // Ax  ( By − Cy ) + Bx  ( Cy − Ay ) + Cx  ( Ay − By )  
    char x1, x2, x3, y1, y2, y3,y4,x4, type = getPolygonType(input);
    float area;
	int x[4],y[4];
    getXAndY(input, &x1, &y1, 0);
    getXAndY(input, &x2, &y2, 1);
    getXAndY(input, &x3, &y3, 2);
	x[0] = x1;
	x[1] = x2;
	x[2] = x3;
    y[0] = y1;
	y[1] = y2;
	y[2] = y3; 
	if (type == SQUARE) {
		getXAndY(input, &x4, &y4, 3);
		x[3] = x4;
		y[3] = y4;
        area = polygonArea(x,y, 4);
    } else {
        area = polygonArea(x,y,3);
    }
    printf("\t area = %.1f\n", area);
}

float polygonArea(int *X,int *Y,int numPoints) 
{ 
  int area = 0,i;         // Accumulates area in the loop
  int j = numPoints-1;  // The last vertex is the 'previous' one to the first

  for (i=0; i<numPoints; i++)
    { 
		area +=  (X[j]+X[i]) * (Y[j]-Y[i]); 
		j = i;  //j is previous vertex to i
    }
  return area/2.0;
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
        runOn = getWhomRunOn(input);
        if (isNewPolygon(input)) {
            runFunction(ADD_POLYGON, input);
        }
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