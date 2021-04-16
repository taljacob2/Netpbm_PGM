#define _CRT_SECURE_NO_WARNINGS
#ifndef ___UTILS___
#define ___UTILS___
#include <stdio.h>
#include "grayImage.h"
#include "imgList.h"


void checkalloc(void* p); /*input:pointer return:Exit program if malloc fail*/
void free2DArray(unsigned char** mat, unsigned int rows); /* Free 2D array */
char* my_getline(FILE* readFile); /* read line until \n */
int my_gets(char** str);
void check_fopen_custom(FILE* fp, char* error_message);
void check_TypeOfFile(char* nameOfFile, char* dot_TypeOfFile);
void free_grayImage(grayImage* img);
void freeSegment(Segment* seg); /* Free Segment */
void free_segment_Array(imgPosCell** list, unsigned int size);
#endif
