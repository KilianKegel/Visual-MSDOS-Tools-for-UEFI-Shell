#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#undef NULL
#include <uefi.h>


extern EFI_SYSTEM_TABLE *gST;

#define ELC(x) ( sizeof(x) / sizeof(x[0]) )

int main(int argc, char **argv) {
	UINTN Cols, Rows, Line = 0;
	int c,i;
	char *pBuf,*p;

	if (argc > 1) {
		fprintf(stderr,"Too many arguments in command line.\n");
	}
	else {

		gST->ConOut->QueryMode(gST->ConOut, (UINTN)gST->ConOut->Mode->Mode, &Cols, &Rows);	// get number of text lines

		for (pBuf = NULL, i = 0; EOF != (c = fgetc(stdin)); i++) {							// read character from STDIN until EOF
			pBuf = realloc(pBuf, i + 1);													// allocate buffer / increase buffersize
			if (NULL == pBuf)
				abort();
			pBuf[i] = (char)c;																// store character in memory buffer
		}

		pBuf[i] = '\0';																		// set string termination

		for (Line = 0, p = strtok(pBuf, "\n"); p != NULL; Line++)							// display the text screen wise
		{
			if (Rows == 1 + Line) {
				UINTN Index;
				EFI_INPUT_KEY Key;
				EFI_STATUS Status;
				printf("-- More --");
				do {																		// read real kbhit(), since STDIN is redirected
					gST->BootServices->WaitForEvent(1, gST->ConIn->WaitForKey, &Index);
					Status = gST->ConIn->ReadKeyStroke(gST->ConIn, &Key);
				} while (EFI_SUCCESS != Status);
				printf("\n");
				Line = 0;
			}
			printf("%s\n", p);																// print the text line
			p = strtok(NULL, "\n");															// get the next text line
		}
	}
	return EFI_SUCCESS;
}
