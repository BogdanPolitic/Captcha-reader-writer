build:
	gcc imgEditor.c -o imgEditor -lm

run:
	./imgEditor

clean:
	rm -rf imgEditor *.o *_task2.txt
