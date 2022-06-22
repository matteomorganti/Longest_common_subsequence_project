#include<stdio.h>
#include<stdlib.h>


#define max(x, y) (((x) > (y)) ? (x) : (y))

size_t fileSize(FILE* fp) {
	fseek(fp, 0, SEEK_END);
	size_t size = ftell(fp);
	rewind(fp);

	return size;
}

void createDifferenceGrid(int*** grid, size_t f1_size, size_t f2_size) {
	*grid = (int **)calloc(f1_size+1,sizeof(int *));
	if (*grid == NULL) {
		printf("Impossibile allocare memoria! \n");
		exit(1);
	}
	for (int i = 0; i <= f1_size; i++) {
		(* grid)[i] = calloc(f2_size+1, sizeof(int));
		if ((*grid)[i] == NULL) {
			printf("Impossibile allocare memoria! \n");
			exit(1);
		}
	}
}

void fillDifferenceGrid(int** grid, size_t f1_size, size_t f2_size, FILE *f1, FILE *f2) {
	char byte1;
	char byte2;
	for (int i = 0; i <= f1_size; i++) {
		fread(&byte1, 1, 1, f1);
		for (int j = 0; j <= f2_size; j++) {
			if (i == 0) {
				grid[i][j] = 0;
				rewind(f1);
			}
			else if (j == 0) {
				grid[i][j] = 0;
			}
			else {
				fread(&byte2, 1, 1, f2);
				if (byte1 == byte2) {
					grid[i][j] = grid[i - 1][j - 1] + 1;
				}
				else {
					grid[i][j] = max(grid[i - 1][j], grid[i][j - 1]);
				}
			}
		}
		rewind(f2);
	}
}

void writeFile(char* lcs_bytes,size_t lcs_size, FILE* fpout) {
	fwrite(lcs_bytes, 1, lcs_size, fpout);
}

void longestCommonSubsequence(char* file1, char* file2, char* outFile) {
	FILE* fp1 = fopen(file1, "rb");
	FILE* fp2 = fopen(file2, "rb");
	FILE* fpout = fopen(outFile, "w+");

	if (fp1 == NULL || fp2 == NULL || fpout == NULL) {
		printf("Errore: Impossibile aprire il file!\n");
		exit(1);
	}

	size_t f1_size = fileSize(fp1);
	size_t f2_size = fileSize(fp2);

	int** difference_grid = NULL;

	createDifferenceGrid(&difference_grid, f1_size, f2_size);
	fillDifferenceGrid(difference_grid, f1_size, f2_size, fp1, fp2);

	int lcs_size = difference_grid[f1_size][f2_size];
	char* lcs_bytes = calloc(lcs_size, sizeof(char));

	if (lcs_bytes == NULL) {
		printf("Impossibile allocare memoria!\n");
		exit(1);
	}

	int i = f1_size, j = f2_size;
	int index = lcs_size;
	fseek(fp1, 0, SEEK_END);
	fseek(fp2, 0, SEEK_END);
	fseek(fp1, -1, SEEK_CUR);
	fseek(fp2, -1, SEEK_CUR);

	char byte1, byte2;
	while (i > 0 && j > 0) {

		fread(&byte1, 1, 1, fp1);
		fseek(fp1, -1, SEEK_CUR);
		fread(&byte2, 1, 1, fp2);
		fseek(fp2, -1, SEEK_CUR);

		if (byte1 == byte2) {
			lcs_bytes[index-1] = byte1;
			i--;
			j--;
			index--;
			fseek(fp1, -1, SEEK_CUR);
			fseek(fp2, -1, SEEK_CUR);

		}
		else if(difference_grid[i - 1][j] > difference_grid[i][j - 1]) {
			fseek(fp1, -1, SEEK_CUR);
			i--;
		}
		else {
			fseek(fp2, -1, SEEK_CUR);
			j--;
		}
	}

	writeFile(lcs_bytes, lcs_size, fpout);

	fclose(fp1);
	fclose(fp2);
	fclose(fpout);

	printf("La massima sottosequenza comune viene generata in %s.\n", outFile);
}

int main(int argc, char * argv[]) {

	if (argc != 4) {
		printf("Usage: %s <inputfile1> <inputfile2> <outputfile>\n",argv[0]);
		exit(0);
	}
	longestCommonSubsequence(argv[1], argv[2], argv[3]);
	return 0;
}
