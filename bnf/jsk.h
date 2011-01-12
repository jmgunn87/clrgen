#ifndef JSK_H
#define JSK_H

#pragma once
#include "json_parser.h"
#include "skeleton_gen.h"

typedef enum _SKFMT_TYPE
{
  SKFMT_COMMENT_FMT=1,
  SKFMT_STACK_ACCESS_FMT=4,
  SKFMT_TBL_FMT=6,
  SKFMT_ACTION_FMT=11,
  SKFMT_UNKNOWN=13

} SKFMT_TYPE;


SKFMT_TYPE get_format_key_type(char* key);

int json_to_skeleton(slist* root,
                     skeleton_format* skfmt);

#endif