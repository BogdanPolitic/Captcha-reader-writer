#pragma pack(1)

struct Position {
	int posX;
	int posY;
};

void addToArray(struct Position** array, int posX, int posY, unsigned int* Len) {

	(*array)[*Len].posX = posX;
	(*array)[*Len].posY = posY;

	*Len = *Len + 1;

}

#pragma pack()
