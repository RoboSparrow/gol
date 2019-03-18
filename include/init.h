/**
 * definitions for bootstrapping app
 */

#ifndef __INIT_H__
#define __INIT_H__

#include "utils.h"
#include "pattern.h"

void init_parse_args(int argc, char* argv[], Pattern *meta, Path patternfile);
void init_set_rootdir(int argc, char* argv[], Path path);
#endif
