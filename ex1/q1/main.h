//
//  main.c
//  HW1
//
//  Created by Dor Galam on 03/11/2017.
//  Copyright © 2017 Dor Galam. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "math.h"

#define ADD_POLYGON 0
#define PERIMETER 1
#define AREA 2
#define PRINT_POLYGON 3
#define DO_CURRENT 4
#define DO_ALL 5
#define true 1
#define false 0
#define TRIANGLE 0
#define SQUARE 4
#define THIS_POLYGON 0
#define ALL_TRIANGLES 1
#define ALL_SQUARES 2
#define ALL_POLYGONS 3
#define WRITE_VERTICES 0b1000
#define CALC_PERIMETER 0b10000
#define CALC_AREA 0b100000

typedef long long unsigned int64;
typedef unsigned char bool;

void add_polygon(int64);
// add new polygon to the list
void perimeter(int64);
// calculate and print the perimeter
void area(int64);
// calculate and print the area
void print_polygon(int64);
// print the type of polygon and its vertices
void do_current(int64);
// do the operations on the current polygon.
void do_all(int64);
// do the operations in the parameter on the list
void runFunction(char func, int64 funcInput);

bool isLastIteration(int64 input);
bool isNewPolygon(int64 input);
char getPolygonType(int64 input);
char getAction(int64 input);
char getWhomRunOn(int64);
void getXAndY(int64 input, char *x, char *y, int vertixNumber);
void freeList();
float distanceBetweenXAndY(char x1, char y1, char x2, char y2);
void runAction(char action, int64 input);

void (*polygon_functions[6])(int64) = {
    add_polygon,
    perimeter,
    area,
    print_polygon,
    do_current,
    do_all
};
    
typedef struct listNode {
    int64 polygon;
    struct listNode *next;
} listNode;

typedef struct List {
    listNode *head;
    listNode *tail;
} List;