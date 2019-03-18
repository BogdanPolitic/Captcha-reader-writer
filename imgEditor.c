#pragma pack(1)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bmp_header.h"
#include "stack.h"
#include "position.h"

void outputImageName(char** name, char* initialName, int taskNo) {

	const char NAME_MAX_LEN = 40;
	char prefix[NAME_MAX_LEN];
	char* suffix = (char*)malloc(5 * sizeof(char));
	char* no = (char*)malloc(5*sizeof(char));
	int i, j;

	for (i = 0; initialName[i] != '.'; i++) {
		prefix[i] = initialName[i];
	}
	prefix[i] = 0;

	switch (taskNo) {
		case 1:
		case 3:
		case 4:
			for (j = 0; initialName[i + j] != 0; j++) {
				suffix[j] = initialName[i + j];
			}
			suffix[j] = 0;
			break;
		case 2:
			suffix = ".txt";
			break;
		default:
			break;
	}
	
	snprintf(no, 2, "%d", taskNo);
	strcpy(*name, prefix);
	if (taskNo == 4) {
		strcat(*name, "_bonus");
	} else {
		strcat(*name, "_task");
		strcat(*name, no);
	}
	strcat(*name, suffix);

}

struct RGB readInputColors(char* secondLine) {

	struct RGB RGB;
	char* stringToInteger = (char*)malloc(5 * sizeof(char));

	stringToInteger = strtok (secondLine, " ");
	RGB.B = atoi(stringToInteger);
	stringToInteger = strtok (NULL, " ");
	RGB.G = atoi(stringToInteger);
	stringToInteger = strtok (NULL, " ");
	RGB.R = atoi(stringToInteger);

	return RGB;

}

int equalColors(struct RGB C1, struct RGB C2) {

	if (C1.B != C2.B) {
		return 0;
	}
	if (C1.G != C2.G) {
		return 0;
	}
	if (C1.R != C2.R) {
		return 0;
	}

	return 1;

}

struct bmp_fileheader readFileheader(FILE* inputImage) {

	struct bmp_fileheader Fileheader;
	unsigned char* character = (unsigned char*)malloc
		(sizeof(unsigned char));
	unsigned short* shortNumber = (unsigned short*)malloc
		(sizeof(unsigned short));
	unsigned int* integer = (unsigned int*)malloc(sizeof(unsigned int));

	fseek(inputImage, 0x0, SEEK_SET);
	fread(character, sizeof(unsigned char), 1, inputImage);
	Fileheader.fileMarker1 = *character;
	fread(character, sizeof(unsigned char), 1, inputImage);
	Fileheader.fileMarker2 = *character;
	fread(integer, sizeof(unsigned int), 1, inputImage);
	Fileheader.bfSize = *integer;
	fread(shortNumber, sizeof(unsigned short), 1, inputImage);
	Fileheader.unused1 = *shortNumber;
	fread(shortNumber, sizeof(unsigned short), 1, inputImage);
	Fileheader.unused2 = *shortNumber;
	fread(integer, sizeof(unsigned int), 1, inputImage);
	Fileheader.imageDataOffset = *integer;

	return Fileheader;

}

struct bmp_infoheader readInfoheader(FILE* inputImage) {

	struct bmp_infoheader Infoheader;
	unsigned char* character = (unsigned char*)malloc
		(sizeof(unsigned char));
	unsigned short* shortNumber = (unsigned short*)malloc
		(sizeof(unsigned short));
	unsigned int* USinteger = (unsigned int*)malloc(sizeof(unsigned int));
	signed int* Sinteger = (signed int*)malloc(sizeof(signed int));

