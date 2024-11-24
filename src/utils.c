#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include "utils.h"

StringView cstr_to_sv(const char *cstr)
{
  return (StringView){
      .count = strlen(cstr),
      .data = cstr,
  };
}

StringView sv_trim_left(StringView sv)
{
  size_t i = 0;
  while (i < sv.count && isspace(sv.data[i]))
  {
    i++;
  }
  return (StringView){
      .count = sv.count - i,
      .data = sv.data + i,
  };
}
StringView sv_trim_right(StringView sv)
{
  size_t i = 0;
  while (i < sv.count && isspace(sv.data[sv.count - 1 - i]))
  {
    i++;
  }
  return (StringView){
      .count = sv.count - i,
      .data = sv.data,
  };
}
StringView sv_trim(StringView sv)
{
  return sv_trim_right(sv_trim_left(sv));
}

StringView sv_chop(StringView *sv, char delim)
{
  size_t i = 0;
  while (i < sv->count && sv->data[i] != delim)
  {
    i++;
  }

  StringView result = {
      .count = i,
      .data = sv->data,
  };

  if (i < sv->count)
  {
    sv->count -= i + 1;
    sv->data += i + 1;
  }
  else
  {
    sv->count -= i;
    sv->data += i;
  }

  return result;
}

StringView sv_to_lower_case(StringView sv)
{

  char *result = (char *)malloc(sv.count);
  assert(result != NULL);
  for (size_t i = 0; i < sv.count; i++)
  {
    result[i] = tolower(sv.data[i]);
  }
  return (StringView){
      .count = sv.count,
      .data = result,
  };
}

void sv_free(StringView sv)
{

  free((void *)sv.data);
}

int sv_eq(StringView a, StringView b)
{
  if (a.count == b.count)
  {
    return memcmp(a.data, b.data, a.count) == 0;
  }

  return 0;
}

int sv_to_int(StringView sv)
{
  int result = 0;

  for (size_t i = 0; i < sv.count && isdigit(sv.data[i]); i++)
  {
    // ('5' - '0') = 5, this convert ASCII char to number
    result = result * 10 + (sv.data[i] - '0');
  }
  return result;
}