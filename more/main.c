/*!
@copyright
    Copyright (c) 2020-2023, Kilian Kegel. All rights reserved.
    This program and the accompanying materials are licensed and made
    available under the terms and conditions of the BSD License
    which accompanies this distribution.  The full text of the license
    may be found at

    http://opensource.org/licenses/bsd-license.php
    THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
    WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
@file main.c
@brief The <b><em>MORE</em></b> command for UEFI shell\n

@mainpage
    <b><em>MORE</em></b> command for UEFI Shell
@section intro_sec Introduction
    The <b><em>MORE</em></b> command for UEFI shell\n

@subsection Parm_sec Command line parameter examples
    1. type AsciiOrUTF16File.txt  | more
    2. dir  AsciiOrUTF16File.txt  | more
    3. more AsciiFile1.txt AsciiFile2.txt AsciiFile3.txt

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#undef NULL
#include <uefi.h>

#define ELC(x) ( sizeof(x) / sizeof(x[0]) )                         // element count

int main(int argc, char** argv) {
    EFI_SYSTEM_TABLE* pEfiSystemTable = (void*)(argv[-1]);          // pEfiSystemTable is passed in argv[-1]
    EFI_HANDLE* hEfiImageHandle = (void*)(argv[-2]);                // ImageHandle is passed in argv[-2]
    UINTN Cols, Rows, Line = 0,LineLength, RowsPerLine;             //
    int c, i, n = 1, idiv = 1;                                      //
    char* pBuf, * p;                                                //
    FILE* fp = stdin;                                               // take file from STDIN, by default
    char fUTF16 = 0;                                                // flag file format
    int nRet = EXIT_SUCCESS;
    int fRedir = (0 == _isatty(_fileno(stdout)));                   // is redirected to a file?

    if (!fRedir)                                                    // if _not_ redirected to a file
    {
        pEfiSystemTable->ConOut->QueryMode(                         // get number of text lines of the screen
            pEfiSystemTable->ConOut,
            (UINTN)pEfiSystemTable->ConOut->Mode->Mode,
            &Cols, &Rows
        );
    }

    do {
        //
        // check, if filename is passed in the commandline. If so, read from file instead from STDIN
        //
        if (argc > 1) {                                             // take filename from commandline
            if (NULL == (fp = fopen(argv[n], "r"))) {               // open filename from commandline
                fprintf(stderr, "Cannot access file %s\n", argv[n]);// file is not present...
                nRet = EXIT_FAILURE;                                // set exit code
                break;                                              // ...exit program
            }
            else {                                                // detect file format
             //
             // check BOM byte order mark UTF-16 (LE) https://en.wikipedia.org/wiki/Byte_order_mark#UTF-16
             //
                unsigned short BOM = 0;
                fread(&BOM, 2, 1, fp);
                fUTF16 = (0xFEFF == BOM);
                if (0 == fUTF16)
                    rewind(fp);
            }
        }

        //
        // read from file/STDIN until EOF to continously enlarged buffer
        //
        idiv = fUTF16 ? 2 : 1;                                      // adjust idiv to decide skip zero high byte 
        pBuf = NULL;                                                // inital request for realloc()
        for (i = 0; EOF != (c = fgetc(fp)); i++) {                  // read character from file/STDIN until EOF
            if (fUTF16)                                             // if UTF16 file format...
                if (0 != (i % 2))                                   // ... if zero high byte...
                    continue;                                       // ... skip that byte and continue
            pBuf = realloc(pBuf, i / idiv + 1);                     // allocate buffer / increase buffersize
            if (NULL == pBuf)                                       // check allocation failure...
                abort();                                            // ...abort(), if so
            pBuf[i / idiv] = (char)c;                               // store character in memory buffer
        }                                                           //
        pBuf[i / idiv] = '\0';                                      // set string termination

        //
        // Buffer contains the entire file now. Print file line-wise to the screen and stop after # rows and wait for key
        //
        for (Line = 0, p = strtok(pBuf, "\n"); p != NULL; Line += RowsPerLine)   // display the text screen wise
        {
            LineLength = strlen(p);
            RowsPerLine = LineLength / Cols + 1;

            if (0 == fRedir) 
            {
                if (Rows <= RowsPerLine + Line) {
                    UINTN Index;
                    EFI_INPUT_KEY Key;
                    EFI_STATUS Status;
                    printf("-- More --");
                    do {                                            // read real kbhit(), since STDIN is redirected
                        pEfiSystemTable->BootServices->WaitForEvent(1, pEfiSystemTable->ConIn->WaitForKey, &Index);
                        Status = pEfiSystemTable->ConIn->ReadKeyStroke(pEfiSystemTable->ConIn, &Key);
                    } while (EFI_SUCCESS != Status);
                    printf("\r\r\r\r\r\r\r\r\r\r");                 // back to start of line
                    printf("          ");                           // overwrite "-- More --"
                    printf("\r\r\r\r\r\r\r\r\r\r");                 // back to start of line
                    Line = 0;
                }
            }
            printf("%s\n", p);                                      // print the text line
            p = strtok(NULL, "\n");                                 // get the next text line
        }
        if (stdin != fp)
            fclose(fp);                                             // close fp, that was opened above
    } while (++n < argc);                                           // next file...

    return nRet;
}
