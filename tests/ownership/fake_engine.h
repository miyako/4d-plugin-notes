// Synthetic 4D engine behind the real SDK's gCall4D boundary.
#pragma once
#include "4DPluginAPI.h"
#include <string>
#include <vector>

namespace fake4d {
struct Obj;
void install();
void reset();
Obj *new_object(bool collection = false);
void release(Obj *o);
int refcount(Obj *o);
void put_real(Obj *o, const char *key, double v);
void put_bool(Obj *o, const char *key, bool v);
void put_string(Obj *o, const char *key, const char *v);
void put_date(Obj *o, const char *key, short day, short month, short year);
void put_ref(Obj *o, const char *key, Obj *child); // retains child
void push_real(Obj *c, double v);
void push_string(Obj *c, const char *v);
void push_ref(Obj *c, Obj *child); // retains child
bool get_string(Obj *o, const char *key, std::string *out);
bool has_picture(Obj *o, const char *key);
size_t live_strings();
size_t live_handles();
size_t live_objects();
size_t live_pictures();
int picture_refcount(PA_Picture p);
// CONVERT PICTURE leaves the picture unchanged (models a failed conversion)
void set_convert_noop(bool noop);
int converts();
// next N element/property reads (EX_GET_OBJ_VALUE, not "length") fail and
// write a poison value the plugin must neither use nor clear
void fail_next_gets(int n, bool elements_only = false);
// method lookup / execution model for listenerLoopExecuteMethod
void set_method(const char *name, PA_long32 id, const char *result /* NULL: no return value */);
int method_calls();
const std::string &last_execute_method_name();
const std::vector<std::string> &violations();
}