	fseek(inputImage, 0xE, SEEK_SET);
	fread(USinteger, sizeof(unsigned int), 1, inputImage);
	Infoheader.biSize = *USinteger;
	fread(Sinteger, sizeof(signed int), 1, inputImage);
	Infoheader.width = *Sinteger;
	fread(Sinteger, sizeof(signed int), 1, inputImage);
	Infoheader.height = *Sinteger;
	fread(shortNumber, sizeof(unsigned short), 1, inputImage);
	Infoheader.planes = *shortNumber;
	fread(shortNumber, sizeof(unsigned short), 1, inputImage);
	Infoheader.bitPix = *shortNumber;
	fread(USinteger, sizeof(unsigned int), 1, inputImage);
	Infoheader.biCompression = *USinteger;
	fread(USinteger, sizeof(unsigned int), 1, inputImage);
	Infoheader.biSizeImage = *USinteger;
	fread(Sinteger, sizeof(signed int), 1, inputImage);
	Infoheader.biXPelsPerMeter = *Sinteger;
	fread(Sinteger, sizeof(signed int), 1, inputImage);
	Infoheader.biYPelsPerMeter = *Sinteger;
	fread(USinteger, sizeof(unsigned int), 1, inputImage);
	Infoheader.biClrUsed = *USinteger;
	fread(USinteger, sizeof(unsigned int), 1, inputImage);
	Infoheader.biClrImportant = *USinteger;

	return Infoheader;

}

struct RGB** makePixelMatrix(char* inputImageName, 
	struct bmp_infoheader Infoheader) {

	const char FILEHEADER_SIZE = 14;
	const char INFOHEADER_SIZE = 40;
	unsigned int line = 0, colon = 0;
	FILE* inputImage = fopen(inputImageName, "r");
	char* threeBytes = (char*)malloc(4 * sizeof(char));
	int i;

	struct RGB** pixelMatrix = (struct RGB**)malloc
		(Infoheader.height * sizeof(struct RGB*));
	for (i = 0; i < Infoheader.height; i++) {
		pixelMatrix[i] = (struct RGB*)malloc(Infoheader.width *
			sizeof(struct RGB));
	}

	fseek(inputImage, FILEHEADER_SIZE + INFOHEADER_SIZE, SEEK_SET);
	for (i = 0; i < Infoheader.biSizeImage / 3; i++) {
		fgets(threeBytes, 4, inputImage);
		if (colon == Infoheader.width) {
			line++;
			colon = 0;
		}
		pixelMatrix[Infoheader.height-line-1][colon].B = threeBytes[0];
		pixelMatrix[Infoheader.height-line-1][colon].G = threeBytes[1];
		pixelMatrix[Infoheader.height-line-1][colon].R = threeBytes[2];
		pixelMatrix[Infoheader.height-line-1][colon].visited = 0;
		colon++;
	}
	fclose(inputImage);

	return pixelMatrix;

}

int outOfBoundsException(int posX, int posY, int width, int height) {

	return !((posX >= 0 && posX < height) && (posY >= 0 && posY < width));

}

void floodFillOneDigit(struct RGB*** imageMatrix, struct RGB color,
	int posX, int posY, int width, int height) {

	struct RGB white;
	white.B = white.G = white.R = 255;

	if (outOfBoundsException(posY, posX, width, height)) {
		return;
	}

	if (!equalColors((*imageMatrix)[posY][posX], color)
	&& !equalColors((*imageMatrix)[posY][posX], white)) {
		(*imageMatrix)[posY][posX] = color;
		floodFillOneDigit(imageMatrix,color,posX,posY+1, width,height);
		floodFillOneDigit(imageMatrix,color,posX+1,posY, width,height);
		floodFillOneDigit(imageMatrix,color,posX,posY-1, width,height);
		floodFillOneDigit(imageMatrix,color,posX-1,posY, width,height);
	}

}

struct RGB makeColor(unsigned char R, unsigned char G, unsigned char B) {
	/* PARAM R FIRST, B LAST (!!!) */

	struct RGB color;

	color.B = B;
	color.G = G;
	color.R = R;

	return color;

}

unsigned char decodeDigit(unsigned char digitEncoding) {
					/* ENCODINGS */
	switch (digitEncoding) {
		case 26:		/* ZERO */
			return 0;
		case 11:		/* ONE */
			return 1;
		case 25:		/* TWO */
			return 2;
		case 27:		/* THREE */
			return 3;
		case 17:		/* FOUR */
			return 4;
		case 23:		/* FIVE */
			return 5;
		case 24:		/* SIX */
			return 6;
		case 19:		/* SEVEN */
			return 7;
		case 29:		/* EIGHT */
			return 8;
		case 28:		/* NINE */
			return 9;
		default:		/* GREATER(NOT POSSIBLE) */
			return 20;
	}

}

