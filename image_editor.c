//Copyright Oana-Alexandra Titoc 313CAb 2022-2023

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#define MAX 1000

//structura in care retinem linii, coloane, magic wordul si
//valoarea maxima pe care o poate lua o culoare
typedef struct {
	int width;
	int height;
	int magic_w;
	int max;
	int x1;
	int y1;
	int x2;
	int y2;
} image;

void alloc_matrix(int ***matrix, image a)
{
	//aloc numarul de linii a matricei
	(*matrix) = (int **)malloc(a.height * sizeof(int *));
	if (!*matrix) {
		printf("alocare esuata1");
		exit(1);
	}

	//aloc numarul de coloane in functie de magic word
	//pentru imagine grayscale: coloane = latimea
	//pentru imagine color: coloane = 3 * latimea
	if (a.magic_w == 2 || a.magic_w == 5) {
		for (int i = 0; i < a.height; i++) {
			(*matrix)[i] = (int *)malloc(a.width * sizeof(int));
			if (!(*matrix)[i]) {
				printf("alocare esuata2");
				exit(1);
			}
		}
	} else {
		for (int i = 0; i < a.height; i++) {
			(*matrix)[i] = (int *)malloc(3 * a.width * sizeof(int));
			if (!(*matrix)[i]) {
				printf("alocare esuata3");
				exit(1);
			}
		}
	}
}

//la fel ca alloc_matrix- inversez liniile si coloanele
//folosita pentru functia rotate
void alloc_matrix_tr(int ***matrix, image a)
{
	(*matrix) = (int **)malloc(a.width * sizeof(int *));
	if (!*matrix) {
		printf("alocare esuata1");
		exit(1);
	}

	if (a.magic_w == 2 || a.magic_w == 5) {
		for (int i = 0; i < a.width; i++) {
			(*matrix)[i] = (int *)malloc(a.height * sizeof(int));
			if (!(*matrix)[i]) {
				printf("alocare esuata2");
				exit(1);
			}
		}
	} else {
		for (int i = 0; i < a.width; i++) {
			(*matrix)[i] = (int *)malloc(3 * a.height * sizeof(int));
			if (!(*matrix)[i]) {
				printf("alocare esuata3");
				exit(1);
			}
		}
	}
}

//eliberarea memoriei matricei
void free_matrix(int **matrix, int rows)
{
	for (int i = 0; i < rows; i++)
		free(matrix[i]);
	free(matrix);
}

//ignor toate liniile din fisier care incep cu caracterul #
void ignore_comments(FILE *in)
{
	char comment, aux[MAX];
	fscanf(in, "%c", &comment);
	if (comment == '#')
		fgets(aux, MAX, in);
	else
		fseek(in, -1, SEEK_CUR);
}

void read_image(int **matrix, image a, FILE *in)
{
	//citesc imagine plain PGM(text)
	if (a.magic_w == 2) {
		for (int i = 0; i < a.height; i++)
			for (int j = 0; j < a.width; j++)
				fscanf(in, "%d", &matrix[i][j]);
	}

	//citesc imagine plain PPM(text)
	if (a.magic_w == 3) {
		for (int i = 0; i < a.height; i++)
			for (int j = 0; j < 3 * a.width; j++)
				fscanf(in, "%d", &matrix[i][j]);
	}

	//citesc imagine PGM(binary)
	if (a.magic_w == 5) {
		//citesc caracterul \n de la finalul numarului maxim(255)
		fseek(in, 1, SEEK_CUR);

		unsigned char ch;
		for (int i = 0; i < a.height; i++)
			for (int j = 0; j < a.width; j++) {
				fread(&ch, sizeof(unsigned char), 1, in);
				matrix[i][j] = (int)ch;
			}
	}

	//citesc imagine PPM(binary)
	if (a.magic_w == 6) {
		fseek(in, 1, SEEK_CUR);
		unsigned char ch;
		for (int i = 0; i < a.height; i++)
			for (int j = 0; j < 3 * a.width; j++) {
				fread(&ch, sizeof(unsigned char), 1, in);
				matrix[i][j] = (int)ch;
			}
	}
}

