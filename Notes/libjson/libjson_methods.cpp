#include "libjson_methods.h"

void json_set_f(JSONNODE *n, const wchar_t *name, json_number value)
{
	if(n)
	{
		JSONNODE *e = json_get(n, name);
		if(e)
		{
			json_set_f(e, value);//over-write existing value
		}else
		{
			json_push_back(n, json_new_f(name, value));
		}
	}
}

void json_set_i(JSONNODE *n, const wchar_t *name, json_int_t value)
{
	if(n)
	{
		JSONNODE *e = json_get(n, name);
		if(e)
		{
			json_set_i(e, value);//over-write existing value
		}else
		{
			json_push_back(n, json_new_i(name, value));
		}
	}
}

void json_set_bool(JSONNODE *n, const wchar_t *name, json_bool_t value)
{
	if(n)
	{
		JSONNODE *e = json_get(n, name);
		if(e)
		{
			json_set_b(e, value);//over-write existing value
		}else
		{
			json_push_back(n, json_new_b(name, value));
		}
	}
}

void json_conv(const char *value, std::wstring &u32)
{
	if(value)
	{
		C_TEXT t;
		CUTF8String u8;
		
		u8 = (const uint8_t *)value;
		t.setUTF8String(&u8);
		
#if VERSIONWIN
		u32 = std::wstring((wchar_t *)t.getUTF16StringPtr());
#else
		
		uint32_t dataSize = (t.getUTF16Length() * sizeof(wchar_t))+ sizeof(wchar_t);
		std::vector<char> buf(dataSize);
		
		PA_ConvertCharsetToCharset((char *)t.getUTF16StringPtr(),
															 t.getUTF16Length() * sizeof(PA_Unichar),
															 eVTC_UTF_16,
															 (char *)&buf[0],
															 dataSize,
															 eVTC_UTF_32);
		
		u32 = std::wstring((wchar_t *)&buf[0]);
#endif
	}else
	{
		u32 = L"";
	}
	
}

#if defined(__APPLE__)

NSString *json_copy_text(JSONNODE *n, const wchar_t *name)
{
	NSString *t = nil;
	
	if(n)
	{
		if(name)
		{
			json_char *json_string = json_as_string(json_get(n, name));
			t = json_conv((json_char *)json_string);
			json_free(json_string);
		}
	}
	return t ? t : @"";
}

NSString *json_conv(json_char *src)
{
	C_TEXT t;
	
	if(src)
	{
		std::wstring wstr = std::wstring(src);
		
		uint32_t dataSize = (uint32_t)((wstr.length() * sizeof(PA_Unichar)) + sizeof(PA_Unichar));
		std::vector<char> buf(dataSize);
		
		PA_ConvertCharsetToCharset((char *)wstr.c_str(),
															 (PA_long32)(wstr.length() * sizeof(wchar_t)),
															 eVTC_UTF_32,
															 (char *)&buf[0],
															 dataSize,
															 eVTC_UTF_16);
		
		//don't use len, null chars matter
		CUTF16String u16 = (const PA_Unichar *)&buf[0];
		t.setUTF16String(&u16);
	}
	return t.copyUTF16String();
}

void json_set_text(JSONNODE *n, NSString *value)
{
	if((n))
	{
		C_TEXT t;
		t.setUTF16String(value);
		
		uint32_t dataSize = (t.getUTF16Length() * sizeof(wchar_t))+ sizeof(wchar_t);
		std::vector<char> buf(dataSize);
		PA_ConvertCharsetToCharset((char *)t.getUTF16StringPtr(),
															 t.getUTF16Length() * sizeof(PA_Unichar),
															 eVTC_UTF_16,
															 (char *)&buf[0],
															 dataSize,
															 eVTC_UTF_32);
		JSONNODE *a = json_new(JSON_STRING);
		json_set_a(a, (wchar_t *)&buf[0]);
		json_push_back(n, a);
	}
}

void json_conv(NSString *value, std::wstring &u32)
{
	if(value)
	{
		C_TEXT t;
		CUTF8String u8;
		
		t.setUTF16String(value);
		
		uint32_t dataSize = (t.getUTF16Length() * sizeof(wchar_t))+ sizeof(wchar_t);
		std::vector<char> buf(dataSize);
		
		PA_ConvertCharsetToCharset((char *)t.getUTF16StringPtr(),
															 t.getUTF16Length() * sizeof(PA_Unichar),
															 eVTC_UTF_16,
															 (char *)&buf[0],
															 dataSize,
															 eVTC_UTF_32);
		
		u32 = std::wstring((wchar_t *)&buf[0]);
		
	}else
	{
		u32 = L"";
	}
	
}

void json_set_path(JSONNODE *n, const wchar_t *name, NSString *value, BOOL isDirectory)
{
	if(value)
	{
		NSURL *u = (NSURL *)CFURLCreateWithFileSystemPath(kCFAllocatorDefault, (CFStringRef)value, kCFURLPOSIXPathStyle, isDirectory);
		if(u)
		{
			NSString *path = (NSString *)CFURLCopyFileSystemPath((CFURLRef)u, kCFURLHFSPathStyle);
			
			if(isDirectory && (![path hasSuffix:@":"]))
			{
				json_set_text(n, name, [path stringByAppendingString:@":"]);
			}else
			{
				json_set_text(n, name, path);
			}
			[path release];
			[u release];
		}else
		{
			json_set_text(n, name, @"");
			NSLog(@"failed to convert path from %@", value);
		}
	}else
	{
		json_set_text(n, name, value);
	}
}

