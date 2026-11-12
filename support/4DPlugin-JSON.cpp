#include "4DPlugin-JSON.h"
#include <climits>
#include <cstring>

void json_wconv(const wchar_t *value, CUTF16String *u16) {

    if(!value) return;

    size_t wlen = wcslen(value);

#if VERSIONWIN
    *u16 = CUTF16String((const PA_Unichar *)value, wlen);
#else
    CFStringRef str = CFStringCreateWithBytes(kCFAllocatorDefault, (const UInt8 *)value, wlen*sizeof(wchar_t), kCFStringEncodingUTF32LE, true);
    if(str)
    {
        CFIndex len = CFStringGetLength(str);
        std::vector<uint8_t> buf((len+1) * sizeof(PA_Unichar));
        CFStringGetCharacters(str, CFRangeMake(0, len), (UniChar *)&buf[0]);
        *u16 = CUTF16String((const PA_Unichar *)&buf[0], len);
        CFRelease(str);
    }
#endif
}

// Single UTF-8 -> UTF-16 conversion shared by every narrow-string setter.
// Returns false on invalid UTF-8 on BOTH platforms: CFStringCreateWithBytes
// already fails on macOS, and MB_ERR_INVALID_CHARS makes Windows fail the
// same way instead of silently substituting U+FFFD. An empty input is a
// successful conversion to an empty string (MultiByteToWideChar would
// otherwise report 0 = failure for a zero-length input).
static bool json_u8_to_u16(const char *u8, CUTF16String *u16) {

    u16->clear();

    if(!u8) return false;

    size_t u8len = strlen(u8);
    if(u8len == 0) return true;

#ifdef _WIN32
    if(u8len > INT_MAX) return false;
    int len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, (LPCSTR)u8, (int)u8len, NULL, 0);
    if(len <= 0) return false;
    std::vector<uint8_t> buf((len + 1) * sizeof(PA_Unichar));
    if(!MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, (LPCSTR)u8, (int)u8len, (LPWSTR)&buf[0], len)) return false;
    *u16 = CUTF16String((const PA_Unichar *)&buf[0], (size_t)len);
    return true;
#else
    CFStringRef str = CFStringCreateWithBytes(kCFAllocatorDefault, (const UInt8 *)u8, (CFIndex)u8len, kCFStringEncodingUTF8, false);
    if(!str) return false;
    CFIndex len = CFStringGetLength(str);
    std::vector<uint8_t> buf((len+1) * sizeof(PA_Unichar));
    CFStringGetCharacters(str, CFRangeMake(0, len), (UniChar *)&buf[0]);
    *u16 = CUTF16String((const PA_Unichar *)&buf[0], (size_t)len);
    CFRelease(str);
    return true;
#endif
}

// Shared tail of every setter: key -> Unistring, set, dispose key, clear v.
// v must already hold its value; PA_SetObjectProperty stores it and
// PA_ClearVariable releases the plugin's own reference.
static void json_set_property(PA_ObjectRef obj, const CUTF16String *ukey, PA_Variable *v) {

    PA_Unistring key = PA_CreateUnistring((PA_Unichar *)ukey->c_str());
    PA_SetObjectProperty(obj, &key, *v);
    PA_DisposeUnistring(&key);
    PA_ClearVariable(v);
}

// Builds a string variable from UTF-16 text. Starts from eVK_Undefined, not
// eVK_Unistring: PA_CreateVariable(eVK_Unistring) allocates an empty
// Unistring that PA_SetStringVariable then overwrites without disposing.
// eVK_Undefined allocates nothing, and PA_SetStringVariable sets fType itself.
// Ownership of the new Unistring passes to the variable (freed by PA_ClearVariable).
static PA_Variable json_string_variable(const CUTF16String *u16) {

    PA_Variable v = PA_CreateVariable(eVK_Undefined);
    PA_Unistring value = PA_CreateUnistring((PA_Unichar *)u16->c_str());
    PA_SetStringVariable(&v, &value);
    return v;
}

void ob_set_p(PA_ObjectRef obj, const wchar_t *_key, PA_Picture value) {

    if(obj && _key && value)
    {
        // eVK_Undefined, not eVK_Picture: PA_CreateVariable(eVK_Picture)
        // allocates an empty picture that PA_SetPictureVariable would overwrite.
        PA_Variable v = PA_CreateVariable(eVK_Undefined);
        CUTF16String ukey;
        json_wconv(_key, &ukey);

        PA_SetPictureVariable(&v, value);
        json_set_property(obj, &ukey, &v);
    }
}

