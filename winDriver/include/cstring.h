/* Copyright (C) Jungo 1997 - 2005 */

#ifndef _CSTRING_H_
#define _CSTRING_H_

#include <stdarg.h>
#if defined(WIN16)
    #include <windows.h> 
    #undef toupper
    #undef tolower
#endif

typedef const char *PCSTR;
class CCString
{
public:
    // Constructors
    CCString();
    CCString(const CCString& stringSrc);
    CCString(PCSTR pcwStr);

    virtual ~CCString();

    operator PCSTR() const;
#if defined(WIN16)
    operator LPCSTR() const;
#endif
    operator char *() const;

    // MFC compatibility functions
    // overloaded assignment
    const BOOL operator==(const CCString& s);
    const BOOL operator==(const char *s);
    const BOOL operator!=(const CCString& s);
    const BOOL operator!=(const char *s);
    const CCString& operator=(const CCString& s);
    const CCString& operator=(PCSTR s);
    const CCString& operator+=(const PCSTR s);
    char& operator[](int i);
    int Compare(PCSTR s);
    int CompareNoCase(PCSTR s);
    const BOOL operator!=(char *s);
    void MakeUpper();
    void MakeLower();

    // sub-string extraction
    CCString Mid(int nFirst, int nCount);
    CCString Mid(int nFirst);

    int Length();
    BOOL is_empty();
    char *m_str; // The String itself

    void Format(const PCSTR format, ...);
    BOOL IsAllocOK();
    BOOL GetBuffer(ULONG size);

    // ANSI C compatibility functions
    int strcmp(const PCSTR s);
    int stricmp(const PCSTR s);
    void sprintf(const PCSTR format, ...);
    void toupper();
    void tolower();

protected:
    void Init();    
    void vsprintf(const PCSTR format, va_list ap);
    int m_buf_size;
};

const CCString operator+(const CCString &str1, const CCString &str2);

#if !defined(WIN32) && !defined(WIN16)
    int stricmp(PCSTR str1, PCSTR str2);
#endif

#endif

