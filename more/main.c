/*!
@copyright
    Copyright (c) 2019, Kilian Kegel. All rights reserved.
    This program and the accompanying materials are licensed and made
    available under the terms and conditions of the BSD License
    which accompanies this distribution.  The full text of the license
    may be found at

    http://opensource.org/licenses/bsd-license.php
    THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
    WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
@file main.c
@brief The <b><em>MORE</em></b> command for UEFI shell\n

@details \n
@todo
    1. add UTF16 vs ASCII distinction for directly passed files
        e.g. more UTF16File1.txt UTF16File2.txt - still fails
        but more < UTF16File.txt - works!
@mainpage
    <b><em>MORE</em></b> command for UEFI Shell
@section intro_sec Introduction
    The <b><em>MORE</em></b> command for UEFI shell\n

@subsection Parm_sec Command line parameter examples
    1. type AsciiOrUTF16File.txt  | more
    2. dir  AsciiOrUTF16File.txt  | more
    3. more AsciiFile1.txt AsciiFile2.txt AsciiFile3.txt

*/#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#undef NULL
#include <uefi.h>

extern EFI_SYSTEM_TABLE* gST;

#define ELC(x) ( sizeof(x) / sizeof(x[0]) ) /* element count */

int main(int argc, char** argv) {
    UINTN Cols, Rows, Line = 0;
    int c, i, n = 1;
    char* pBuf, * p;
    FILE* fp = stdin;                                                                       // take file from STDIN, by default

    gST->ConOut->QueryMode(gST->ConOut, (UINTN)gST->ConOut->Mode->Mode, &Cols, &Rows);      // get number of text lines of the screen

    do {
        //
        // check, if filename is passed in the commandline. If so, read from file instead from STDIN
        //
        if (argc > 1) {                                                                     // take filename from commandline
            if (NULL == (fp = fopen(argv[n], "r"))) {
                fprintf(stderr, "Cannot access file %s\n", argv[n]);                        // file is not present...
                break;                                                                      // ...exit program
            }
        }
        //
        // read from file/STDIN until EOF to continously enlarged buffer
        //
        for (pBuf = NULL, i = 0; EOF != (c = fgetc(fp)); i++) {                             // read character from file/STDIN until EOF
            pBuf = realloc(pBuf, i + 1);                                                    // allocate buffer / increase buffersize
            if (NULL == pBuf)                                                               // check allocation failure...
                abort();                                                                    // ...abort(), if so
            pBuf[i] = (char)c;                                                              // store character in memory buffer
        }

        pBuf[i] = '\0';                                                                     // set string termination

        //
        // Buffer contains the entire file now. Print file line-wise to the screen and stop after # rows and wait for key
        //
        for (Line = 0, p = strtok(pBuf, "\n"); p != NULL; Line++)                           // display the text screen wise
        {
            if (Rows == 1 + Line) {
                UINTN Index;
                EFI_INPUT_KEY Key;
                EFI_STATUS Status;
                printf("-- More --");
                do {                                                                        // read real kbhit(), since STDIN is redirected
                    gST->BootServices->WaitForEvent(1, gST->ConIn->WaitForKey, &Index);
                    Status = gST->ConIn->ReadKeyStroke(gST->ConIn, &Key);
                } while (EFI_SUCCESS != Status);
                printf("\n");
                Line = 0;
            }
            printf("%s\n", p);                                                              // print the text line
            p = strtok(NULL, "\n");                                                         // get the next text line
        }
        if(stdin != fp)
            fclose(fp);                                                                         // close fp, that was opened above
    } while (++n < argc);                                                                   // next file...

    return EFI_SUCCESS;
}