void load_image(int ***matrix, image *a, int *loaded)
{
	//deschid fisierul
	char file[100];
	scanf("%s", file);
	FILE *in = fopen(file, "rb");
	if (!in) {
		printf("Failed to load %s\n", file);
		if (*loaded == 1) {
			free_matrix(*matrix, a->height);
			*loaded = 0;
		}
		return;
	}

	//eliberez memoria matricii din vechiul fisier daca acesta a existat
	if (*loaded)
		free_matrix(*matrix, a->height);

	//aflu tipul imaginii
	char magic_word[3];
	fscanf(in, "%s", magic_word);
	a->magic_w = magic_word[1] - '0';

	ignore_comments(in);

	//aflu dimensiunile imaginii
	fscanf(in, "%d%d", &a->width, &a->height);

	ignore_comments(in);

	//aflu numarul maxim al culorii
	fscanf(in, "%d", &a->max);

	ignore_comments(in);

	//aloc memorie pentru noua matrice
	alloc_matrix(matrix, *a);

	//citesc noua matrice in functie de tipul de imagine dat de magic_word
	read_image(*matrix, *a, in);

	fclose(in);
	*loaded = 1;
	printf("Loaded %s\n", file);

	//selectez toata imaginea
	a->x1 = 0;
	a->y1 = 0;
	a->x2 = a->width;
	a->y2 = a->height;
}

//construiesc numarul x1 din stringul citit dupa SELECT
int build_number(char s[])
{
	int x1 = 0;
	int length = strlen(s);
	if (s[0] == '-') {
		for (int i = 1; i < length; i++)
			x1 = x1 * 10 + (s[i] - '0');
		return -x1;
	}
	for (int i = 0; i < length; i++)
		x1 = x1 * 10 + (s[i] - '0');
	return x1;
}

void swap_integer(int *a, int *b)
{
	int aux = *a;
	*a = *b;
	*b = aux;
}

int not_number(char string[100])
{
	int length = strlen(string);
	if (string[0] == '-') {
		for (int i = 1; i < length; i++)
			if (!strchr("0123456789", string[i]))
				return 1;
	} else {
		for (int i = 0; i < length; i++)
			if (!strchr("0123456789", string[i]))
				return 1;
	}
	return 0;
}

//functia de selectie a regiunii de lucru
void select_region(image *a, int loaded)
{
	char string[10];
	getchar();

	scanf("%s", string);
	if (strcmp(string, "ALL") == 0) {
		if (!loaded) {
			printf("No image loaded\n");
			return;
		}
		a->x1 = 0;
		a->x2 = a->width;
		a->y1 = 0;
		a->y2 = a->height;
		printf("Selected ALL\n");
	} else {
		char ch;
		scanf("%c", &ch);
		if (ch == '\n') {
			printf("Invalid command\n");
			return;
		}
		if (not_number(string)) {
			printf("Invalid command\n");
			return;
		}
		int x1 = build_number(string);

		char s_x2[10], s_y1[10], s_y2[10];
		scanf("%s%c", s_y1, &ch);
		if (ch == '\n') {
			printf("Invalid command\n");
			return;
		}
		if (not_number(s_y1)) {
			printf("Invalid command\n");
			return;
		}
		int y1 = build_number(s_y1);

		scanf("%s%c", s_x2, &ch);
		if (ch == '\n') {
			printf("Invalid command\n");
			return;
		}
		if (not_number(s_x2)) {
			printf("Invalid command\n");
			return;
		}
		int x2 = build_number(s_x2);

		scanf("%s", s_y2);
		if (not_number(s_y2)) {
			printf("Invalid command\n");
			return;
		}
		int y2 = build_number(s_y2);

		if (!loaded) {
			printf("No image loaded\n");
			return;
		}

		if (x1 > x2)
			swap_integer(&x1, &x2);
		if (y1 > y2)
			swap_integer(&y1, &y2);
		if (x1 < 0 || y1 < 0 || x2 > a->width || y2 > a->height ||
			x1 == x2 || y1 == y2) {
			printf("Invalid set of coordinates\n");
			return;
		}

		a->x1 = x1;
		a->x2 = x2;
		a->y1 = y1;
		a->y2 = y2;
		printf("Selected %d %d %d %d\n", x1, y1, x2, y2);
	}
}

