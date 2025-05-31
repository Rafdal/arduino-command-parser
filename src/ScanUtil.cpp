#include "ScanUtil.h"
#include <ctype.h>

int ScanUtil::seek(char *cstr)
{
    uint8_t i = 0;
    if (cstr == NULL || cstr[0] == NULLCHAR)
        return 1; // empty string is always found

    while ((cstr[i] != NULLCHAR) && (pos < size) && (str[pos] != NULLCHAR))
    {
        if (str[pos] == cstr[i]) // found a match
            i++;
        else
            i = 0;
        pos++;

        // reached null-terminator of cstr means we have a full match of the string
        if (cstr[i] == NULLCHAR)
            return 1;
    }
    err++;
    return 0;
}

int ScanUtil::skip(unsigned int n)
{
    if (pos + n < size)
    {
        pos += n;
        return 1;
    }
    err++;
    return 0;
}

int ScanUtil::skipTo(char c)
{
    while (pos < size && str[pos] != c && str[pos] != NULLCHAR)
        pos++;
    if (str[pos] == c)
    {
        pos++; // skip this char
        return 1;
    }
    err++;
    return 0;
}

uint8_t ScanUtil::substring(char *substr, char delimiter, uint8_t maxSize)
{
    uint8_t i = 0;
    while ((pos < size) && (i < SCAN_UTIL_MAX_SUBSTR_SIZE))
    {
        if ((str[pos] == NULLCHAR) || (str[pos] == delimiter) || (maxSize != 0 && i >= maxSize))
        {
            substr[i] = NULLCHAR; // write null terminator
            pos++;                // skip delimiter character
            // Serial.print("substr:");
            // Serial.print(size);
            // Serial.print(",");
            // Serial.print(i);
            // Serial.print(",");
            // Serial.print(pos);
            // Serial.print(", \'");
            // Serial.print(substr);
            // Serial.print("\' , \'");
            // Serial.print(str);
            // Serial.println('\'');
            return i;
        }
        substr[i++] = normalizeChar(str[pos++]); // copy normalized string
    }
    substr[i] = NULLCHAR; // write null terminator
    return i;
}

uint8_t ScanUtil::extract(char* substr, const char* excludeChars, uint8_t maxSize, char replace)
{
    uint8_t i = 0;
    while ((pos < size) && (i < SCAN_UTIL_MAX_SUBSTR_SIZE))
    {
        if (strchr(excludeChars, str[pos]) != NULL)
        {
            if (replace != NULLCHAR)
                substr[i++] = replace; // replace excluded character
            pos++; // skip excluded character
            continue;
        }
        if ((str[pos] == NULLCHAR) || (maxSize != 0 && i >= maxSize))
        {
            substr[i] = NULLCHAR; // write null terminator
            pos++;                // skip delimiter character
            return i;
        }
        substr[i++] = normalizeChar(str[pos++]); // copy normalized string
    }
    substr[i] = NULLCHAR; // write null terminator
    return i;
}

bool ScanUtil::getANum(void)
{
    // First of all find first digit or sign character
    sign = 0; // (0 = not  found)
    bool foundDigitOrSign = false;
    while (!foundDigitOrSign && (str[pos] != NULLCHAR) && (pos < size))
    {
        switch (str[pos])
        {
        case ' ':
        case '\t':
            pos++; // skip whitespaces
            break;
        case '-':
            sign = -1;
            foundDigitOrSign = true;
            pos++;
            break;
        case '+':
            sign = 1;
            foundDigitOrSign = true;
            pos++;
            break;
        default:
            if (isdigit(str[pos]))
            {
                sign = 1; // assume positive
                foundDigitOrSign = true;
                // Dont Increment pos!
            }
            else
            {
                err++;
                return false; // not found
            }
            break;
        }
    }

    if(sign == 0)
    {
        err++;
        return false; // not found
    }

    // Note that we should be positioned at the first digit
    scanNum = 0;
    unsigned long lastNum = 0;
    bool foundNumbers = false;
    while ((str[pos] != NULLCHAR) && (pos < size) && isdigit(str[pos]))
    {
        foundNumbers = true;

        // add digits one by one
        scanNum = (scanNum * 10) + (unsigned long)(str[pos] - '0');

        if(scanNum < lastNum) // overflow!
        {
            err++;
            return false;
        }
        lastNum = scanNum;
        pos++;
    }

    if(!foundNumbers)
    {
        err++;
        return false;
    }
    
    pos++; // skip the non-digit character and move next
    return true;
}

bool ScanUtil::get_uint8_t(uint8_t *out)
{
    if(out != NULL)
    {
        if(getANum())
        {
            if((scanNum <= 0xFF) && (sign == 1))
            {
                *out = (uint8_t)scanNum;
                return true;
            }
        }       
    }
    err++;
    return false;
}

bool ScanUtil::get_unsigned_long(unsigned long *out)
{
    if(out != NULL)
    {
        if(getANum())
        {
            if(sign == 1)
            {
                *out = scanNum;
                return true;
            }
        }       
    }
    err++;
    return false;
}

bool ScanUtil::get_int(int *out)
{
    if(out != NULL)
    {
        if(getANum())
        {
            if(sign == 1)
            {
                if(scanNum <= INT16_MAX)
                {
                    *out = ((int) scanNum);
                    return true;
                }
            }
            else if(sign == -1)
            {
                if((-(int)scanNum) >= INT16_MIN) // ugly way to avoid warnings
                {
                    *out = -1 * ((int) scanNum);
                    return true;
                }                
            }
        }     
    }
    err++;
    return false;
}

char ScanUtil::normalizeChar(char c)
{
    // Source: https://theasciicode.com.ar/
    switch ((unsigned)c)
    {
    case 131:
    case 132:
    case 133:
    case 134:
    case 142:
    case 143:
    case 160:
    case 181:
    case 182:
    case 183:
    case 198:
    case 199:
        c = 'A';
        break;
    case 130:
    case 136:
    case 137:
    case 138:
    case 144:
    case 210:
    case 211:
    case 212:
        c = 'E';
        break;
    case 139:
    case 140:
    case 141:    
    case 161:    
    case 213:    
    case 214:    
    case 215:    
    case 216:    
    case 222:    
        c = 'I';
        break;
    case 147:
    case 148:
    case 149:
    case 153:
    case 162:
    case 224:
    case 226:
    case 227:
    case 228:
    case 229:
        c = 'O';
        break;
    case 129:
    case 150:
    case 151:
    case 154:
    case 163:
    case 233:
    case 234:
    case 235:
        c = 'U';
        break;
    case 164:
    case 165:
        c = 'N';
        break;
    default:
        c = toupper(c);
        break;
    }
    return c;
}