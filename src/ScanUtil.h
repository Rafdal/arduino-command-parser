/**
 * @brief Pretty shitty but robust c-string scanning tool to avoid using scanf
 * @author Rafael Dalzotto
 * @date 2022-08-15
 * @link https://github.com/Rafdal/lib-sim800l-sms/blob/main/src/ScanUtil.h
 */

#ifndef _SCAN_UTIL_H_
#define _SCAN_UTIL_H_

#define SCAN_UTIL_MAX_SUBSTR_SIZE 32

#ifndef LOCAL_CPP_TEST
#include <Arduino.h>
#else
#include <cstring>
#include <cstdint>
#endif

#define NULLCHAR    (char)0

class ScanUtil
{
public:
    ScanUtil(char* str, unsigned int size = 0)
    {
        if(size == 0)
            this->size = strlen(str);
        else
            this->size = size;
        this->str = str; 
    }
    ScanUtil(const char* str, unsigned int size = 0)
    {
        if(size == 0)
            this->size = strlen(str);
        else
            this->size = size;
        this->str = const_cast<char*>(str);
    }

    /**
     * @brief Seek string and move to the position after
     * 
     * @param cstr  C-style string (null-terminated string)
     * @retval 1 = found
     * @retval 0 = not found or mismatch
     */
    int seek(char* cstr);
    /**
     * @brief Seek string and move to the position after
     * 
     * @param cstr  C-style string (null-terminated string)
     * @retval 1 = found
     * @retval 0 = not found or mismatch
     */
    int seek(const char* cstr);

    /**
     * @brief Seek string and if found move to the position after
     * 
     * @param cstr  C-style string (null-terminated string)
     * @retval 1 = found
     * @retval 0 = not found or mismatch
     */
    int seekIf(char* cstr)
    {
        int originalPos = pos;
        int res = seek(cstr);
        if(res == 0)
            pos = originalPos; // restore position
        return res;
    }

    /**
     * @brief Seek string and move to the position after
     * 
     * @param cstr  C-style string (null-terminated string)
     * @retval 1 = found
     * @retval 0 = not found or mismatch
     */
    int seekIf(const char* cstr);

    /**
     * @brief Don't move, just tell if cstr is contained
     * 
     * @param cstr  C-style string (null-terminated string)
     * @retval 1 = found
     * @retval 0 = not found or mismatch
     */
    bool contains(char* cstr)
    {
        return (bool)strstr(&str[pos], cstr);
    }

    /**
     * @brief Don't move, just tell if cstr is contained
     * 
     * @param cstr  C-style string (null-terminated string)
     * @retval 1 = found
     * @retval 0 = not found or mismatch
     */
    bool contains(const char* cstr);

    /**
     * @brief Check if character c is contained in the remaining string
     * 
     * @param c character to search
     * @retval true = found
     * @retval false = not found
     */
    bool contains(char c)
    {
        for(unsigned int i=pos; i<size; i++)
        {
            if(str[i] == c)
                return true;
        }
        return false;
    }

    /**
     * @brief Check if current position matches cstr
     * 
     * @param cstr  C-style string (null-terminated string)
     * @retval true = match
     * @retval false = mismatch
     */
    bool equals(const char* cstr)
    {
        return strcmp(&str[pos], cstr) == 0;
    }

    /**
     * @brief Skip n bytes
     *
     * @param n bytes to skip
     * @retval 1 = found
     * @retval 0 = not found or mismatch
     */
    int skip(unsigned int n);

    /**
     * @brief skip to first occurrence of char c
     * 
     * @param c 
     * @retval 1 = found
     * @retval 0 = not found or mismatch
     */
    int skipTo(char c);

    /**
     * @brief Save normalized substring until character delimiter is found (or NULLCHAR terminator)
     * 
     * @param substr External buffer, should be initialized with enough size
     * @param delimiter delimiter stop character
     * @param maxSize 
     * @retval size of the substring
     */
    uint8_t substring(char* substr, char delimiter, uint8_t maxSize = 0);

    /**
     * @brief Save normalized substring excluding characters in excludeChars
     * 
     * @param substr External buffer, should be initialized with enough size
     * @param excludeChars characters to exclude
     * @param maxSize maximum size of the extracted substring
     * @param replace character to replace excluded characters (NULLCHAR = no replace)
     * @retval size of the extracted substring
     */
    uint8_t extract(char* substr, const char* excludeChars, uint8_t maxSize = 0, char replace = '\0');

    /**
     * @brief Get the current number
     * 
     * @param out Pointer to number
     * @retval true - success
     * @retval false - error
     */
    bool get_uint8_t(uint8_t* out);
    bool get_unsigned_long(unsigned long* out);
    bool get_int(int* out);

    /**
     * @brief Get error count
     * 
     * @retval 0  == OK
     * @retval >0 == ERROR
     */
    int error(void);

    /**
     * @brief Normalize ASCII char (lowercase without accents)
     * 
     * @param c ASCII char (8 bit)
     * @return Normalized character
     */
    char normalizeChar(char c);

    /**
     * @brief Normalize all characters in the string
     */
    inline void normalize() {
        for (unsigned int i = 0; i < size; i++) {
            str[i] = normalizeChar(str[i]);
        }
    }

    inline char* current(void) { return &str[pos]; }
    inline unsigned int remaining(void) { return size - pos; }

    ~ScanUtil() {}

    unsigned int pos=0;     // pos to current char of str

private:
    char* str = NULL;       // ptr to c-str to be analyzed (must be null-terminated)
    unsigned int size;      // size of str
    int err=0;              // error counter


    unsigned long scanNum;
    int sign;
    /**
     * @brief Get a number
     * 
     * @retval true - success
     * @retval false - error
     */
    bool getANum(void);
};

inline int ScanUtil::error(void)
{
    return err;
}

inline int ScanUtil::seek(const char *cstr)
{
    char *p = const_cast<char *>(cstr);
    return seek(p);
}

inline int ScanUtil::seekIf(const char *cstr)
{
    char *p = const_cast<char *>(cstr);
    return seekIf(p);
}

inline bool ScanUtil::contains(const char *cstr)
{
    char *p = const_cast<char *>(cstr);
    return contains(p);
}

#endif