unsigned char continueDescending(int i, int j,
	int height, struct RGB** pixelMatrix) {

	struct RGB WHITE = makeColor(255, 255, 255);
	if (j >= height) {
		return 0;
	}

	if (j > 0) {
		if (!equalColors(pixelMatrix[j - 1][i], WHITE) &&
			!pixelMatrix[j - 1][i].visited) {
			return 0;
		}
	}

	return 1;

}

void searchForTopRight(struct RGB*** pixelMatrix, int i, int j, int* max_i,
int* min_j, int width, int height, unsigned char* digitPixels) {

	struct RGB WHITE = makeColor(255, 255, 255);
	Stack S = initStack();
	Pixel* P;

	addPixel(&S, i, j);
	while (!isEmpty(S)) {
		P = popPixel(&S);
		i = P->posX;
		j = P->posY;
		if (!outOfBoundsException(j, i, width, height)) {
			if (!(*pixelMatrix)[j][i].visited &&
				!equalColors((*pixelMatrix)[j][i], WHITE)) {
				if (*max_i < i) {
					*max_i = i;
				}
				if (*min_j > j) {
					*min_j = j;
				}
				(*pixelMatrix)[j][i].visited = 1;
				*digitPixels = *digitPixels + 1;
				addPixel(&S, i, j - 1);
				addPixel(&S, i + 1, j);
				addPixel(&S, i, j + 1);
				addPixel(&S, i - 1, j);
			}
		}
	}

}

unsigned char leftsidePixelCount(struct RGB** pixelMatrix, int i, int j) {

	unsigned char count = 0;
	struct RGB WHITE = makeColor(255, 255, 255);

	while (count < 5) {
		if (!equalColors(pixelMatrix[j][i - count], WHITE)) {
			count++;
		} else {
			return count;
		}
	}

	return count;

}

unsigned char downsidePixelCount(struct RGB** pixelMatrix, int i, int j) {

	int count = 0;
	struct RGB WHITE = makeColor(255, 255, 255);

	while (count < 5) {
		if (!equalColors(pixelMatrix[j + count][i], WHITE)) {
			count++;
		} else {
			return count;
		}
	}

	return count;

}

void outputToImage(char* outputImageName, char* inputImageName,
	struct RGB** pixelMatrix, int width, int height) {

	const char FILEHEADER_SIZE = 14;
	const char INFOHEADER_SIZE = 40;
	char* strFileHeader = (char*)malloc(FILEHEADER_SIZE * sizeof(char));
	char* strInfoHeader = (char*)malloc(INFOHEADER_SIZE * sizeof(char));
	FILE* inputImage = fopen(inputImageName, "r");
	FILE* outputImage = fopen(outputImageName, "w");
	int i, j;

	fseek(inputImage, 0, SEEK_SET);
	fread(strFileHeader, sizeof(char), FILEHEADER_SIZE, inputImage);
	fread(strInfoHeader, sizeof(char), INFOHEADER_SIZE, inputImage);

	fseek(outputImage, 0, SEEK_SET);
	for (i = 0; i < FILEHEADER_SIZE; i++) {
		fprintf(outputImage, "%c", strFileHeader[i]);
	}
	for (i = 0; i < INFOHEADER_SIZE; i++) {
		fprintf(outputImage, "%c", strInfoHeader[i]);
	}

	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			fprintf(outputImage,"%c",pixelMatrix[height-i-1][j].B);
			fprintf(outputImage,"%c",pixelMatrix[height-i-1][j].G);
			fprintf(outputImage,"%c",pixelMatrix[height-i-1][j].R);
		}
	}

	fclose(inputImage);
	fclose(outputImage);

}

void outputToFile(char* digitsNumber, char* task2TextName) {

	FILE* outputNumber = fopen(task2TextName, "w");

	fprintf(outputNumber, "%s", digitsNumber);

	fclose(outputNumber);

}