void histogram(int **matrix, image a, int loaded)
{
	//verific daca s-a citit corect comanda
	char ch;
	scanf("%c", &ch);
	if (ch == '\n') {
		if (!loaded) {
			printf("No image loaded\n");
			return;
		}
		printf("Invalid command\n");
		return;
	}

	//citesc numarul maxim de stelute si de binuri si ma asigur
	//ca este introdusa corect comanda
	int x, y;
	scanf("%d", &x);
	scanf("%c", &ch);
	if (ch == '\n') {
		printf("Invalid command\n");
		return;
	}
	scanf("%d", &y);
	if (!loaded) {
		printf("No image loaded\n");
		return;
	}
	scanf("%c", &ch);
	if (ch != '\n') {
		char line[100];
		fgets(line, 100, stdin);
		printf("Invalid command\n");
		return;
	}

	//verific daca imaginea este grayscale sau color
	if (a.magic_w == 3 || a.magic_w == 6) {
		printf("Black and white image needed\n");
		return;
	}

	//vectorul de frecventa a aparitiilor tututror numerelor initializat cu 0
	int freq[300] = {0};

	int number_bins = 256 / y, sum = 0;
	int fr_max = 0;
	for (int i = 0; i < a.height; i++)
		for (int j = 0; j < a.width; j++)
			freq[matrix[i][j]]++;
	for (int i = 0; i < 256; i++) {
		sum += freq[i];
		if ((i + 1) % number_bins == 0) {
			if (sum > fr_max)
				fr_max = sum;
			sum = 0;
		}
	}

	double sum2 = 0;
	for (int i = 0; i < 256; i++) {
		sum2 += freq[i];
		if ((i + 1) % number_bins == 0) {
			double n_stars = (double)(sum2 / fr_max) * x;
			printf("%d\t|\t", (int)n_stars);
			for (int k = 0; k < (int)n_stars; k++)
				printf("*");
			printf("\n");
			sum2 = 0;
		}
	}
}

//functia care creeaza noul pixel dupa egalizarea imaginilor
void equalize(int **matrix, image a, int loaded)
{
	if (!loaded) {
		printf("No image loaded\n");
		return;
	}
	if (a.magic_w == 3 || a.magic_w == 6) {
		printf("Black and white image needed\n");
		return;
	}

	//aflu frecventa tuturor culorilor din matrice
	int freq[300] = {0};
	for (int i = 0; i < a.height; i++)
		for (int j = 0; j < a.width; j++)
			freq[matrix[i][j]]++;

	//aflu noile valori ale pixelilor
	double pixel[300] = {0};
	int sum = 0;
	for (int i = 0; i <= 255; i++)
		if (freq[i]) {
			sum = 0;
			for (int j = 0; j <= i; j++)
				sum += freq[j];
			pixel[i] = (double)a.max / (a.height * a.width) * sum;
		}

	//reactualizez matricea cu noile valori
	for (int i = 0; i < a.height; i++) {
		for (int j = 0; j < a.width; j++)
			matrix[i][j] = (int)pixel[matrix[i][j]];
	}
	printf("Equalize done\n");
}

//functia care reconstruieste matricea principala (folosita pentru CROP)
//(imagine grayscale)
void rebuild_matrix_grayscale(int **matrix, image a, int **aux_matrix)
{
	for (int i = 0; i < a.height; i++)
		for (int j = 0; j < a.width; j++)
			matrix[i][j] = aux_matrix[i][j];
}

//functia care reconstruieste matricea principala
//(imagine color)
void rebuild_matrix_color(int **matrix, image a, int **aux_matrix)
{
	for (int i = 0; i < a.height; i++)
		for (int j = 0; j < 3 * a.width; j++)
			matrix[i][j] = aux_matrix[i][j];
}

