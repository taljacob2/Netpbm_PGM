#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "grayImage.h"
#include "utils.h"
#include "prints.h"
#include "main.h"

/* -------Static Declerations------ */

static void colorSegmentFromList(grayImage* img, imgPosCell* lst, int value); /* update image value on list position */
static unsigned char** buildMatrixFromFile(unsigned short rows, unsigned short cols, FILE* readFile); /* Create Matrix from pgm file */
static void compressImage(grayImage* img, unsigned char divider); /* compress each pixel in image to new pixel */
static void convertCompressedImageToPGM_Aux(FILE* fp_bin, FILE* fp_PGM);
static void DE_compressImage(grayImage* image, unsigned char divider);
static void export_to_PGM_File(grayImage* image, FILE* fp_PGM);



/* ------Functions------ */

grayImage buildgrayImage()
{
	unsigned short rows, cols;
	printf("Enter rows: ");
	scanf("%hu", &rows);
	printf("Enter cols:");
	scanf("%hu", &cols);
	grayImage matrix;
	matrix.rows = rows;
	matrix.cols = cols;
	matrix.pixels = (unsigned char**)malloc(sizeof(unsigned char*) * rows);
	checkalloc(matrix.pixels);
	int i;
	for (i = 0; i < rows; i++)
	{
		matrix.pixels[i] = (unsigned char*)malloc(sizeof(unsigned char) * cols);
		checkalloc(matrix.pixels[i]);
	}
	return matrix;
}
void fillmatrix(grayImage matrix)
{
	int i, j;
	unsigned char** mat = matrix.pixels;
	for (i = 0; i < matrix.rows; i++)
	{
		for (j = 0; j < matrix.cols; j++)
		{
			mat[i][j] = rand() % 255;
		}
	}
}
Segment* findSingleSegment(grayImage* img, imgPos kernel, unsigned char threshold)
{
	Segment* seg = (Segment*)malloc(sizeof(Segment));
	checkalloc(seg);
	seg->size = 1;
	unsigned char** flagMatrix = buildFlagMatrix(img->rows, img->cols);
	flagMatrix[kernel[0]][kernel[1]] = FULL;
	seg->root = buildTreeNode(kernel);
	findSingleSegmentRec(img, flagMatrix, seg->root, threshold, &seg->size, kernel);
	free2DArray(flagMatrix, img->rows);

	return seg;
}
void findSingleSegmentRec(grayImage* img, unsigned char** flagMatrix, treeNode* current_root, unsigned char threshold, unsigned int* size, imgPos kernel) /* Rec function to create Neighbors array */
{
	unsigned int i, current_sn_size = 0; /* sn array starts empty */
	createSimilarNeighbors(img, flagMatrix, current_root, threshold, size, &current_sn_size, kernel); /* create a similar neighbors array for the first circle */
	if (current_root->similar_neighbors) /* as long as there is a sn array, search for sn arrays of each member */
		for (i = 0; i < current_sn_size; i++) /* check the circle of every neighbor inside this array */
			findSingleSegmentRec(img, flagMatrix, current_root->similar_neighbors[i], threshold, size, kernel);

}
void createSimilarNeighbors(grayImage* img, unsigned char** flagMatrix, treeNode* current_root, unsigned char threshold, unsigned int* size, unsigned int* current_sn_size, imgPos kernel) /* Create similar neighbors 2d array */
{
	int i, j; /* must not be unsigned! */
	imgPos pos = { current_root->position[0], current_root->position[1] };
	imgPos newpos = { current_root->position[0], current_root->position[1] };

	for (i = -1; i < 2; i++) { /* Search 8 neighbors circle */
		for (j = -1; j < 2; j++) {

			if ((int)(pos[0] + i) >= 0 && ((int)(pos[0] + i) < img->rows) && ((int)(pos[1] + j) >= 0) && ((int)(pos[1] + j) < img->cols) && (i != 0 || j != 0)) /* if neighbor is valid position in matrix */
			{
				if (abs(img->pixels[pos[0] + i][pos[1] + j] - img->pixels[kernel[0]][kernel[1]]) <= threshold) /* Check threshold */
				{
					if (flagMatrix[pos[0] + i][pos[1] + j] == EMPTY) /* if not already in segment */
					{ /* Add position to similar Neighbors array */
						flagMatrix[pos[0] + i][pos[1] + j] = FULL;
						(*size)++;
						(*current_sn_size)++;
						current_root->similar_neighbors = (treeNode**)realloc(current_root->similar_neighbors, sizeof(treeNode*) * (*current_sn_size));
						checkalloc(current_root->similar_neighbors);

						newpos[0] = pos[0] + i; newpos[1] = pos[1] + j;
						current_root->similar_neighbors[(*current_sn_size) - 1] = buildTreeNode(newpos); /* create a newTree and insert it into this sn array */
					}
				}
			}
		}
	}
	/* question says we need to add an additional treeNode with NULL */
	current_root->similar_neighbors = (treeNode**)realloc(current_root->similar_neighbors, sizeof(treeNode*) * ((*current_sn_size) + 1));
	checkalloc(current_root->similar_neighbors);
	current_root->similar_neighbors[(*current_sn_size)] = NULL; /* insert NULL in the last treeNode location */
}
treeNode* buildTreeNode(imgPos pos) /* Build Tree Node and return it */
{
	treeNode* root = (treeNode*)malloc(sizeof(treeNode));
	checkalloc(root);
	root->position[0] = pos[0];
	root->position[1] = pos[1];
	root->similar_neighbors = NULL;
	return root;
}
unsigned char** buildFlagMatrix(unsigned short rows, unsigned short cols) /* Build flag matrix with required rows and cols and init with 0*/
{
	unsigned char** mat = (unsigned char**)malloc(sizeof(unsigned char*) * rows);
	checkalloc(mat);
	unsigned int i;
	for (i = 0; i < rows; i++)
	{
		mat[i] = (unsigned char*)calloc(cols, sizeof(unsigned char));
		checkalloc(mat[i]);
	}
	return mat;
}
grayImage* colorSegments(grayImage* img, imgPosCell** segments, unsigned int size) /* change each segment in array to one value */
{
	grayImage* newImage = (grayImage*)malloc(sizeof(grayImage)); /* create new image */
	checkalloc(newImage);
	newImage->pixels = (unsigned char**)malloc(sizeof(unsigned char*) * img->rows);
	checkalloc(newImage->pixels);
	newImage->cols = img->cols;
	newImage->rows = img->rows;
	unsigned int i, j;
	for (i = 0; i < img->rows; i++)
	{
		newImage->pixels[i] = (unsigned char*)malloc(sizeof(unsigned char) * img->cols);
		checkalloc(newImage->pixels[i]);
	}
	/* empty image was created, now begin: */
	if (size == 1) /* Return 0 image */
	{
		for (i = 0; i < img->rows; i++)
		{
			for (j = 0; j < img->cols; j++)
				newImage->pixels[i][j] = 0;
		}
		return newImage;
	}
	for (i = 0; i < size; i++) /* Create new image from segments array */
	{
		colorSegmentFromList(newImage, segments[i], floor(i * (255 / (size - 1)))); /* Update pixels base on the list position */
	}
	return newImage;
}
static void colorSegmentFromList(grayImage* img, imgPosCell* lst, int value) /* update image value on list position */
{
	while (lst) /* change all list position in image to value */
	{
		img->pixels[lst->position[0]][lst->position[1]] = value;
		lst = lst->next;
	}
}
grayImage* readPGM(char* fname) /* create grayImage struct from file */
{
	FILE* readFile = fopen(fname, "r");
	check_TypeOfFile(fname, ".pgm"); /* check the file type */
	check_fopen_custom(readFile, "Unable to open this '.pgm' file...\n");
	grayImage* img = (grayImage*)malloc(sizeof(grayImage));
	checkalloc(img);
	char* line = my_getline(readFile);
	if ((strcmp(line, "P2")) != 0) /* Check image format is p2 */
	{
		fprintf(stderr, "Support only P2 file\n");
		exit(ERROR);
	}
	unsigned short rows, cols;
	free(line);
	line = my_getline(readFile);
	while (line[0] == '#') /* Skip comments */
	{
		free(line);
		line = my_getline(readFile);
	}
	sscanf(line, " %hu %hu", &rows, &cols); /* Scan width and cols */
	img->cols = cols;
	img->rows = rows;
	free(line);
	line = my_getline(readFile);
	while (line[0] == '#') /* Loop until we get value of matrix line */
	{
		free(line);
		line = my_getline(readFile);
	}
	if (strcmp(line, "255") != 0) /* Check value is 255 ASCII */
	{
		fprintf(stderr, "Value is not 255\n");
		exit(ERROR);
	}
	free(line);
	img->pixels = buildMatrixFromFile(img->rows, img->cols, readFile);
	fclose(readFile);
	return img;
}
static unsigned char** buildMatrixFromFile(unsigned short rows, unsigned short cols, FILE* readFile) /* Create Matrix from pgm file */
{
	unsigned char** matrix = (unsigned char**)malloc(sizeof(unsigned char*) * rows);
	checkalloc(matrix);
	unsigned short i, j;
	for (i = 0; i < rows; i++)
	{
		matrix[i] = (unsigned char*)malloc(sizeof(unsigned char) * cols);
		checkalloc(matrix[i]);
	}
	/* we created the matrix, now begin to read: */
	for (i = 0; i < rows; i++)
	{
		for (j = 0; j < cols; j++)
		{
			fscanf(readFile, "%hhu", &(matrix[i][j]));
		}
	}
	return matrix;
}
void saveCompressed(char* file_name, grayImage* image, unsigned char reduced_gray_levels) /* compress file and save in bin file */
{
	check_TypeOfFile(file_name, ".bin");
	FILE* file = fopen(file_name, "wb");
	check_fopen_custom(file, "Unable to open file to write compressed file\n");
	fwrite(&(image->rows), sizeof(unsigned short), 1, file);
	fwrite(&(image->cols), sizeof(unsigned short), 1, file);
	fwrite(&reduced_gray_levels, sizeof(unsigned char), 1, file);
	unsigned int i, j, p = 0; /* p index for comp file array.*/
	unsigned char divider = 256 / reduced_gray_levels; /* divider to get new pixel value */
	unsigned int numBits = (unsigned int)(log(reduced_gray_levels) / log(2)); /* how many bits of new pixels */
	unsigned int bytesAmount = (unsigned int)ceil((((double)numBits * (double)(image->cols) * (double)(image->rows)) / 8.0)); /* Calculate needed bytes for compress file */
	unsigned char* compFile = (unsigned char*)malloc(sizeof(unsigned char) * bytesAmount);
	checkalloc(compFile);
	compFile[bytesAmount - 1] = 0; /* zero the last byte */
	compFile[0] = 0; /* for Big endian or Little endian init first byte with zero */
	unsigned char bitsLeft = 0; /* number of active bits */
	unsigned char LS, MS;
	compressImage(image, divider);
	for (i = 0; i < image->rows; i++)
	{
		for (j = 0; j < image->cols; j++)
		{
			if (bitsLeft <= 8 - numBits)
			{
				compFile[p] <<= numBits;
				compFile[p] |= (image->pixels[i][j]);
				bitsLeft += numBits;
				if (bitsLeft == 8)
				{
					bitsLeft = 0;
					if (p + 1 < bytesAmount) { /* only if there is a next available place */
						compFile[p + 1] = 0;
						p++;
					}
				}
			}
			else
			{
				compFile[p] <<= 8 - bitsLeft;
				MS = (image->pixels[i][j]) >> (bitsLeft + numBits - 8);
				compFile[p] += MS;
				LS = (image->pixels[i][j]) << (16 - bitsLeft - numBits);
				LS >>= (16 - bitsLeft - numBits);
				bitsLeft = bitsLeft + numBits - 8;
				if (p + 1 < bytesAmount) { /* only if there is a next available place */
					compFile[p + 1] = LS;
					p++;
				}
			}
		}
	}
	fwrite(compFile, sizeof(unsigned char), bytesAmount, file);
	free(compFile);
	fclose(file);
}
static void compressImage(grayImage* img, unsigned char divider) /* compress each pixel in image to new pixel */
{
	unsigned int i, j;
	for (i = 0; i < img->rows; i++)
	{
		for (j = 0; j < img->cols; j++)
		{
			img->pixels[i][j] /= divider;
		}
	}
}
void convertCompressedImageToPGM(char* compressed_file_name, char* pgm_file_name)
{
	check_TypeOfFile(compressed_file_name, ".bin");
	FILE* fp_bin = fopen(compressed_file_name, "rb");
	check_fopen_custom(fp_bin, "Unable to open this '.bin' file...\n");
	FILE* fp_PGM = fopen(pgm_file_name, "w");
	check_fopen_custom(fp_PGM, "Unable to open file to write PGM file\n");

	convertCompressedImageToPGM_Aux(fp_bin, fp_PGM);

	fclose(fp_bin); fclose(fp_PGM);
}
static void convertCompressedImageToPGM_Aux(FILE* fp_bin, FILE* fp_PGM)
{
	/* get the rows and cols from the '.bin' file, and create and empty img: */
	unsigned char reduced_gray_levels;
	grayImage* image = (grayImage*)malloc(sizeof(grayImage));
	checkalloc(image);
	fread(&(image->rows), sizeof(unsigned short), 1, fp_bin);
	fread(&(image->cols), sizeof(unsigned short), 1, fp_bin);
	fread(&reduced_gray_levels, sizeof(unsigned char), 1, fp_bin);
	image->pixels = buildFlagMatrix(image->rows, image->cols);
	/* successfully exported an empty img */

	unsigned int i, j;
	unsigned char divider = 256 / reduced_gray_levels; /* divider to get new pixel value */
	unsigned int numBits = (unsigned int)(log(reduced_gray_levels) / log(2)); /* how many bits of new pixels */
	unsigned char bitsPosition = 8; /* this is the position of the current bits active */
	unsigned char LS = 0, MS = 0, curr = 0, unsignedCharRead = 0;

	fread(&unsignedCharRead, sizeof(unsigned char), 1, fp_bin);
	for (i = 0; i < image->rows; i++) {
		for (j = 0; j < image->cols;) {
			if (bitsPosition >= numBits) {
				curr = unsignedCharRead << (8 - bitsPosition);
				curr = (curr >> (8 - numBits)); /* constant */
				bitsPosition = bitsPosition - numBits;
				image->pixels[i][j] = curr;
				j++;
			}
			if (bitsPosition < numBits) { /* dealing with the leftover: */
				MS = LS = 0;
				MS = unsignedCharRead << (8 - bitsPosition);
				MS >>= (8 - numBits); /* constant */
				if (j >= image->cols) { /* continue only if we didn't finish to fill the row*/
					break; /* we finished to fill the row. start another row */
				}
				fread(&unsignedCharRead, sizeof(unsigned char), 1, fp_bin);
				LS = unsignedCharRead >> (bitsPosition = (8 - (numBits - bitsPosition)));
				curr = MS | LS;
				image->pixels[i][j] = curr;
				j++;
			}
		}
	}
	/* 'image->pixels' has been exported successfully - and now it needs to be de-compressed */
	DE_compressImage(image, divider);
	/* now we export to file: */
	export_to_PGM_File(image, fp_PGM);
	free_grayImage(image);
}
static void export_to_PGM_File(grayImage* image, FILE* fp_PGM)
{
	fprintf(fp_PGM, "P2\n");
	fprintf(fp_PGM, "# this is the exported PGM file.\n");
	fprintf(fp_PGM, "%hu %hu\n", image->rows, image->cols);
	fprintf(fp_PGM, "255\n");
	fprintmatrix(fp_PGM, *image);
}
static void DE_compressImage(grayImage* image, unsigned char divider)/* de-compress each pixel in image to new pixel */
{
	unsigned int i, j;
	for (i = 0; i < image->rows; i++)
	{
		for (j = 0; j < image->cols; j++)
		{
			image->pixels[i][j] *= divider;
		}
	}
}

