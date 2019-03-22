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

int cell_load_meta(FILE *fp, Pattern *pattern){
    char title[LINE_BOUNDS] = { '\0' };
    char description[LINE_BOUNDS] = { '\0' };

    const char comment_delimiter = ':';

    // make sure we are on top of file
    rewind(fp);
    int cols = 0;
    int rows = 0;
    char line[LINE_BOUNDS];

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
    pattern->rows = rows;

    return 0;
}

int cell_load_data(FILE *fp, Pattern *pattern){
    // make sure we are on top of file
    rewind(fp);

    int index = 0;
    int cols = pattern->cols;
    char *data = pattern->data;

    for (int i = 0; 1; i++) {
        char line[LINE_BOUNDS];

        //EOL
        if (fgets(line, LINE_BOUNDS, fp) == NULL) {
            break;
        }

        // ..data ine
        if (line[0] == CELL_DEAD || line[0] == CELL_ALIVE) {
            int len = strlen(line);
            int hits = 0;
            // a line can betrunctated (i.e. "..0" instead "..0..")
            for (int k = 0; k < len; k++) {
                if (line[k] == CELL_DEAD || line[k] == CELL_ALIVE) {
                    data[index] = (line[k] == CELL_ALIVE) ? GOL_ALIVE : GOL_DEAD;
                    hits++;
                    index++;
                }
            }
            // fill remainder with empty cells
            if(hits < cols) {
                int rem = cols - hits;
                for (int n = 0; n < rem; n++) {
                    data[index] = GOL_DEAD;
                    index++;
                }
            }
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
        return pattern->state;
    }

    // ...data
    pattern->data = gol_allocate_data(pattern->cols, pattern->rows);
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
