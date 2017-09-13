#include "4DPluginAPI.h"
#include "libjson.h"

void json_conv(const char *value, std::wstring &u32);

#if defined(__APPLE__)

#define DATE_FORMAT_ISO @"yyyy-MM-dd'T'HH:mm:ss"
#define DATE_FORMAT_ISO_GMT @"yyyy-MM-dd'T'HH:mm:ss'Z'"

NSString *json_conv(json_char *src);
NSString *json_copy_text(JSONNODE *n, const wchar_t *name);
void json_set_text(JSONNODE *n, NSString *value);
void json_set_text(JSONNODE *n, const wchar_t *name, NSString *value);
void json_conv(NSString *value, std::wstring &u32);
void json_set_text(JSONNODE *n, const wchar_t *name, NSString *value);
void json_set_path(JSONNODE *n, const wchar_t *name, NSString *value, BOOL isDirectory = FALSE);
#endif

void json_set_null(JSONNODE *n, const wchar_t *name);

void json_set_f(JSONNODE *n, const wchar_t *name, json_number value);
void json_set_i(JSONNODE *n, const wchar_t *name, json_int_t value);
void json_set_bool(JSONNODE *n, const wchar_t *name, json_bool_t value);

void json_set_text(JSONNODE *n, const wchar_t *name, char *value);
void json_set_text(JSONNODE *n, char *value);
void json_set_text(JSONNODE *n, const wchar_t *name, C_TEXT &t);

void json_stringify(JSONNODE *json, CUTF16String &t, BOOL pretty = NO);
void json_stringify(JSONNODE *json, C_TEXT &t, BOOL pretty = NO);
