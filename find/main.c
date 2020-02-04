/*!
@copyright
    Copyright (c) 2020, Kilian Kegel. All rights reserved.
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
    <b><em>FIND</em></b> command for UEFI Shell
@section intro_sec Introduction
    The <b><em>FIND</em></b> command for UEFI shell\n

@subsection Parm_sec Command line parameter examples
    1. find "seachstring" AsciiFile.txt
    2. dir | find "seachstring" /v /n /i 
    3. find /?
*/
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define ELC(x) ( sizeof(x) / sizeof(x[0]) ) /* element count */
#define MAXSLASHPARM 7
#define MAXLINELEN 1024 

int parm[MAXSLASHPARM] = { 0 };

int IsSlashParm(int iargv) {
    int i = 0, nRet = 0;

    return (parm[0] == iargv || parm[1] == iargv || parm[2] == iargv || parm[3] == iargv || parm[4] == iargv || parm[5] == iargv || parm[6] == iargv);

}

int IsFile(char** argv, int iargv) {
    int nRet = 0;

    do {
        if (1 == IsSlashParm(iargv))
            break;                  //break with nRet == 1 -> is '/C' or '/N' or ...
        if (0 == strncmp("/", argv[iargv], strlen("/"))) {
            nRet = -1;
            break;
        }
        nRet = 1;
    } while (0);

    return nRet;
}

int main(int argc, char** argv) {
    int i, iargv = 2;
    FILE* fp = stdin;                                                                       // take file from STDIN, by default
    enum PARM { V, C, N, I, O/*FF*/, L/*OFFLINE*/, H };
    int nRet = 0;
    int cntFile = 0;
    size_t linelen, stringlen;
    char* pLineOrg = malloc(MAXLINELEN), * pLineUp = malloc(MAXLINELEN) , * pString;
    int line,count;

    do {
        //
        // get parameters form command line
        //
        if (argc > 1) {
            stringlen = strlen(argv[1]);  // first argument IS! THE! searchstring. (the ""magic stuff can not be implemented in Standard C)
            pString = malloc(1 + stringlen);
            strcpy(pString, argv[1]);
        }
        else {
            fprintf(stderr, "FIND: Parameter format not correct\n");
            nRet = 1;
            break;//while(0)
        }

        for (i = 1; i < argc; i++) {

            parm[H] = (0 == strcmp("/h", argv[i])) ? i : parm[H];
            parm[H] = (0 == strcmp("/H", argv[i])) ? i : parm[H];
            parm[H] = (0 == strcmp("/?", argv[i])) ? i : parm[H];
            parm[V] = (0 == strcmp("/v", argv[i])) ? i : parm[V];
            parm[V] = (0 == strcmp("/V", argv[i])) ? i : parm[V];
            parm[C] = (0 == strcmp("/c", argv[i])) ? i : parm[C];
            parm[C] = (0 == strcmp("/C", argv[i])) ? i : parm[C];
            parm[N] = (0 == strcmp("/n", argv[i])) ? i : parm[N];
            parm[N] = (0 == strcmp("/N", argv[i])) ? i : parm[N];
            parm[I] = (0 == strcmp("/i", argv[i])) ? i : parm[I];
            parm[I] = (0 == strcmp("/I", argv[i])) ? i : parm[I];
            //parm[O] = (0 == strcmp("/OFF", argv[i])) ? i : parm[O];
            //parm[L] = (0 == strcmp("/OFFLINE", argv[i])) ? i : parm[L];
        }
        //
        // print help screen and exit
        //
        if (parm[H]) {
            printf("Searches for a text string in a file or files or in STDIN\n\n");
            printf("FIND \"string\" [/ V][/ C][/ N][/ I] [[drive:][path]filename[...]]\n");
            printf("  \"string\"    is the first parameter w/ or w/o quotation marks\n");
            printf("  /V or /v   Displays all lines NOT containing the specified string.\n");
            printf("  /C or /c   Displays only the count of lines containing the string.\n");
            printf("  /N or /n   Displays line numbers with the displayed lines.\n");
            printf("  /I or /i   Ignores the case of characters when searching for the string.\n");
            break;
        }
        //printf("parmV = %d, parmC = %d, parmN = %d, parmI = %d, parmO = %d, parmL = %d\n", parm[V], parm[C], parm[N], parm[I], parm[O], parm[L]);

        for (i = 2; i < argc; i++) {
            int xRet = IsFile(argv, i);

            if (-1 == xRet) {
                fprintf(stderr, "FIND: Invalid switch \"%s\"\n",argv[i]);
                break;
            }

            cntFile += (1 == xRet ? 1 : 0); // count number of files passed in the command line
        }

        //printf("cntFile: %d\n", cntFile);   // debug
        if (0 != parm[I])                            // case independance upcase
            for (i = 0; i < stringlen; i++)
                pString[i] = toupper(pString[i]);

        do {
            //
            // check, if filename is passed in the commandline. If not so, read from file instead from STDIN
            //
            if (0 < cntFile ) {                                             // take filename from commandline
                if (!IsFile(argv, iargv))
                    continue;
                if (NULL == (fp = fopen(argv[iargv], "r"))) {
                    fprintf(stderr, "File not found - %s\n", argv[iargv]);      // file is not present...
                    continue;                                               // ...exit program
                }
                else {
                    printf("---------- %s%s", argv[iargv], 0 == parm[C] ? "\n" : ": ");
                }
            }
            
            line = 0;
            count = 0;

            while (NULL != fgets(pLineOrg, MAXLINELEN, fp)) {
                int equality = (0 == parm[V]);
                int match;
                line++;
                linelen = strlen(pLineOrg);

                
                for (i = 0; i < linelen; i++)
                    if (0 != parm[I])                            // case independance upcase
                        pLineUp[i] = toupper(pLineOrg[i]);
                    else
                        pLineUp[i] = pLineOrg[i];

                
                for (i = 0, match = 0; i + stringlen <= linelen; i++) 
                {
                    match = (0 == strncmp(&pLineUp[i], pString, stringlen));
                    if (match)
                        break;//for
                }
                if (match == equality) {
                    count++;
                    if (0 == parm[C]) {

                        if (0 != parm[N])                   // printf line number
                            printf("[%d]", line);
                        printf("%s", pLineOrg);
                    }
                }

            }
            if (0 != parm[C])
                printf("%d\n", count);
            printf("\n");
            if (stdin != fp)
                fclose(fp);                                                                     // close fp, that was opened above
        } while (++iargv < argc );                                                              // next file...
    } while (0);
    return 0;
}

