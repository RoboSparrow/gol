/**
 * CELLS file parser
 * @see http://www.conwaylife.com/wiki/Plaintext
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>

#include "utils.h"
#include "gol.h"
#include "pattern.h"

#define CELL_DEAD '.' /** defines value for a dead cell */
#define CELL_ALIVE 'O' /** defines value for a living  cell */
#define CELL_COMMENT_FLAG '!' /** defines value for comment line */

/**
 * counts the occurence of cell chars in a line
 */
int cell_data_line_width(char *line) {
    size_t len = strlen(line);
    int count = 0;
    for(int i = 0; i < len; i++) {
        if (line[i] == CELL_DEAD || line[i] == CELL_ALIVE) {
            count++;
        }
    }
    return count;
}

/**
 * parses an CELL data row and adds parsed row to data matrix
 * @see http://www.conwaylife.com/wiki/Plaintext
 *
 * @param row RLE data fragment for a row, already split from data line by RLE_EOROW token
 * @param rowOffset row offset in context of data
 * @param pattern Pattern struct
 */
void cell_parse_data_line(char *line, int rowOffset, Pattern *pattern) {
        int cols = pattern->cols;
        char *data = pattern->data;

        int index = rowOffset * cols;

        // // blank line: if compressed, blank lines in between data rows indicate rows entirely filled with dead cells
        // if (line[0] != CELL_DEAD && line[0] != CELL_ALIVE) {
        //     for (int p = 0; p < cols; p++) {
        //         data[index] = GOL_DEAD;
        //         index++;
        //     }
        //     return;
        // }

        // populated line

        size_t len = strlen(line);
        int hits = 0;

        // a line can betrunctated (i.e. "..0" instead "..0..")

        for (int k = 0; k < len; k++) {
            if (line[k] == CELL_DEAD || line[k] == CELL_ALIVE) {
                data[index] = (line[k] == CELL_ALIVE) ? GOL_ALIVE : GOL_DEAD;
                hits++;
                index++;
            }
        }

        // compressed: fill remainder with empty cells
        if(hits < cols) {
            int rem = cols - hits;
            for (int n = 0; n < rem; n++) {
                data[index] = GOL_DEAD;
                index++;
            }
        }
}

/**
 * parses CELL datastring into a prepared data matrix
 * @see http://www.conwaylife.com/wiki/Plaintext
 *
 * @param fp FILE pointer , aleady set to start of data section!
 * @param pattern Pattern struct with alredy ready allocated data matrix filled with GOL_CELL_DEAD values (1D char array)
 * @return 0 on success >= 1 if parsing error
 */
int cell_parse_data(char *currentLine, FILE *fp, Pattern *pattern){
    int rows = pattern->rows;

    cell_parse_data_line(currentLine, 0, pattern);

    // it is important to stick to the rows limit instead reading any remaining lines.
    // This way we guard against memory errors in "dirty" files
    for (int i = 1; i < rows; i++) {
        char line[LINE_BOUNDS];

        //EOL
        if (fgets(line, LINE_BOUNDS, fp) == NULL) {
            break;
        }

        cell_parse_data_line(line, i, pattern);
    }

    return 0;
}

int cell_load_meta(FILE *fp, Pattern *pattern){
    char title[LINE_BOUNDS] = { '\0' };
    char description[LINE_BOUNDS] = { '\0' };

    const char comment_delimiter = ':';

    // make sure we are on top of file
    rewind(fp);
    int cols = 0;
    int rows = 0;
    char line[LINE_BOUNDS];

    int isDataSection = 0; // set if datasection entered
    int blankLines = 0; // trailing blank (non-data lines)

    for (int i = 0; 1; i++) {
        //EOL
        if (fgets(line, LINE_BOUNDS, fp) == NULL) {
            break;
        }

        // ..comments
        if (line[0] == '!') {
            char *str = line;
            str++; // skip "!"
            char *chr = strchr(str, comment_delimiter);
            if (chr != NULL) {
                chr += 1;
            }
            if(i == 0) {
                strcpy(title, (chr != NULL) ? chr : str);
            }
            if(i == 1) {
                strcpy(description, str);
            }
        }

        // ..data
        if (line[0] == CELL_DEAD || line[0] == CELL_ALIVE) {
            // reset blank (non-data lines),
            // this will help to identify trailng line breaks and remove them from row count
            blankLines = 0;
            isDataSection = 1;
        }

        if(isDataSection) {
            // count blank (non-data lines)
            if (line[0] != CELL_DEAD && line[0] != CELL_ALIVE) {
                blankLines++;
            }

            int len = cell_data_line_width(line);
            // a line can betrunctated (i.e. "..0" instead "..0..")
            if (len > cols) {
                cols = len;
            }

            rows ++;
        }

    }

    //todo check if empty
    strcpy(pattern->title, str_trim(title));
    strcpy(pattern->description, str_trim(description));
    pattern->cols = cols;
    pattern->rows = rows - blankLines;

    return 0;
}