void ob_set_s(PA_ObjectRef obj, const char *_key, const char *_value) {

    if(obj && _key && _value)
    {
        CUTF16String u16k, u16v;

        // invalid UTF-8 key: skip, rather than write under "" and collide
        if(!json_u8_to_u16(_key, &u16k)) return;
        // invalid UTF-8 value: empty string on both platforms
        json_u8_to_u16(_value, &u16v);

        PA_Variable v = json_string_variable(&u16v);
        json_set_property(obj, &u16k, &v);
    }
}

void ob_set_s(PA_ObjectRef obj, const wchar_t *_key, const char *_value) {

    if(obj && _key && _value)
    {
        CUTF16String ukey, u16;
        json_wconv(_key, &ukey);
        // invalid UTF-8 value: empty string on both platforms
        json_u8_to_u16(_value, &u16);

        PA_Variable v = json_string_variable(&u16);
        json_set_property(obj, &ukey, &v);
    }
}

void ob_set_a(PA_ObjectRef obj, const wchar_t *_key, const wchar_t *_value) {

    if(obj && _key && _value)
    {
        CUTF16String ukey;
        CUTF16String uvalue;
        json_wconv(_key, &ukey);
        json_wconv(_value, &uvalue);

        PA_Variable v = json_string_variable(&uvalue);
        json_set_property(obj, &ukey, &v);
    }
}

void ob_set_a(PA_ObjectRef obj, const wchar_t *_key, const CUTF16String *_value) {

    if(obj && _key && _value)
    {
        // _value is already a native UTF-16 (PA_Unichar) buffer with a known
        // length -- do NOT route it through json_wconv/wcslen the way _key
        // is below. json_wconv assumes its input is a genuine platform
        // wchar_t buffer (UTF-32 on macOS), so reinterpreting a CUTF16String's
        // UTF-16 data as wchar_t there would misread the buffer width and
        // corrupt or over-read it. Only _key -- a real wchar_t string
        // literal from the caller -- goes through the normal conversion.
        CUTF16String ukey;
        json_wconv(_key, &ukey);

        PA_Variable v = json_string_variable(_value);
        json_set_property(obj, &ukey, &v);
    }
}

void ob_set_o(PA_ObjectRef obj, const wchar_t *_key, PA_ObjectRef value) {

    if(obj && _key && value)
    {
        PA_Variable v = PA_CreateVariable(eVK_Object);
        CUTF16String ukey;
        json_wconv(_key, &ukey);

        PA_SetObjectVariable(&v, value);
        json_set_property(obj, &ukey, &v);
    }
}

void ob_set_o(PA_ObjectRef obj, const char *_key, PA_ObjectRef value) {

    if(obj && _key && value)
    {
        CUTF16String u16k;
        if(!json_u8_to_u16(_key, &u16k)) return;

        PA_Variable v = PA_CreateVariable(eVK_Object);
        PA_SetObjectVariable(&v, value);
        json_set_property(obj, &u16k, &v);
    }
}

void ob_set_c(PA_ObjectRef obj, const wchar_t *_key, PA_CollectionRef value) {

    if(obj && _key && value)
    {
        PA_Variable v = PA_CreateVariable(eVK_Collection);
        CUTF16String ukey;
        json_wconv(_key, &ukey);

        PA_SetCollectionVariable(&v, value);
        json_set_property(obj, &ukey, &v);
    }
}

void ob_set_n(PA_ObjectRef obj, const wchar_t *_key, double value) {

    if(obj && _key)
    {
        PA_Variable v = PA_CreateVariable(eVK_Real);
        CUTF16String ukey;
        json_wconv(_key, &ukey);

        PA_SetRealVariable(&v, value);
        json_set_property(obj, &ukey, &v);
    }
}

void ob_set_n(PA_ObjectRef obj, const char *_key, double value) {

    if(obj && _key)
    {
        CUTF16String u16k;
        if(!json_u8_to_u16(_key, &u16k)) return;

        PA_Variable v = PA_CreateVariable(eVK_Real);
        PA_SetRealVariable(&v, value);
        json_set_property(obj, &u16k, &v);
    }
}

void ob_set_i(PA_ObjectRef obj, const wchar_t *_key, PA_long32 value) {

    if(obj && _key)
    {
        PA_Variable v = PA_CreateVariable(eVK_Longint);
        CUTF16String ukey;
        json_wconv(_key, &ukey);

        PA_SetLongintVariable(&v, value);
        json_set_property(obj, &ukey, &v);
    }
}