//functia principala pentru CROP
void crop_image(int ***matrix, image *a, int loaded)
{
	if (!loaded) {
		printf("No image loaded\n");
		return;
	}

	//fac urmatoarele operatii doar daca selectul anterior nu a fost SELECT ALL
	if (!(a->width == a->x2 && a->height == a->y2 && !(a->x1) && !(a->y1))) {
		//actualizez noua inaltime si latime dar retin vechea inaltime
		//pentru a putea elibera memoria lui matrix
		int old_height = a->height;
		a->height = a->y2 - a->y1;
		a->width = a->x2 - a->x1;
		//declar o matrice auxiliara in care sa copiez matricea din cadrul
		//selectiei curente, careia ii aloc noile dimensiuni
		int **aux_matrix, q = 0, r = 0;
		alloc_matrix(&aux_matrix, *a);

		if (a->magic_w == 2 || a->magic_w == 5) {
			for (int i = a->y1; i < a->y2; i++) {
				r = 0;
				for (int j = a->x1; j < a->x2; j++) {
					aux_matrix[q][r] = (*matrix)[i][j];
					r++;
				}
				q++;
			}
			free_matrix(*matrix, old_height);
			alloc_matrix(matrix, *a);
			rebuild_matrix_grayscale(*matrix, *a, aux_matrix);
		} else {
			for (int i = a->y1; i < a->y2; i++) {
				r = 0;
				for (int j = 3 * a->x1; j < 3 * a->x2; j++) {
					aux_matrix[q][r] = (*matrix)[i][j];
					r++;
				}
				q++;
			}
			free_matrix(*matrix, old_height);
			alloc_matrix(matrix, *a);
			rebuild_matrix_color(*matrix, *a, aux_matrix);
		}
		//reactualizez selectia la SELECT ALL
		a->x1 = 0;
		a->y1 = 0;
		a->x2 = a->width;
		a->y2 = a->height;
		free_matrix(aux_matrix, a->height);
	}
	printf("Image cropped\n");
}

//functie folosita pentru APPLY care creeaza cele 4 matrici de 3x3
//si caluleaza noul pixel
int build_pixel(int **aux_matrix, int i, int j, char parameter[])
{
	//matricea nucleu si suma prin care calculez noua valoare a pixelului
	double nucleu[4][4], sum = 0;

	//construiesc nucleele in functie de parametru
	if (!strcmp(parameter, "EDGE")) {
		for (int k = 0; k < 3; k++)
			for (int q = 0; q < 3; q++) {
				if (k == 1 && q == 1)
					nucleu[k][q] = 8;
				else
					nucleu[k][q] = -1;
			}
	} else if (!strcmp(parameter, "SHARPEN")) {
		for (int k = 0; k < 3; k++)
			for (int q = 0; q < 3; q++) {
				if (k == 1 && q == 1)
					nucleu[k][q] = 5;
				else if ((k == 0 || k == 2) && (q == 0 || q == 2))
					nucleu[k][q] = 0;
				else
					nucleu[k][q] = -1;
			}
	} else if (!strcmp(parameter, "BLUR")) {
		for (int k = 0; k < 3; k++)
			for (int q = 0; q < 3; q++)
				nucleu[k][q] = (double)(1. / 9);
	} else if (!strcmp(parameter, "GAUSSIAN_BLUR")) {
		for (int k = 0; k < 3; k++)
			for (int q = 0; q < 3; q++) {
				if (k == 1 && q == 1)
					nucleu[k][q] = (double)(4. / 16);
				else if ((k == 0 || k == 2) && (q == 0 || q == 2))
					nucleu[k][q] = (double)(1. / 16);
				else
					nucleu[k][q] = (double)(2. / 16);
			}
	}

	//construiesc suma
	int m = 0, n = 0;
	for (int k = i - 1; k <= i + 1; k++) {
		n = 0;
		for (int q = j - 3; q <= j + 3; q += 3) {
			sum += (double)(aux_matrix[k][q] * nucleu[m][n]);
			n++;
		}
		m++;
	}
	if (sum < 0)
		sum = 0;
	if (sum > 255)
		sum = 255;
	sum = round(sum);
	return (int)sum;
}

void apply_matrix(int **matrix, image a, char parameter[])
{
	//aflam coordonatele pixelilor care urmeaza sa fie prelucrati
	int start_i = a.y1, start_j = 3 * a.x1, stop_i = a.y2, stop_j = 3 * a.x2;
	if (a.x1 - 1 < 0)
		start_j += 3;
	if (a.y1 - 1 < 0)
		start_i++;
	if (a.x2 == a.width)
		stop_j -= 3;
	if (a.y2 == a.height)
		stop_i--;

	//construiesc o matrice auxiliara
	int **aux_matrix;
	alloc_matrix(&aux_matrix, a);
	for (int i = 0; i < a.height; i++)
		for (int j = 0; j < 3 * a.width; j++)
			aux_matrix[i][j] = matrix[i][j];

	for (int i = start_i; i < stop_i; i++)
		for (int j = start_j; j < stop_j; j++)
			matrix[i][j] = build_pixel(aux_matrix, i, j, parameter);
	free_matrix(aux_matrix, a.height);
}

