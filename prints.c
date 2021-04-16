#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "grayImage.h"
#include "prints.h"

/* -------Static Declerations------ */
static void print_sn_Rec(grayImage* img, treeNode* current_root, unsigned int depth);

/* ------Functions------ */

void printSegment(grayImage* img, Segment seg, unsigned char threshold)
{
	unsigned int depth = 1;
	printf("\nSeg size = %u\nThreshold=%hhu\n", seg.size, threshold);
	printf("ROOT:\n");
	print_sn_Rec(img, seg.root, depth);
}
static void print_sn_Rec(grayImage* img, treeNode* current_root, unsigned int depth)
{
	unsigned int i = 0, copy_depth;

	printf("[%hu=(%d,%d)]", img->pixels[current_root->position[0]][current_root->position[1]], current_root->position[0], current_root->position[1]); /* print root */

	if (current_root->similar_neighbors[i]) { /* if there are childs, print them *//* as long as there is a sn array, print sn arrays of each member */
		printf(" : childs are ->");
		for (i = 0; current_root->similar_neighbors[i]; i++) { /* check for every neighbor inside this array */
			printf(" ,[%hu=(%d,%d)]", img->pixels[current_root->similar_neighbors[i]->position[0]][current_root->similar_neighbors[i]->position[1]], current_root->similar_neighbors[i]->position[0], current_root->similar_neighbors[i]->position[1]);
		}
		putchar('\n');

		for (copy_depth = depth; copy_depth > 0; copy_depth--) { /* print spaces */
			printf("       ");
		}
		printf("<%d> ", depth);
		printf("extend childs: \n\n");

		for (i = 0; current_root->similar_neighbors[i]; i++) { /* check for every neighbor inside this array */
			for (copy_depth = depth; copy_depth > 0; copy_depth--) { /* print spaces */
				printf("       ");
			}
			printf("<%d> ", depth);
			print_sn_Rec(img, current_root->similar_neighbors[i], depth + 1);
		}
	}
	else putchar('\n');
}
void printmatrix(grayImage mat)
{
	int i, j;
	for (i = 0; i < mat.rows; i++)
	{
		for (j = 0; j < mat.cols; j++)
		{
			printf("%3hu ", mat.pixels[i][j]);
		}
		printf("\n");
	}
}
void fprintmatrix(FILE* fp, grayImage mat)
{
	int i, j;
	for (i = 0; i < mat.rows; i++)
	{
		for (j = 0; j < mat.cols; j++)
		{
			fprintf(fp, "%3hu ", mat.pixels[i][j]);
		}
		fprintf(fp, "\n");
	}
}
void printFlagMatrix(unsigned char** flagMatrix, grayImage mat)
{
	PRINT_UL;
	printf("Flag Matrix is:\n");
	int i, j;
	for (i = 0; i < mat.rows; i++)
	{
		for (j = 0; j < mat.cols; j++)
		{
			printf("%3hu ", flagMatrix[i][j]);
		}
		printf("\n");
	}
}
void printAllSegments(grayImage* img, imgPosCell*** list, unsigned int size)
{
	printf("\nArray of List size=%d\n", size);
	imgPosCell* p = (*list)[0];
	unsigned int i;
	printf("Print Lists: \n");
	for (i = 0; i < size; i++)
	{
		p = (*list)[i];
		while (p)
		{
			if (p->next)
				printf("[%d=(%d,%d)] -> ", img->pixels[p->position[0]][p->position[1]], p->position[0], p->position[1]);
			else
				printf("[%d=(%d,%d)] ", img->pixels[p->position[0]][p->position[1]], p->position[0], p->position[1]);
			p = p->next;
		}
		printf("\n");
	}
}