void ob_set_b(PA_ObjectRef obj, const wchar_t *_key, bool value) {

    if(obj && _key)
    {
        PA_Variable v = PA_CreateVariable(eVK_Boolean);
        CUTF16String ukey;
        json_wconv(_key, &ukey);

        PA_SetBooleanVariable(&v, value);
        json_set_property(obj, &ukey, &v);
    }
}

void ob_set_0(PA_ObjectRef obj, const wchar_t *_key) {

    if(obj && _key)
    {
        // eVK_Null has no dedicated case in PA_CreateVariable's switch (confirmed
        // by reading its implementation): it only assigns variable.fType, no
        // union payload and no heap allocation for this kind, so there's no
        // PA_SetXVariable call to make before PA_SetObjectProperty.
        PA_Variable v = PA_CreateVariable(eVK_Null);
        CUTF16String ukey;
        json_wconv(_key, &ukey);

        json_set_property(obj, &ukey, &v);
    }
}

void ob_set_0(PA_ObjectRef obj, const char *_key) {

    if(obj && _key)
    {
        CUTF16String u16k;
        if(!json_u8_to_u16(_key, &u16k)) return;

        PA_Variable v = PA_CreateVariable(eVK_Null);
        json_set_property(obj, &u16k, &v);
    }
}

bool ob_is_defined(PA_ObjectRef obj, const wchar_t *_key) {

    bool is_defined = false;

    if(obj && _key)
    {
        CUTF16String ukey;
        json_wconv(_key, &ukey);
        PA_Unistring key = PA_CreateUnistring((PA_Unichar *)ukey.c_str());

        is_defined = PA_HasObjectProperty(obj, &key);

        PA_DisposeUnistring(&key);
    }
    return is_defined;
}

bool ob_get_a(PA_ObjectRef obj, const wchar_t *_key, CUTF8String *value) {

    bool is_defined = false;

    if(obj && _key)
    {
        CUTF16String ukey;
        json_wconv(_key, &ukey);
        PA_Unistring key = PA_CreateUnistring((PA_Unichar *)ukey.c_str());
        is_defined = PA_HasObjectProperty(obj, &key);

        if(is_defined)
        {
            PA_Variable v = PA_GetObjectProperty(obj, &key);
            bool got = (PA_GetLastError() == eER_NoErr);
            if(PA_GetVariableKind(v) == eVK_Unistring)
            {
                PA_Unistring uvalue = PA_GetStringVariable(v);

                CUTF16String u = CUTF16String(uvalue.fString, uvalue.fLength);
#ifdef _WIN32
                int len = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)u.c_str(), u.length(), NULL, 0, NULL, NULL);

                if(len){
                    std::vector<uint8_t> buf(len + 1);
                    if(WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)u.c_str(), u.length(), (LPSTR)&buf[0], len, NULL, NULL)){
                        *value = CUTF8String((const uint8_t *)&buf[0], (size_t)len);
                    }
                }else{
                    *value = CUTF8String((const uint8_t *)"");
                }

#else
                CFStringRef str = CFStringCreateWithCharacters(kCFAllocatorDefault, (const UniChar *)u.c_str(), u.length());
                if(str){

                    size_t size = CFStringGetMaximumSizeForEncoding(
                                                                    CFStringGetLength(str),
                                                                    kCFStringEncodingUTF8) + sizeof(uint8_t);
                    std::vector<uint8_t> buf(size);
                    CFIndex len = 0;
                    CFStringGetBytes(str,
                                     CFRangeMake(
                                                 0,
                                                 CFStringGetLength(str)),
                                     kCFStringEncodingUTF8,
                                     0, true, (UInt8 *)&buf[0], size, &len);

                    *value = CUTF8String((const uint8_t *)&buf[0], len);
                    CFRelease(str);
                }
#endif
            }
            // PA_GetObjectProperty returns an owned copy
            if(got) PA_ClearVariable(&v);
        }

        PA_DisposeUnistring(&key);
    }

    return is_defined;
}

bool ob_get_b(PA_ObjectRef obj, const wchar_t *_key) {

    bool value = false;

    if(obj && _key)
    {
        CUTF16String ukey;
        json_wconv(_key, &ukey);
        PA_Unistring key = PA_CreateUnistring((PA_Unichar *)ukey.c_str());

        if(PA_HasObjectProperty(obj, &key))
        {
            PA_Variable v = PA_GetObjectProperty(obj, &key);
            if(PA_GetVariableKind(v) == eVK_Boolean)
            {
                value = PA_GetBooleanVariable(v);
            }
        }

        PA_DisposeUnistring(&key);
    }

    return value;
}