//functia principala pentru APPLY
void apply(int **matrix, image a, int loaded)
{
	//aflu daca s-a introdus vreun parametru
	char ch;
	scanf("%c", &ch);
	if (ch == '\n') {
		if (!loaded)
			printf("No image loaded\n");
		else
			printf("Invalid command\n");
		return;
	}
	char parameter[15];
	scanf("%s", parameter);

	//verific daca exista vreo imagine in memorie
	if (!loaded) {
		printf("No image loaded\n");
		return;
	}

	//verific daca s-a introdus un parametru valid
	char s[5][15] = {"EDGE", "SHARPEN", "BLUR", "GAUSSIAN_BLUR"};
	int count = 0;
	for (int i = 0; i < 4; i++) {
		if (!strstr(s[i], parameter))
			count++;
		if (count == 4) {
			printf("APPLY parameter invalid\n");
			return;
		}
	}

	//verific daca imaginea este grayscale
	if (a.magic_w == 2 || a.magic_w == 5) {
		printf("Easy, Charlie Chaplin\n");
		return;
	}

	apply_matrix(matrix, a, parameter);
	printf("APPLY %s done\n", parameter);
}

//functia pentru salvarea imaginii in format ascii
void save_ascii(FILE *out, int **matrix, image a)
{
	if (a.magic_w == 5)
		a.magic_w = 2;
	if (a.magic_w == 6)
		a.magic_w = 3;
	fprintf(out, "P%d\n%d %d\n%d\n", a.magic_w, a.width, a.height, a.max);
	if (a.magic_w == 2) {
		for (int i = 0; i < a.height; i++) {
			for (int j = 0; j < a.width; j++)
				fprintf(out, "%d ", matrix[i][j]);
			fprintf(out, "\n");
		}
	} else {
		for (int i = 0; i < a.height; i++) {
			for (int j = 0; j < 3 * a.width; j++)
				fprintf(out, "%d ", matrix[i][j]);
			fprintf(out, "\n");
		}
	}
}

//functia pentru salvarea imaginii in format binar
void save_binary(FILE *out, int **matrix, image a)
{
	if (a.magic_w == 2)
		a.magic_w = 5;
	if (a.magic_w == 3)
		a.magic_w = 6;
	fprintf(out, "P%d\n%d %d\n%d\n", a.magic_w, a.width, a.height, a.max);
	if (a.magic_w == 5) {
		for (int i = 0; i < a.height; i++)
			for (int j = 0; j < a.width; j++)
				fwrite(&matrix[i][j], sizeof(char), 1, out);
	} else {
		for (int i = 0; i < a.height; i++)
			for (int j = 0; j < 3 * a.width; j++)
				fwrite(&matrix[i][j], sizeof(char), 1, out);
	}
}

//functia principala pentru salvarea imaginii intr-un nou fisier binar/text
void save_image(int **matrix, image a, int loaded)
{
	//deschid noul fisier
	char new_file[100], line[100];
	fgets(line, 100, stdin);
	int rv = sscanf(line, "%s", new_file);
	if (rv != 1) {
		printf("eroare\n");
		return;
	}
	if (loaded == 0) {
		printf("No image loaded\n");
		return;
	}
	FILE *out = fopen(new_file, "wb");
	if (!out) {
		printf("Failed to open %s\n", new_file);
		return;
	}

	//verific daca dupa comanda save s-a introdus si "ascii"
	if (strstr(line, "ascii"))
		save_ascii(out, matrix, a);
	else
		save_binary(out, matrix, a);

	//inchid fisierul si afisez mesajul de salvare
	fclose(out);
	printf("Saved %s\n", new_file);
}

//rotirea cu 90 de grade a intregii imagini (grayscale)
void rotate_90_grayscale(int ***matrix, image *a, int ***aux)
{
	//aloc o matrice auxiliara pe care o egalez cu matricea curenta
	alloc_matrix(aux, *a);
		for (int i = 0; i < a->height; i++)
			for (int j = 0; j < a->width; j++)
				(*aux)[i][j] = (*matrix)[i][j];

	//dealoc memoria vechii matrice si o realoc cu dimensiunile inversate
	free_matrix(*matrix, a->height);
	alloc_matrix_tr(matrix, *a);

	//parcurg matricea de sus in jos si de la dreapta la stanga
	int k = 0, q = 0;
	for (int i = a->height - 1; i >= 0; i--) {
		for (int j = 0; j < a->width; j++) {
			(*matrix)[j][i] = (*aux)[k][q];
			q++;
			if (q == a->width) {
				q = 0;
				k++;
			}
		}
	}
	//dealoc memoria de la matricea auxiliara
	free_matrix(*aux, a->height);
	//fac interschimbarea intre dimensiuni
	swap_integer(&a->height, &a->width);
	//actualizez select all
	a->x2 = a->width;
	a->y2 = a->height;
}