void json_set_text(JSONNODE *n, const wchar_t *name, NSString *value)
{
	if(n)
	{
		if(value)
		{
			std::wstring w32;
			json_conv(value, w32);
			
			JSONNODE *e = json_get(n, name);
			if(e)
			{
				json_set_a(e, w32.c_str());//over-write existing value
			}else
			{
				json_push_back(n, json_new_a(name, w32.c_str()));
			}
			
		}else
		{
			JSONNODE *e = json_get(n, name);
			if(e)
			{
				json_nullify(e);//over-write existing value
			}else
			{
				JSONNODE *node = json_new_a(name, L"");
				json_nullify(node);
				json_push_back(n, node);
			}
		}
	}
}

#endif

void json_set_null(JSONNODE *n, const wchar_t *name)
{
	if(n)
	{
		JSONNODE *e = json_get(n, name);
		if(e)
		{
			json_nullify(e);//over-write existing value
		}else
		{
			JSONNODE *node = json_new_a(name, L"");
			json_nullify(node);
			json_push_back(n, node);
		}
	}
}

void json_set_text(JSONNODE *n, const wchar_t *name, char *value)
{
	if(n)
	{
		if(value)
		{
			std::wstring w32;
			json_conv(value, w32);
			
			JSONNODE *e = json_get(n, name);
			if(e)
			{
				json_set_a(e, w32.c_str());//over-write existing value
			}else
			{
				json_push_back(n, json_new_a(name, w32.c_str()));
			}
			
		}else
		{
			JSONNODE *e = json_get(n, name);
			if(e)
			{
				json_nullify(e);//over-write existing value
			}else
			{
				JSONNODE *node = json_new_a(name, L"");
				json_nullify(node);
				json_push_back(n, node);
			}
		}
	}
}

void json_set_text(JSONNODE *n, char *value)
{
	if(n)
	{
		if(value)
		{
			std::wstring w32;
			json_conv(value, w32);
			
			JSONNODE *e = json_new(JSON_STRING);
			json_set_a(e, w32.c_str());
			json_push_back(n, e);
		}else
		{
			JSONNODE *e = json_new(JSON_STRING);
			json_nullify(e);
			json_push_back(n, e);
		}
	}
}

void json_set_text(JSONNODE *n, const wchar_t *name, C_TEXT &t)
{
	if(n)
	{
		if(name)
		{
			uint32_t dataSize = (t.getUTF16Length() * sizeof(wchar_t))+ sizeof(wchar_t);
			std::vector<char> buf(dataSize);
			PA_ConvertCharsetToCharset((char *)t.getUTF16StringPtr(),
																 t.getUTF16Length() * sizeof(PA_Unichar),
																 eVTC_UTF_16,
																 (char *)&buf[0],
																 dataSize,
																 eVTC_UTF_32);
			
			JSONNODE *e = json_get(n, name);
			if(e)
			{
				json_set_a(e, (wchar_t *)&buf[0]);
			}else
			{
				json_push_back(n, json_new_a(name, (wchar_t *)&buf[0]));
			}
			
		}else
		{
			JSONNODE *node = json_new_a(name, L"");
			json_nullify(node);
			json_push_back(n, node);
		}
	}
}

void json_stringify(JSONNODE *json, CUTF16String &t, BOOL pretty)
{
	json_char *json_string = pretty ? json_write_formatted(json) : json_write(json);
	std::wstring wstr = std::wstring(json_string);
	
#if VERSIONWIN
	t = CUTF16String((const PA_Unichar *)wstr.c_str(), (uint32_t)wstr.length());
#else
	uint32_t dataSize = (uint32_t)((wstr.length() * sizeof(wchar_t))+ sizeof(PA_Unichar));
	std::vector<char> buf(dataSize);
	uint32_t len = PA_ConvertCharsetToCharset((char *)wstr.c_str(),
																						(PA_long32)(wstr.length() * sizeof(wchar_t)),
																						eVTC_UTF_32,
																						(char *)&buf[0],
																						dataSize,
																						eVTC_UTF_16);
	t = CUTF16String((const PA_Unichar *)&buf[0], len);
#endif
	json_free(json_string);
}

void json_stringify(JSONNODE *json, C_TEXT &t, BOOL pretty)
{
	json_char *json_string = pretty ? json_write_formatted(json) : json_write(json);
	std::wstring wstr = std::wstring(json_string);
	uint32_t dataSize = (uint32_t)((wstr.length() * sizeof(wchar_t))+ sizeof(PA_Unichar));
	std::vector<char> buf(dataSize);
	PA_ConvertCharsetToCharset((char *)wstr.c_str(),
														 (PA_long32)(wstr.length() * sizeof(wchar_t)),
														 eVTC_UTF_32,
														 (char *)&buf[0],
														 dataSize,
														 eVTC_UTF_16);
	CUTF16String u16 = (const PA_Unichar *)&buf[0];
	t.setUTF16String(&u16);
	json_free(json_string);
}
