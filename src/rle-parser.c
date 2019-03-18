/**
 * RLE file parser
 * @see http://www.conwaylife.com/wiki/RLE
 *
 * The first line is a header line, which has the form x = <width>, y = <height>
 * encoded as a sequence of items of the form <run_i><tag>, where <run_i> is the number of occurrences of <tag> and <tag>
 *
 * <run_i> can be omitted if it is equal to 1. The last <run_i><tag> item is followed by a ! character.
 * Dead cells at the end of a pattern line do not need to be encoded, nor does the end of the last line of the pattern.
 * Whitespace is permitted between <run_i><tag> items (and between the last <run_i><tag> item and the following !),
 * but except for carriage returns and line feeds this is not recommended.
 *
 * It is not permitted to place whitespace in the middle of a <run_i><tag> item.
 * Lines in the RLE file must not exceed 70 characters, although it is a good idea for RLE readers to be able to cope with longer lines.
 * DOS, Unix and Mac newline conventions are all acceptable.
 * Anything after the final ! is ignored. It used to be common to put comments here (starting on a new line),
 * but the usual method for adding comments is now by means of #C lines (see below).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>

#include "utils.h"
#include "gol.h"
#include "pattern.h"

#define REL_CELL_DEAD 'b' /** defines value for a dead RLE cell */
#define REL_CELL_ALIVE 'o' /** defines value for a living RLE cell */
#define REL_EOROW '$' /** defines value for RLE line break */
#define REL_EOPATT '!' /** defines value for RLE end of pattern */
#define REL_COMMENT_FLAG '#' /** defines value for RLE end of pattern */

/**
 * parses text data from a comment line into pattern, if certain RLE comment flags are matching pattern properties
 * @see http://www.conwaylife.com/wiki/RLE
 *
 * @param line
 * @param pattern Pattern struct
 * @return 0 on success >= 1 if parsing error
 */
int parse_comment(char *line, Pattern *pattern) {

    char flag = toupper(line[1]);

    // Example: #N My awesome pattern
    if (flag != 'N' &&  flag != 'C') {
        return 0;
    }

    if (line[2] != ' ') {
        return 0;
    }

    line += 3;
    str_trim(line);

    switch (flag) {
        case 'N':
            strcpy(pattern->title, line);
            return 0;
        case 'C':
            // only consider first "c" line
            if (pattern->description[0] != '\0') {
                return 0;
            }
            strcpy(pattern->description, line);
            return 0;
    }

    return 0; //err parsing
}

/**
 * parses row and column values from a headerline
 * @see http://www.conwaylife.com/wiki/RLE
 *
 * @param line
 * @param pattern Pattern struct
 * @return 0 on success >= 1 if parsing error
 */
int parse_header(char *line, Pattern *pattern) {
    str_trim(line);

    str_nospaces(line);

    // split by comma
    int i = 0;
    char *p;
    char *key = 0;

    p = strtok(line, ",=");
    while (p != NULL) {

        if(i % 2 == 0) {
            // key
            str_tolower(p);
            key = p;
        } else {
            // value
            if (strcmp(key, "x") == 0 || strcmp(key, "Y") == 0)  {
                pattern->cols = atoi(p);
            }

            if (strcmp(key, "y") == 0 || strcmp(key, "Y") == 0)  {
                pattern->rows = atoi(p);
            }

            key = 0;
        }
        p = strtok(NULL, ",=");
        i++;
    }

    return 0; //err parsing
}

/**
 * parses an RLE data row and adds parsed row to data matrix
 * @see http://www.conwaylife.com/wiki/RLE
 *
 * @param row RLE data fragment for a row, already split from data line by REL_EOROW token
 * @param rowOffset row offset in context of data
 * @param pattern Pattern struct
 * @param data data matrix (1D char array)
 * @return 0 on success >= 1 if parsing error
 */
int parse_data_row(char *row, int rowOffset, Pattern *pattern, char *data) {

    int index = rowOffset * pattern->cols;
    int len = strlen(row);
    // printf("row: %s, rowOffset: %d, index %d, length: %d\n", row, rowOffset, index, len);

    char digits[len];
    int digitIndex = 0;

    size_t i = 0;
    while (row[i] != '\0') {

        if(isdigit(row[i])) {
            digits[digitIndex] = row[i];
            digitIndex++;
        }

        if(row[i] == REL_CELL_DEAD || row[i] == REL_CELL_ALIVE) {
            //parse previously allocated digits
            int amount = 1;
            if(digitIndex > 0) {
                // parse
                digits[digitIndex++] = '\0';
                amount = atoi(digits);

                // reset
                digits[0] = '\0';
                digitIndex = 0;
            }

            // write group
            char cell = (row[i] == REL_CELL_DEAD) ? GOL_DEAD : GOL_ALIVE;
            // printf("---> %d -, amount %d: cell %d \n", index, amount, cell);
            for(int k = 0; k < amount; k++) {
                data[index] = cell;
                //printf("#%d: %c %d\n", index, cell, pattern->data[index]);
                index++;
            }

        }

        i++;
    }

    return 0;
}