void main() {

	const char NAME_MAX_LEN = 40;
	const int MAX_DIGITS_NO = 1024;
	char* inputImageName = (char*)malloc(NAME_MAX_LEN * sizeof(char));
	char* task1ImageName = (char*)malloc(NAME_MAX_LEN * sizeof(char));
	char* task2TextName = (char*)malloc(NAME_MAX_LEN * sizeof(char));
	char* task3ImageName = (char*)malloc(NAME_MAX_LEN * sizeof(char));
	char* bonusImageName = (char*)malloc(NAME_MAX_LEN * sizeof(char));
	char* firstLine = (char*)malloc(NAME_MAX_LEN * sizeof(char));
	char* secondLine = (char*)malloc(NAME_MAX_LEN * sizeof(char));
	char* fourthLine = (char*)malloc(NAME_MAX_LEN * sizeof(char));
	char* digitsNumber = (char*)malloc(NAME_MAX_LEN * sizeof(char));
	char* chr = (char*)malloc(sizeof(char));
	struct RGB** pixelMatrix;
	struct RGB WHITE;
	int i, j;
	unsigned char digitPixels; /* <--- PENTRU TASK 2 */
	int max_i, min_j; /* <--- PENTRU TASK 2 */
	int captchaCode; /* <--- PENTRU TASK 2 */
	unsigned char digitEncoding; /* <--- PENTRU TASK 2 */
	int digitCount; /* <---- PENTRU TASK 2 */
	struct Position* captchaPositions = (struct Position*)malloc
		(MAX_DIGITS_NO * sizeof(struct Position));
	unsigned int arrayLen = 0;

	FILE* inputFile = fopen("input.txt", "r");
	FILE* inputImage;

	fgets(inputImageName, NAME_MAX_LEN, inputFile);
	inputImageName[strlen(inputImageName) - 1] = 0;
	inputImage = fopen(inputImageName, "r");

	outputImageName(&task1ImageName, inputImageName, 1);
	printf("%s\n", secondLine);

	fseek(inputImage, 0, SEEK_SET);

	struct bmp_fileheader FileHeader = readFileheader(inputImage);
	struct bmp_infoheader InfoHeader = readInfoheader(inputImage);
	fclose(inputImage);

/* START OF TASK 2 */

	outputImageName(&task2TextName, inputImageName, 2);

	captchaCode = 0;
	WHITE = makeColor(255, 255, 255);

	digitCount = 0;
	pixelMatrix = makePixelMatrix(inputImageName, InfoHeader);

	for (i = 0; i < InfoHeader.width; i++) {
		for (j = 0; continueDescending(i, j, InfoHeader.height,
			pixelMatrix); j++) {
			continue;
		}
		if (j < InfoHeader.height) {
			digitCount++;
			j--;
			digitPixels = 0;
			max_i = i;
			min_j = j;
			searchForTopRight(&pixelMatrix, i, j, 
				&max_i, &min_j, InfoHeader.width, 
				InfoHeader.height, &digitPixels);
		/* FOR TASK 1 */
			addToArray(&captchaPositions, max_i, min_j, &arrayLen);
		/* */
			digitEncoding = digitPixels +
				leftsidePixelCount(pixelMatrix, max_i, min_j) +
				downsidePixelCount(pixelMatrix, max_i, min_j);
			*chr = decodeDigit(digitEncoding) + 48;
			strcpy(digitsNumber + digitCount - 1, chr);
		}
	}

	outputToFile(digitsNumber, task2TextName);

// END OF TASK 2

// CREATE FILES FOR TASK 3 AND TASK 4

	outputImageName(&task3ImageName, inputImageName, 3);
	outputToImage(task3ImageName, inputImageName,
		pixelMatrix, InfoHeader.width, InfoHeader.height);

	fgets(secondLine, NAME_MAX_LEN, inputFile);
	secondLine[strlen(secondLine) - 1] = 0;
	fgets(fourthLine, NAME_MAX_LEN, inputFile);
	fourthLine[strlen(fourthLine) - 1] = 0;

	outputImageName(&bonusImageName, inputImageName, 4);
	outputToImage(bonusImageName, inputImageName, pixelMatrix,
		InfoHeader.width, InfoHeader.height);

//END

// START OF TASK 1

	pixelMatrix = makePixelMatrix(inputImageName, InfoHeader);

	struct RGB RGB = readInputColors(secondLine);

	for (i = 0; i < arrayLen; i++) {
		floodFillOneDigit(&pixelMatrix, RGB, captchaPositions[i].posX,
			captchaPositions[i].posY, InfoHeader.width,
			InfoHeader.height);
	}	

	outputToImage(task1ImageName, inputImageName, pixelMatrix,
		InfoHeader.width, InfoHeader.height);

// END OF TASK 1

	fclose(inputFile);

}

#pragma pack()