/**
 * Load CELL meta data into a given pattern struct
 * This function depends on a valid Pattern data, typically provided by rle_load_meta()
 * @see http://www.conwaylife.com/wiki/Plaintext
 * @param fp valid(!) file pointer to rle file
 * @param pattern Pattern struct, populated with valid values for these propetries
 *  - file"
 *  - cols"
 *  - rows"
 * @param data ready allocated data matrix filled with GOL_CELL_DEAD values (1D char array)
 * @return 0 on success >= 1 if parsing error
 */
int cell_load_data(FILE *fp, Pattern *pattern) {
    rewind (fp); // TODO use fseek, this should not be neccessary

    int i = 0;

    // guard
    if (pattern->state < PATTERN_META) {
         LOG_ERROR("error loading pattern data before loading meta meta.");
         fclose(fp);
         return -1;
    }

    // scan through non-data lines
    for (i = 0; 1; i++) {
        char line[LINE_BOUNDS];
        //EOF
        if (fgets(line, LINE_BOUNDS, fp) == NULL) {
            break;
        }
        // start of data section
        if (line[0] == CELL_DEAD || line[0] == CELL_ALIVE) {
            if (cell_parse_data(line, fp, pattern)) {
                LOG_ERROR("rle-parser: Unable able to parse data.");
                // exit(EXIT_FAILURE);
            }
            break;
        }
    }
    return 0;
}


pattern_state cell_load_pattern(char *file, Pattern *pattern, pattern_state targ_state) {
    // always
    strcpy(pattern->file, file);

    FILE *fp = fopen(file, "r");
    if (fp == NULL) {
        LOG_ERROR_F("Error while opening the file %s.", file);
        return PATTERN_NONE;
    }

    // ...meta
    int loaded  = cell_load_meta(fp, pattern);
    if(loaded < 0) {
        LOG_ERROR_F("error loading pattern file meta %s.", file);
        fclose(fp);
        return PATTERN_NONE;
    }
    pattern->state = PATTERN_META;

    if(targ_state == PATTERN_META) {
        fclose(fp);
        return pattern->state;
    }

    // ...data
    pattern->data = gol_allocate_data(pattern->data, pattern->cols, pattern->rows);
    if(pattern->data == NULL) {
        LOG_ERROR("error allocating memory for data.");
        fclose(fp);
        return PATTERN_NONE;
    }

    int parsed = cell_load_data(fp, pattern);
    if(parsed < 0) {
        LOG_ERROR_F("error loading pattern file data %s.", file);
        fclose(fp);
        return PATTERN_NONE;
    }
    pattern->state = PATTERN_FULL;

    fclose(fp);
    return pattern->state;
}

// TODO "compress", remove trailing empty cell sequences
int cell_save_pattern(Path file, Pattern *pattern) {

    FILE *fp = fopen(file, "w");

    if (fp == NULL) {
        LOG_ERROR("Error while opening the file.");
        return -1;
    }

    int cols  = pattern->cols;
    char *data  = pattern->data;

    fprintf(fp, "!Name: %s\n", pattern->title);
    fprintf(fp, "!%s\n", pattern->description);

    int size = pattern->cols * pattern->rows;
    int i = 0;

    while (i < size) {
        // line end
        if (i && i % cols == 0) {
            fprintf(fp, "\n");
        }
        fprintf(fp, "%c", (data[i] == GOL_ALIVE) ? CELL_ALIVE : CELL_DEAD);
        i++;
    }

    fclose(fp);

    return 0;
}
