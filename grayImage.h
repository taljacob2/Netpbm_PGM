#define _CRT_SECURE_NO_WARNINGS
#ifndef __grayImage
#define __grayImage
#define ERROR 1
#define EMPTY 0
#define FULL 1
#define TRUE 1
#define FALSE 0
#include <stdio.h>


typedef struct _grayImage {
    unsigned short rows, cols;
    unsigned char** pixels;
}grayImage;

typedef unsigned short imgPos[2];
typedef unsigned char BOOL;

typedef struct _treeNode {
    imgPos position;
    struct _treeNode** similar_neighbors;
}treeNode;
typedef struct _segment {
    treeNode* root;
    unsigned int size;
}Segment;

typedef struct _imgPosCell
{
    imgPos position;
    struct _imgPosCell* next, * prev;
}imgPosCell;

grayImage buildgrayImage(); /*input:nothing return:grayImage struct */
void fillmatrix(grayImage matrix);
Segment* findSingleSegment(grayImage* img, imgPos kernel, unsigned char threshold);
unsigned char** buildFlagMatrix(unsigned short rows, unsigned short cols); /* Build flag matrix with required rows and cols and init with 0*/
void createSimilarNeighbors(grayImage* img, unsigned char** flagMatrix, treeNode* current_root, unsigned char threshold, unsigned int* size, unsigned int* current_sn_size, imgPos kernel); /* Create similar neighbors 2d array */
void findSingleSegmentRec(grayImage* img, unsigned char** flagMatrix, treeNode* current_root, unsigned char threshold, unsigned int* size, imgPos kernel); /* Rec function to create Neighbors array */
treeNode* buildTreeNode(imgPos pos); /* Build Tree Node and return it */
grayImage* colorSegments(grayImage* img, imgPosCell** segments, unsigned int size); /* change each segment in array to one value */
grayImage* readPGM(char* fname); /* create grayImage struct from file */
void saveCompressed(char* file_name, grayImage* image, unsigned char reduced_gray_levels); /* compress file and save in bin file */
void convertCompressedImageToPGM(char* compressed_file_name, char* pgm_file_name); /* create pgm file from compressed file */
void convertCompressedImageToPGM(char* compressed_file_name, char* pgm_file_name);
#endif
