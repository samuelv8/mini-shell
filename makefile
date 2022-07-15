all: main.c
	gcc main.c -lreadline -fsanitize=address -fsanitize=undefined \
    -fno-sanitize-recover=all -fsanitize=float-divide-by-zero     \
    -fsanitize=float-cast-overflow -fno-sanitize=null             \
    -fno-sanitize=alignment -o main