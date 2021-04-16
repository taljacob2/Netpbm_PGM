#define _CRT_SECURE_NO_WARNINGS
#include "utils.h"
#include "imgList.h"
#include "prints.h"

/* -------Static Declerations------ */
static BOOL getMinInImage(grayImage* img, unsigned char*** flagMatrix, imgPos** minPos); /*put min position in min position return FALSE if not found else TRUE */
static imgPosCell* buildSingleSegmentList(treeNode* root); /* Create segment list and return it */
static void mergeSortArray(imgPosCell** arr, unsigned int* listLength, unsigned int size); /* Merge sort array from biggest list to smallest */
static void sortArray(imgPosCell** a, unsigned int* listLengthA, unsigned int sizeA, imgPosCell** b, unsigned int* listLengthB, unsigned int sizeB, imgPosCell** temp, int* tempLength); /* sort 2 arrays to one*/
static void sortLists(imgPosCell** arr, unsigned int size); /* Sort list by Column and row */
static void sortSingleList(imgPosCell* list); /* Sort single list by column and row */
static Segment* findSingleSegmentWithFlag(grayImage* img, imgPos kernel, unsigned char threshold, unsigned char** flagMatrix); /* create segment with given flag matrix */
static imgPos* get_next_EmptyPosition_viaFlagMatrix(grayImage* img, unsigned char*** flagMatrix, BOOL* found);

/* ------Functions------ */