//rotirea cu 90 grade a intregii imagini (color)
void rotate_90_color(int ***matrix, image *a, int ***aux)
{
	//aloc o matrice auxiliara pe care o egalez cu matricea curenta
	alloc_matrix(aux, *a);
		for (int i = 0; i < a->height; i++)
			for (int j = 0; j < 3 * a->width; j++)
				(*aux)[i][j] = (*matrix)[i][j];

	//dealoc memoria vechii matrice si o realoc cu dimensiunile inversate
	free_matrix(*matrix, a->height);
	alloc_matrix_tr(matrix, *a);

	//parcurg matricea de sus in jos si de la dreapta la stanga
	int k = 0, q = 0;
	for (int i = 3 * (a->height - 1); i >= 0; i -= 3) {
		for (int j = 0; j < a->width; j++) {
			(*matrix)[j][i] = (*aux)[k][q];
			(*matrix)[j][i + 1] = (*aux)[k][q + 1];
			(*matrix)[j][i + 2] = (*aux)[k][q + 2];
			q += 3;
			if (q == 3 * a->width) {
				q = 0;
				k++;
			}
		}
	}
	//dealoc memoria matricei auxiliare
	free_matrix(*aux, a->height);
	//interschimb dimensiunile
	swap_integer(&a->height, &a->width);
	//actualizez select all
	a->x2 = a->width;
	a->y2 = a->height;
}

//functia de ROTATE pentru intreaga selectie (SELECT ALL)
void rotate_all(int ***matrix, image *a, int angle)
{
	int **aux;
	if (angle == 180 || angle == -180) {
		if (a->magic_w == 2 || a->magic_w == 5) {
			rotate_90_grayscale(matrix, a, &aux);
			rotate_90_grayscale(matrix, a, &aux);
		} else {
			rotate_90_color(matrix, a, &aux);
			rotate_90_color(matrix, a, &aux);
		}
	}

	if (angle == 90 || angle == -270) {
		if (a->magic_w == 2 || a->magic_w == 5)
			rotate_90_grayscale(matrix, a, &aux);
		else
			rotate_90_color(matrix, a, &aux);
	}

	if (angle == -90 || angle == 270) {
		if (a->magic_w == 2 || a->magic_w == 5) {
			rotate_90_grayscale(matrix, a, &aux);
			rotate_90_grayscale(matrix, a, &aux);
			rotate_90_grayscale(matrix, a, &aux);
		} else {
			rotate_90_color(matrix, a, &aux);
			rotate_90_color(matrix, a, &aux);
			rotate_90_color(matrix, a, &aux);
		}
	}
}

//rotirea cu 90 de grade a unei selectii dintr-o imagine grayscale
void rotate_90_grayscale_selected(int **matrix, image a, int ***aux)
{
	//aloc o matrice auxiliara pe care o egalez cu matricea curenta
	alloc_matrix(aux, a);
	for (int i = 0; i < a.height; i++)
		for (int j = 0; j < a.width; j++)
			(*aux)[i][j] = matrix[i][j];

	//parcurg matricea curenta de sus in jos si de la dreapta la stanga
	int k = a.y1, q = a.x1;
	for (int i = a.x2 - 1; i >= a.x1; i--) {
		for (int j = a.y1; j < a.y2; j++) {
			matrix[j][i] = (*aux)[k][q];
			q++;
			if (q == a.x2) {
				q = a.x1;
				k++;
			}
		}
	}
	//eliberez memoria matricei auxiliare
	free_matrix(*aux, a.height);
}