double ob_get_n(PA_ObjectRef obj, const wchar_t *_key) {

    double value = 0.0;

    if(obj && _key)
    {
        CUTF16String ukey;
        json_wconv(_key, &ukey);
        PA_Unistring key = PA_CreateUnistring((PA_Unichar *)ukey.c_str());

        if(PA_HasObjectProperty(obj, &key))
        {
            PA_Variable v = PA_GetObjectProperty(obj, &key);
            if(PA_GetVariableKind(v) == eVK_Real)
            {
                value = PA_GetRealVariable(v);
            }
        }

        PA_DisposeUnistring(&key);
    }

    return value;
}

PA_CollectionRef ob_get_c(PA_ObjectRef obj, const wchar_t *_key) {

    PA_CollectionRef value = NULL;

    if(obj && _key)
    {
        CUTF16String ukey;
        json_wconv(_key, &ukey);
        PA_Unistring key = PA_CreateUnistring((PA_Unichar *)ukey.c_str());

        if(PA_HasObjectProperty(obj, &key))
        {
            PA_Variable v = PA_GetObjectProperty(obj, &key);
            bool got = (PA_GetLastError() == eER_NoErr);
            if(PA_GetVariableKind(v) == eVK_Collection)
            {
                value = PA_GetCollectionVariable(v);// still referenced by obj after the clear below
            }
            // PA_GetObjectProperty returns a retained copy
            if(got) PA_ClearVariable(&v);
        }

        PA_DisposeUnistring(&key);
    }
    return value;
}

PA_ObjectRef ob_get_o(PA_ObjectRef obj, const wchar_t *_key) {

    PA_ObjectRef value = NULL;

    if(obj && _key)
    {
        CUTF16String ukey;
        json_wconv(_key, &ukey);
        PA_Unistring key = PA_CreateUnistring((PA_Unichar *)ukey.c_str());

        if(PA_HasObjectProperty(obj, &key))
        {
            PA_Variable v = PA_GetObjectProperty(obj, &key);
            bool got = (PA_GetLastError() == eER_NoErr);
            if(PA_GetVariableKind(v) == eVK_Object)
            {
                value = PA_GetObjectVariable(v);// still referenced by obj after the clear below
            }
            // PA_GetObjectProperty returns a retained copy
            if(got) PA_ClearVariable(&v);
        }

        PA_DisposeUnistring(&key);
    }
    return value;
}

void ob_stringify(PA_ObjectRef obj, CUTF8String *value) {

    if(!obj || !value) return;

    // Pass the caller's object directly instead of a PA_DuplicateObject
    // (= OB Copy, a full deep copy) of it. This mirrors the SDK's own
    // PA_DuplicateObject, which hands its argument to PA_ExecuteCommandByID
    // the same way. The parameter is detached before it goes out of scope
    // and never cleared, so the caller's reference is not released.
    PA_Variable    _params[1];
    _params[0] = PA_CreateVariable(eVK_Undefined);
    PA_SetObjectVariable(&_params[0], obj);
    PA_Variable vjson = PA_ExecuteCommandByID( /*JSON Stringify */1217, _params, 1);
    PA_SetObjectVariable(&_params[0], NULL);

    if(PA_GetVariableKind(vjson) != eVK_Unistring)
    {
        // "JSON Stringify" didn't return a string (unexpected failure) --
        // bail out instead of reading a Unistring out of a variable that
        // may not hold one.
        PA_ClearVariable(&vjson);
        return;
    }

    PA_Unistring ujson = PA_GetStringVariable(vjson);

#ifdef _WIN32
    int len = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)ujson.fString, ujson.fLength, NULL, 0, NULL, NULL);

    if(len){
        std::vector<uint8_t> buf(len + 1);
        if(WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)ujson.fString, ujson.fLength, (LPSTR)&buf[0], len, NULL, NULL)){
            *value = CUTF8String((const uint8_t *)&buf[0], (size_t)len);
        }
    }else{
        *value = CUTF8String((const uint8_t *)"");
    }

#else
    CFStringRef str = CFStringCreateWithCharacters(kCFAllocatorDefault, (const UniChar *)ujson.fString, ujson.fLength);
    if(str){

        size_t size = CFStringGetMaximumSizeForEncoding(CFStringGetLength(str), kCFStringEncodingUTF8) + sizeof(uint8_t);
        std::vector<uint8_t> buf(size);
        CFIndex len = 0;
        CFStringGetBytes(str, CFRangeMake(0, CFStringGetLength(str)), kCFStringEncodingUTF8, 0, true, (UInt8 *)&buf[0], size, &len);

        *value = CUTF8String((const uint8_t *)&buf[0], len);
        CFRelease(str);
    }

#endif

    PA_ClearVariable(&vjson);
}
