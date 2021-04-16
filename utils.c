#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grayImage.h"

/* -------Static Declerations------ */

static void freeTree(treeNode* root); /* free Tree nodes */
static void freeList(imgPosCell* lst_head);

/* ------Functions------ */

void checkalloc(void* p) /*input:pointer return:Exit program if malloc fail*/
{
	if (!p)
	{
		fprintf(stderr, "Allocating failed\n");
		exit(ERROR);
	}
}
void free2DArray(unsigned char** mat, unsigned int rows) /* Free 2D array */
{
	unsigned int i;
	for (i = 0; i < rows; i++)
		free(mat[i]);
	free(mat);
}
char* my_getline(FILE* readFile) /* read line until \n */
{
	char* line = NULL;
	int i = 0, size = 0;
	char c;
	c = fgetc(readFile);
	while (c >= 0 && c != '\n')
	{
		if (i == size)
		{
			size = size * 2 + 1;
			line = (char*)realloc(line, sizeof(char) * size);
			checkalloc(line);
		}
		line[i] = c;
		i++;
		c = fgetc(readFile);

	}
	line = (char*)realloc(line, sizeof(char) * (i + 1));
	checkalloc(line);
	line[i] = '\0';
	return line;
}
int my_gets(char** str)  /* DONT USE GETS *//* INSTEAD */ /* requires <string.h> */
{
	int i, size = 0;
	char c;
	*str = (char*)malloc(0);
	checkalloc(*str);

	for (i = 0; (c = getchar()) != '\n'; i++) {
		if (i == size) {
			size = (size * 2 + 1);
			*str = (char*)realloc(*str, sizeof(char*) * size);
			checkalloc(*str);
		}
		(*str)[i] = c;
	}
	*str = (char*)realloc(*str, sizeof(char*) * (i + 1));
	checkalloc(*str);
	(*str)[i] = '\0';
	/* fseek(stdin,0,SEEK_END); */ /* clear buffer to make sure */
	return i; /* i is actually strlen(*str) */
}
void check_fopen_custom(FILE* fp, char* error_message)
{
	if (!fp) {
		fputs(error_message, stderr);
		exit(ERROR);
	}
}
void check_TypeOfFile(char* nameOfFile, char* dot_TypeOfFile)
{
	/* makes sure if the user inputed the required type of file */
	/* this function searches wether the needed type of file appears in the name of the file inputed by the user */
	char* pureName;
	int i, dot_i = -1;
	/* go to the last dot */
	for (i = 0; nameOfFile[i]; i++) {
		if (nameOfFile[i] == '.')
			dot_i = i;
	}
	if (dot_i == -1) {
		fputs("Please insert: <name of the file><'.'><type of the file>\n", stderr);
		exit(ERROR);
	}

	pureName = (char*)malloc((dot_i + 1) * sizeof(char));
	checkalloc(pureName);
	memcpy(pureName, nameOfFile, dot_i); /* extract the pureName of the file */
	pureName[dot_i] = '\0';

	/* now search for the word ".txt" or ".bin" for example (== dot_TypeOfFile) */
	if (strcmp(nameOfFile + dot_i, dot_TypeOfFile)) {
		fputs("Bad file type inputed...\n", stderr);
		exit(ERROR);
	}
	free(pureName);
	/*return pureName;*/ /* outputs the pureName of the file before the dot and type */
}
void free_grayImage(grayImage* img)
{
	free2DArray(img->pixels, img->rows);
	free(img);
}
void freeSegment(Segment* seg) /* Free Segment */
{
	freeTree(seg->root);
	free(seg);
}
static void freeTree(treeNode* root) /* free Tree nodes */
{
	if (root == NULL)
		return;
	int i;
	for (i = 0; root->similar_neighbors[i]; i++); /* go to the last node */
	for (; i >= 0; i--) {
		freeTree(root->similar_neighbors[i]);
	}
	free(root->similar_neighbors);
	free(root);
}
void free_segment_Array(imgPosCell** list, unsigned int size)
{
	/* free List: (each node is a Segment so wee need to 'freeSegment' the nodes) */
	unsigned int i;
	for (i = 0; i < size; i++) {
		freeList(list[i]);
	}
	free(list);
}
static void freeList(imgPosCell* lst_head)
{
	/* need to free ALL the list from here */
	imgPosCell* currNode = NULL;
	imgPosCell* prevNode = NULL; /* remember the previous Node */

	if (lst_head) { /* only if the List isn't empty */
		currNode = lst_head->next;
		for (prevNode = lst_head; currNode; currNode = currNode->next) {
			free(prevNode);/* free the currNode itself */
			prevNode = currNode;
		}
		free(prevNode);/* free the last prevNode */
	}
	else return; /* the list is already empty */
}
