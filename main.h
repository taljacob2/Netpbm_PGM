#define _CRT_SECURE_NO_WARNINGS
#ifndef ____MAIN_H____
#define ____MAIN_H____
#include "utils.h"


/********************QUESTIONS********************/

/*********** Q0 ***********/

#define Q0 \
	printf("______Q0______:\n");\
	grayImage matrix=buildgrayImage();\
	fillmatrix(matrix);\
	printmatrix(matrix);\

/*********** Q1 ***********/

#define Q1 \
	printf("______Q1______:\n");\
	imgPos pos = Q1_KERNEL_POSITION;\
	Segment *seg;\
	unsigned char threshold;\
	printf("Please enter a threshold: ");\
	scanf("%hhu", &threshold);\
	seg=findSingleSegment(&matrix, pos, threshold);\
	printSegment(&matrix, *seg, threshold);\

/*********** Q2 ***********/

#define Q2 \
	printf("______Q2______:\n");\
	imgPosCell **list;\
	unsigned int size=findAllSegments(&matrix, threshold, &list);\
	printAllSegments(&matrix, &list, size);\


/*********** Q3 ***********/

#define Q3 \
	printf("______Q3______:\n");\
	grayImage *newImage=colorSegments(&matrix,list,size);\
	printf("\nNew matrix after Q3 is:\n");\
    printmatrix(*newImage);\

/*********** Q4 ***********/

#define Q4 \
	printf("______Q4______:\n");\
    printf("Please enter the name of the pgm file to import (followed by '.pgm' type): ");\
	char c;\
	while((c = getchar()) != '\n'); /* clear buffer */\
    char *fileName = my_getline(stdin);\
    grayImage *PGM_img = readPGM(fileName);\
	printf("succeed import from %s\n", fileName);\

/*********** Q5 ***********/

#define Q5 \
	printf("______Q5______:\n");\
	unsigned char z_range;\
	printf("Please enter the compression level you wish ('Z - Range') (should be a power of 2): ");\
	scanf("%hhu", &z_range);\
	getchar();\
	printf("Please enter the name of the bin file to export (followed by '.bin' type): ");\
	char* compressed_file_name;\
	my_gets(&compressed_file_name);\
	saveCompressed(compressed_file_name, PGM_img, z_range);\
	printf("succeed to compress %s\n", compressed_file_name);\

/*********** Q6 ***********/

#define Q6 \
	printf("______Q6______:\n");\
	printf("Please enter the name of the pgm file to export (followed by '.pgm' type): ");\
	char *pgm_file_name;\
	my_gets(&pgm_file_name);\
	convertCompressedImageToPGM(compressed_file_name, pgm_file_name);\
	printf("succeed to export %s\n", pgm_file_name);\



/********************FREES********************/

/* this Macro frees the dynamic memory of the needed Questions accordingly */

#define F_Q0 \
	free2DArray(matrix.pixels, matrix.rows);

#define F_Q1 \
	freeSegment(seg);

#define F_Q2 \
	free_segment_Array(list, size);

#define F_Q3 \
	free_grayImage(newImage);

#define F_Q4 \
	free_grayImage(PGM_img);\
	free(fileName);\

#define F_Q5 \
	free(compressed_file_name);\

#define F_Q6 \
	free(pgm_file_name);\





#endif
