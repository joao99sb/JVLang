#ifndef JVVM_UTILS
#define JVVM_UTILS
#include <stddef.h>

#define ARRAY_SIZE(X) (sizeof(X) / sizeof(X[0]))

typedef struct
{
  size_t count;
  const char *data;
} StringView;

StringView cstr_to_sv(const char *cstr);
StringView sv_trim_left(StringView sv);
StringView sv_trim_right(StringView sv);
StringView sv_chop(StringView *sv, char delim);
StringView sv_to_lower_case(StringView sv);
void sv_free(StringView sv);
int sv_eq(StringView a, StringView b);
int sv_to_int(StringView sv);
#endif