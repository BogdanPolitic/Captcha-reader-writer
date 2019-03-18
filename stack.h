#pragma pack(1)
#include <stdlib.h>

typedef struct P {
	int posX;
	int posY;
	struct P* next;
	struct P* prev;
} Pixel;

typedef struct S {
	Pixel* frame;
	Pixel* head;
} Stack;

Pixel* initPixel(int posX, int posY) {

	Pixel* P;
	P->posX = posX;
	P->posY = posY;
	return P;

}

Stack initStack() {

	Stack S;
	S.head = S.frame = NULL;
	return S;

}

unsigned char isEmpty(Stack S) {

	return S.head == NULL;

}

void push(Stack** S, Pixel* P) {

	if ((*S)->head == NULL) {
		(*S)->head = (*S)->frame = P;
		return;
	}
	(*S)->head->next = P;
	(*S)->head->next->prev = (*S)->head;
	(*S)->head = (*S)->head->next;

}

Pixel* pop(Stack** S) {

	Pixel* aux = (*S)->head;
	if ((*S)->head == NULL) {
		return NULL;
	}
	if ((*S)->head == (*S)->frame) {
		(*S)->head = (*S)->frame = NULL;
		return aux;
	}
	(*S)->head = (*S)->head->prev;
	return aux;

}

void addPixel(Stack* S, int posX, int posY) {

	Pixel* P = (Pixel*)malloc(sizeof(Pixel));
	P->posX = posX;
	P->posY = posY;
	push(&S, P);

}

Pixel* popPixel(Stack* S) {

	return pop(&S);

}

#pragma pack()