/**
 * parses RLE datastring into a prepared data matrix
 * @see http://www.conwaylife.com/wiki/RLE
 *
 * @param fp FILE pointer
 * @param pattern Pattern struct
 * @param data ready allocated data matrix filled with GOL_CELL_DEAD values (1D char array)
 * @return 0 on success >= 1 if parsing error
 */
int parse_data(FILE *fp, Pattern *pattern, char *data) {
    //int len = pattern->rows * pattern->cols;
    int i = 0;
    int rowIndex = 0;
    char ch;

    char row[pattern->cols + 1];

    while(1) {
        ch = fgetc(fp);

        if(feof(fp)) {
            break ;
        }

        if(ch == REL_EOROW || ch == REL_EOPATT) {
            row[i] = '\0';
            parse_data_row(row, rowIndex, pattern, data);
            row[0] = '\0'; // clear
            i = 0;
            rowIndex++;
            // end of data
            if(ch == REL_EOPATT) {
                break;
            }
            continue;
        };

        row[i] = ch;
        i++;

    }

    return 0;
}

////
// main functions for file parsing
////

/**
 * Load RLE meta data into a given pattern struct
 * You must call this before calling rle_load_data()
 * @see http://www.conwaylife.com/wiki/RLE
 *
 * @param filepath path to RLE file
 * @param pattern Pattern struct
 * @return 0 on success >= 1 if parsing error
 */
int rle_load_meta(char *filepath, Pattern *pattern) {
    FILE *fp;

    fp = fopen(filepath, "r");

    if (fp == NULL) {
        perror("Error while opening the file.\n");
        return -1;
    }

    // store filepath
    strcpy(pattern->file, filepath);
    char identifier;
    for (int i = 0; 1; i++) {

        // EOF
        char line[LINE_BOUNDS];

        if (fgets(line, LINE_BOUNDS, fp) == NULL) {
            break;
        }

        identifier = tolower(line[0]);

        if (identifier == REL_COMMENT_FLAG) {
            if (parse_comment(line, pattern)) {
                fprintf(stderr, "rle-parser: Unable able to parse comment line (%d).\n", i);
            }
            continue;
        }

        // header line: there is eact 1 header line foolwed by data lines.
        // stop line reading and continue read data char-by-char
        if (identifier == 'x') {
            if (parse_header(line, pattern)) {
                fprintf(stderr, "rle-parser: Unable able to parse header line (%d).\n", i);
            }

            break;
        }
    }

    // entering data section
    if (isdigit(identifier) || identifier == REL_CELL_ALIVE || identifier == REL_CELL_DEAD) {
        printf("entering data, skip");
        fclose(fp);
        return 0;
    }

    fclose(fp);
    return 0;
}

/**
 * Load RLE meta data into a given pattern struct
 * This function depends on a valid Pattern data, typically provided by rle_load_meta()
 * @see http://www.conwaylife.com/wiki/RLE
 * @param pattern Pattern struct, populated with valid values for these propetries
 *  - file"
 *  - cols"
 *  - rows"
 * @param data ready allocated data matrix filled with GOL_CELL_DEAD values (1D char array)
 * @return 0 on success >= 1 if parsing error
 */
int rle_load_data(Pattern *pattern, char *data) {

    FILE *fp;

    fp = fopen(pattern->file, "r");

    if (fp == NULL) {

        fprintf(stderr, "Error while opening the file.\n");
        return -1;
    }

    int i = 0;
    char identifier;
    for (i = 0; 1; i++) {

        // EOF
        char line[LINE_BOUNDS];

        if (fgets(line, LINE_BOUNDS, fp) == NULL) {
            break;
        }

        identifier = tolower(line[0]);

        if (identifier == REL_COMMENT_FLAG) {
            continue;
        }

        // header line: there is eact 1 header line foolwed by data lines.
        // stop line reading and continue read data char-by-char
        if (identifier == 'x') {
            break;
        }
    }

    if (parse_data(fp, pattern, data)) {
        fprintf(stderr, "rle-parser: Unable able to parse data\n");
        // exit(EXIT_FAILURE);
    }

    fclose(fp);

    return 0;
}


char *rle_load_pattern(char *file, Pattern *pattern) {

    int loaded  = rle_load_meta(file, pattern);
    if(loaded < 0) {
        fprintf(stderr,"error loading pattern file meta %s\n", file);
        return NULL;
    }

    char *data = gol_allocate_data(pattern->cols, pattern->rows);
    if(data == NULL) {
        fprintf(stderr, "pattern_merge: error allocating memory for data");
        return NULL;
    }

    int parsed = rle_load_data(pattern, data);
    if(parsed < 0) {
        fprintf(stderr, "error loading pattern file data %s\n", file);
        return NULL;
    }

    return data;
}
