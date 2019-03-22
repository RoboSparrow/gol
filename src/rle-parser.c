/**
 * RLE file parser
 * @see http://www.conwaylife.com/wiki/RLE
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>

#include "utils.h"
#include "gol.h"
#include "pattern.h"

#define RLE_CELL_DEAD 'b' /** defines value for a dead RLE cell */
#define RLE_CELL_ALIVE 'o' /** defines value for a living RLE cell */
#define RLE_EOROW '$' /** defines value for RLE line break */
#define RLE_EOPATT '!' /** defines value for RLE end of pattern */

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

    switch (flag) {
        case 'N':
            strcpy(pattern->title, str_trim(line));
            return 0;
        case 'C':
            // only consider first "c" line
            if (pattern->description[0] != '\0') {
                return 0;
            }
            strcpy(pattern->description, str_trim(line));
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
    str_nospaces(str_trim(line));

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
 * @param row RLE data fragment for a row, already split from data line by RLE_EOROW token
 * @param rowOffset row offset in context of data
 * @param pattern Pattern struct
 * @param data data matrix (1D char array)
 * @return 0 on success >= 1 if parsing error
 */
int parse_data_row(char *row, int rowOffset, Pattern *pattern) {

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

        if(row[i] == RLE_CELL_DEAD || row[i] == RLE_CELL_ALIVE) {
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
            char cell = (row[i] == RLE_CELL_DEAD) ? GOL_DEAD : GOL_ALIVE;
            // printf("---> %d -, amount %d: cell %d \n", index, amount, cell);
            for(int k = 0; k < amount; k++) {
                pattern->data[index] = cell;
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
 * @param pattern Pattern struct with alredy ready allocated data matrix filled with GOL_CELL_DEAD values (1D char array)
 * @return 0 on success >= 1 if parsing error
 */
int parse_data(FILE *fp, Pattern *pattern) {
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

        if(ch == RLE_EOROW || ch == RLE_EOPATT) {
            row[i] = '\0';
            parse_data_row(row, rowIndex, pattern);
            row[0] = '\0'; // clear
            i = 0;
            rowIndex++;
            // end of data
            if(ch == RLE_EOPATT) {
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
 * @param fp valid(!) file pointer to rle file
 * @param pattern Pattern struct
 * @return 0 on success >= 1 if parsing error
 */
int rle_load_meta(FILE *fp, Pattern *pattern) {
    // make sure we are on top of file
    rewind(fp);

    char identifier;
    for (int i = 0; 1; i++) {
        char line[LINE_BOUNDS];

        // EOF
        if (fgets(line, LINE_BOUNDS, fp) == NULL) {
            break;
        }

        identifier = tolower(line[0]);
        if (identifier == '#') {
            if (parse_comment(line, pattern)) {
                LOG_ERROR_F("rle-parser: Unable able to parse comment line (%d).", i);
            }
            continue;
        }

        // header line: there is eact 1 header line foolwed by data lines.
        // stop line reading and continue read data char-by-char
        if (identifier == 'x' || identifier == 'y') {
            if (parse_header(line, pattern)) {
                LOG_ERROR_F("rle-parser: Unable able to parse header line (%d).", i);
            }
            break;
        }
    }

    // entering data section
    if (isdigit(identifier) || identifier == RLE_CELL_ALIVE || identifier == RLE_CELL_DEAD) {
        fclose(fp);
        return 0;
    }

    return 0;
}

/**
 * Load RLE meta data into a given pattern struct
 * This function depends on a valid Pattern data, typically provided by rle_load_meta()
 * @see http://www.conwaylife.com/wiki/RLE
 * @param fp valid(!) file pointer to rle file
 * @param pattern Pattern struct, populated with valid values for these propetries
 *  - file"
 *  - cols"
 *  - rows"
 * @param data ready allocated data matrix filled with GOL_CELL_DEAD values (1D char array)
 * @return 0 on success >= 1 if parsing error
 */
int rle_load_data(FILE *fp, Pattern *pattern) {
    rewind (fp);// TODO this should not be neccessary

    int i = 0;
    char identifier;

    // scan through non-data lines
    for (i = 0; 1; i++) {
        // EOF
        char line[LINE_BOUNDS];

        if (fgets(line, LINE_BOUNDS, fp) == NULL) {
            break;
        }

        identifier = tolower(line[0]);
        if (identifier == '#') {
            continue;
        }

        // header line: there is eact 1 header line foolwed by data lines.
        // stop line reading and continue read data char-by-char
        if (identifier == 'x' || identifier == 'y') {
            break;
        }
    }

    if (parse_data(fp, pattern)) {
        LOG_ERROR("rle-parser: Unable able to parse data.");
        // exit(EXIT_FAILURE);
    }

    return 0;
}


pattern_state rle_load_pattern(char *file, Pattern *pattern, pattern_state targ_state) {
    // always
    strcpy(pattern->file, file);

    FILE *fp = fopen(file, "r");
    if (fp == NULL) {
        LOG_ERROR_F("Error while opening the file %s.", file);
        return PATTERN_NONE;
    }

    // ...meta
    int loaded  = rle_load_meta(fp, pattern);
    if(loaded < 0) {
        LOG_ERROR_F("error loading pattern file meta %s.", file);
        fclose(fp);
        return PATTERN_NONE;
    }
    pattern->state = PATTERN_META;

    if(targ_state == PATTERN_META) {
        return pattern->state;
    }

    // ...data
    pattern->data = gol_allocate_data(pattern->cols, pattern->rows);
    if(pattern->data == NULL) {
        LOG_ERROR("error allocating memory for data.");
        fclose(fp);
        return PATTERN_NONE;
    }

    int parsed = rle_load_data(fp, pattern);
    if(parsed < 0) {
        LOG_ERROR_F("error loading pattern file data %s.", file);
        fclose(fp);
        return PATTERN_NONE;
    }
    pattern->state = PATTERN_FULL;

    fclose(fp);
    return pattern->state;
}

int rle_save_pattern(Path file, Pattern *pattern) {

    FILE *fp = fopen(file, "w");

    if (fp == NULL) {
        LOG_ERROR("Error while opening the file.");
        return -1;
    }

    int cols  = pattern->cols;
    char *data  = pattern->data;

    fprintf(fp, "#N %s\n", pattern->title);
    fprintf(fp, "#C %s\n", pattern->description);
    fprintf(fp, "x = %d, y = %d\n", pattern->cols, pattern->rows);

    int i = 1;
    int size = pattern->cols * pattern->rows;
    char last = data[0];
    int count = 1;

    while (i < size) {
        if (data[i] != last) {
            char cell = (last == GOL_DEAD) ? RLE_CELL_DEAD : RLE_CELL_ALIVE;
            fprintf(fp, "%d%c", count, cell);

            last = data[i];
            count = 1;
        } else {
            count++;
        }
        // line end
        if(i % cols == 0) {
            if(data[i] == last) {
                char cell = (last == GOL_DEAD) ? RLE_CELL_DEAD : RLE_CELL_ALIVE;
                fprintf(fp, "%d%c", count, cell);
            }
            if(i < size - 1) {
                last = data[i + 1];
                count = 1;
                fprintf(fp, "%c", RLE_EOROW);
            }
        }
        i++;
    }
    fprintf(fp, "%c", RLE_EOPATT);
    fclose(fp);

    return 0;
}