//rotirea cu 90 de grade a unei selectii dintr-o imagine color
void rotate_90_color_selected(int **matrix, image a, int ***aux)
{
	//aloc o matrice auxiliara pe care o egalez cu matricea curenta
	alloc_matrix(aux, a);
	for (int i = 0; i < a.height; i++)
		for (int j = 0; j < 3 * a.width; j++)
			(*aux)[i][j] = matrix[i][j];

	//parcurg matricea curenta de sus in jos si de la dreapta la stanga
	int k = a.y1, q = 3 * a.x1;
	for (int i = 3 * (a.x2 - 1); i >= 3 * a.x1; i -= 3) {
		for (int j = a.y1; j < a.y2; j++) {
			matrix[j][i] = (*aux)[k][q];
			matrix[j][i + 1] = (*aux)[k][q + 1];
			matrix[j][i + 2] = (*aux)[k][q + 2];
			q += 3;
			if (q == 3 * a.x2) {
				q = 3 * a.x1;
				k++;
			}
		}
	}
	//eliberez memoria matricei auxiliare
	free_matrix(*aux, a.height);
}

//functia de rotate pentru o anumita selectie(inafara de SELECT ALL)
void rotate_selection(int **matrix, image a, int angle)
{
	int **aux;
	if (angle == 180 || angle == -180) {
		if (a.magic_w == 2 || a.magic_w == 5) {
			rotate_90_grayscale_selected(matrix, a, &aux);
			rotate_90_grayscale_selected(matrix, a, &aux);
		} else {
			rotate_90_color_selected(matrix, a, &aux);
			rotate_90_color_selected(matrix, a, &aux);
		}
	}

	if (angle == 90 || angle == -270) {
		if (a.magic_w == 2 || a.magic_w == 5)
			rotate_90_grayscale_selected(matrix, a, &aux);
		else
			rotate_90_color_selected(matrix, a, &aux);
	}

	if (angle == -90 || angle == 270) {
		if (a.magic_w == 2 || a.magic_w == 5) {
			rotate_90_grayscale_selected(matrix, a, &aux);
			rotate_90_grayscale_selected(matrix, a, &aux);
			rotate_90_grayscale_selected(matrix, a, &aux);
		} else {
			rotate_90_color_selected(matrix, a, &aux);
			rotate_90_color_selected(matrix, a, &aux);
			rotate_90_color_selected(matrix, a, &aux);
		}
	}
}

//functia principala pentru ROTATE
void rotate(int ***matrix, image *a, int loaded)
{
	int angle;
	scanf("%d", &angle);
	if (!loaded) {
		printf("No image loaded\n");
		return;
	}
	if (angle != 360 && angle != -360 && angle != 90 && angle != -90 &&
		angle != 180 && angle != -180 && angle != 270 && angle != -270 &&
		angle != 0) {
		printf("Unsupported rotation angle\n");
		return;
	}
	if (!a->x1 && !a->y1 && a->x2 == a->width && a->y2 == a->height) {
		if (angle != 360 && angle != -360 && angle != 0)
			rotate_all(matrix, a, angle);
	} else {
		if (a->x2 - a->x1 != a->y2 - a->y1) {
			printf("The selection must be square\n");
			return;
		}
		rotate_selection(*matrix, *a, angle);
	}
	printf("Rotated %d\n", angle);
}

//se apeleaza cand un parametru este invalid pentru a citi intreaga linie
void check_invalid(void)
{
	char c;
	scanf("%c", &c);
	if (c == ' ') {
		char line[100];
		fgets(line, 100, stdin);
	}
	printf("Invalid command\n");
}

void exit_from_program(int **matrix, image a, int loaded)
{
	if (!loaded)
		printf("No image loaded\n");
	else
		free_matrix(matrix, a.height);
}

int main(void)
{
	int loaded = 0, **matrix;
	char command[15];
	image a;
	scanf("%s", command);
	while (strcmp(command, "EXIT") != 0) {
		if (strcmp(command, "LOAD") == 0)
			load_image(&matrix, &a, &loaded);
		else if (!strcmp(command, "SELECT"))
			select_region(&a, loaded);
		else if (!strcmp(command, "HISTOGRAM"))
			histogram(matrix, a, loaded);
		else if (!strcmp(command, "EQUALIZE"))
			equalize(matrix, a, loaded);
		else if (!strcmp(command, "CROP"))
			crop_image(&matrix, &a, loaded);
		else if (!strcmp(command, "APPLY"))
			apply(matrix, a, loaded);
		else if (!strcmp(command, "SAVE"))
			save_image(matrix, a, loaded);
		else if (!strcmp(command, "ROTATE"))
			rotate(&matrix, &a, loaded);
		else
			check_invalid();
		scanf("%s", command);
	}
	exit_from_program(matrix, a, loaded);
	return 0;
}
