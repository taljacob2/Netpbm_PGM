#define _CRT_SECURE_NO_WARNINGS
#ifndef ___PRINTS___
#define ___PRINTS___

/* for prints */
#define PRINT_UL printf("___________________\n")

/*Q1:*/
void printSegment(grayImage* img, Segment seg, unsigned char threshold);
void printmatrix(grayImage mat);
void printFlagMatrix(unsigned char** flagMatrix, grayImage mat);
/*Q2:*/
void printAllSegments(grayImage* img, imgPosCell*** list, unsigned int size);
/*Q6*/
void fprintmatrix(FILE* fp, grayImage mat);
#endif