unsigned int findAllSegments(grayImage* img, unsigned char threshold, imgPosCell*** segments)
{
	unsigned char** flagMatrix = buildFlagMatrix(img->rows, img->cols);
	imgPosCell** segment_Array = NULL;
	imgPos* minPosition = NULL;
	unsigned int* listLength = NULL;
	Segment* seg;
	BOOL flag = getMinInImage(img, &flagMatrix, &minPosition);
	unsigned int i = 0; /* Array index */
	unsigned int size = 0; /* Array Physical size */
	while (flag)
	{
		seg = findSingleSegmentWithFlag(img, *minPosition, threshold, flagMatrix);
		if (i == size) /* Make array bigger and update listLength array */
		{
			size = size * 2 + 1;
			segment_Array = (imgPosCell**)realloc(segment_Array, sizeof(imgPosCell*) * size);
			checkalloc(segment_Array);
			listLength = (unsigned int*)realloc(listLength, sizeof(unsigned int) * size);
			checkalloc(listLength);
		}
		listLength[i] = seg->size;
		segment_Array[i] = buildSingleSegmentList(seg->root);
		i++;
		free(minPosition); minPosition = NULL;/* free last minimum, and reset to NULL */
		flag = getMinInImage(img, &flagMatrix, &minPosition); /* create new minimum */
		freeSegment(seg);
	}
	free(minPosition);
	segment_Array = realloc(segment_Array, sizeof(imgPosCell*) * (i + 1)); /* Shrink segment Array to min space needed */
	checkalloc(segment_Array);
	mergeSortArray(segment_Array, listLength, i);
	free(listLength);
	sortLists(segment_Array, i);
	*segments = segment_Array;
	free2DArray(flagMatrix, img->rows);
	return i;
}
static imgPosCell* buildSingleSegmentList(treeNode* root) /* Create segment list and return it */
{
	if (root == NULL)
		return NULL;
	imgPosCell* cell = (imgPosCell*)malloc(sizeof(imgPosCell));
	checkalloc(cell);
	cell->prev = cell->next = NULL;
	cell->position[0] = root->position[0];
	cell->position[1] = root->position[1];
	imgPosCell* head = cell;
	unsigned int i;
	for (i = 0; root->similar_neighbors[i] != NULL; i++)
	{
		cell->next = buildSingleSegmentList(root->similar_neighbors[i]);
		if (cell->next)
		{
			cell->next->prev = cell;
			while (cell->next) /* Run to the last node in the list */
				cell = cell->next;
		}

	}
	return head;
}
static imgPos* get_next_EmptyPosition_viaFlagMatrix(grayImage* img, unsigned char*** flagMatrix, BOOL* found)
{
	imgPos* pos = (imgPos*)malloc(sizeof(imgPos));
	checkalloc(pos);
	unsigned int i, j;
	for (i = 0; i < img->rows; i++) {
		for (j = 0; j < img->cols; j++) {
			if ((*flagMatrix)[i][j] == EMPTY) { /* search the FlagMatrix for an empty Position */
				(*pos)[0] = i; (*pos)[1] = j;
				(*flagMatrix)[i][j] = FULL;
				*found = TRUE;
				return pos;
			}
		}
	}
	/* if we didn't find an empty position available: */
	free(pos);
	*found = FALSE;
	return NULL;
}
static BOOL getMinInImage(grayImage* img, unsigned char*** flagMatrix, imgPos** minPos) /*put min position in minPos return FALSE if not found else TRUE */
{
	unsigned int i, j;
	BOOL found;
	int min;
	/* get the first empty position available */
	*minPos = get_next_EmptyPosition_viaFlagMatrix(img, flagMatrix, &found);
	if (*minPos == NULL) {
		return found;
	}
	min = img->pixels[(**minPos)[0]][(**minPos)[1]]; /* initialize min */
	/*begin search:*/
	for (i = 0; i < img->rows; i++)
	{
		for (j = 0; j < img->cols; j++)
		{
			if (img->pixels[i][j] < min && (*flagMatrix)[i][j] == EMPTY) /* If found Min value */
			{
				(*flagMatrix)[(**minPos)[0]][(**minPos)[1]] = EMPTY; /* empty the last Min position */
				min = img->pixels[i][j];							/* replace with filling the new Min position */
				(**minPos)[0] = i;
				(**minPos)[1] = j;
				(*flagMatrix)[i][j] = FULL;
				found = TRUE;
			}
		}
	}
	return found; /* Return True if found Min */
}
static void mergeSortArray(imgPosCell** arr, unsigned int* listLength, unsigned int size) /* Merge sort array from biggest list to smallest */
{
	if (size < 2)
		return;
	int mid = size / 2;
	unsigned int i;
	imgPosCell** temp = (imgPosCell**)malloc(sizeof(imgPosCell*) * size);
	checkalloc(temp);
	int* tempLength = (int*)malloc(sizeof(int) * size);
	checkalloc(tempLength);
	mergeSortArray(arr, listLength, mid);
	mergeSortArray(arr + mid, listLength + mid, size - mid);
	sortArray(arr, listLength, mid, arr + mid, listLength + mid, size - mid, temp, tempLength);
	for (i = 0; i < size; i++) /* Copy sorted array from temp to original array */
	{
		arr[i] = temp[i];
		listLength[i] = tempLength[i];
	}
	free(temp);
	free(tempLength);
}
static void sortArray(imgPosCell** a, unsigned int* listLengthA, unsigned int sizeA, imgPosCell** b, unsigned int* listLengthB, unsigned int sizeB, imgPosCell** temp, int* tempLength) /* sort 2 arrays to one*/
{
	unsigned int i = 0, j = 0; /* i for index a j for index b */
	while (i < sizeA && j < sizeB)
	{
		if (listLengthA[i] > listLengthB[j])
		{
			tempLength[i + j] = listLengthA[i];
			temp[i + j] = a[i];
			i++;
		}
		else
		{
			tempLength[i + j] = listLengthB[j];
			temp[i + j] = b[j];
			j++;
		}
	}
	/* Copy any remains cells to temp array */
	while (i < sizeA)
	{
		tempLength[i + j] = listLengthA[i];
		temp[i + j] = a[i];
		i++;
	}

	while (j < sizeB)
	{
		tempLength[i + j] = listLengthB[j];
		temp[i + j] = b[j];
		j++;
	}

}
static void sortLists(imgPosCell** arr, unsigned int size) /* Sort array of lists by Column and row */
{
	unsigned int i;
	for (i = 0; i < size; i++)
	{
		sortSingleList(arr[i]);
	}
}
static void sortSingleList(imgPosCell* list) /* Sort single list by column and row */
{
	imgPosCell* i, * j, * head;
	head = list;
	imgPos temp;
	for (i = list; i != NULL; i = i->next)
	{
		for (j = head; j->next != NULL; j = j->next)
		{
			if (j->position[1] == j->next->position[1]) /* if Same position check row */
			{
				if (j->position[0] > j->next->position[0])
				{
					temp[0] = j->position[0];
					j->position[0] = j->next->position[0];
					j->next->position[0] = temp[0];

				}
			}
			else if (j->position[1] > j->next->position[1])
			{
				temp[0] = j->position[0]; temp[1] = j->position[1];
				j->position[0] = j->next->position[0];
				j->position[1] = j->next->position[1];
				j->next->position[0] = temp[0];
				j->next->position[1] = temp[1];
			}
		}
	}

}
static Segment* findSingleSegmentWithFlag(grayImage* img, imgPos kernel, unsigned char threshold, unsigned char** flagMatrix) /* create segment with given flag matrix */
{
	Segment* seg = (Segment*)malloc(sizeof(Segment));
	checkalloc(seg);
	seg->size = 1;
	flagMatrix[kernel[0]][kernel[1]] = FULL;
	seg->root = buildTreeNode(kernel);
	findSingleSegmentRec(img, flagMatrix, seg->root, threshold, &seg->size, kernel);
	return seg;
}

