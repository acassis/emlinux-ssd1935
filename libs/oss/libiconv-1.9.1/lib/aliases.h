/* ANSI-C code produced by gperf version 3.0.1 */
/* Command-line: gperf -m 10 lib/aliases.gperf  */
/* Computed positions: -k'1,5-7,$' */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

#line 1 "lib/aliases.gperf"
struct alias { int name; unsigned int encoding_index; };

#define TOTAL_KEYWORDS 52
#define MIN_WORD_LENGTH 2
#define MAX_WORD_LENGTH 17
#define MIN_HASH_VALUE 8
#define MAX_HASH_VALUE 62
/* maximum key range = 55, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
aliases_hash (register const char *str, register unsigned int len)
{
  static const unsigned char asso_values[] =
    {
      63, 63, 63, 63, 63, 63, 63, 63, 63, 63,
      63, 63, 63, 63, 63, 63, 63, 63, 63, 63,
      63, 63, 63, 63, 63, 63, 63, 63, 63, 63,
      63, 63, 63, 63, 63, 63, 63, 63, 63, 63,
      63, 63, 63, 63, 63,  1, 63, 63, 17,  1,
       7, 19,  4,  2,  1,  1,  3, 63, 63, 63,
      63, 63, 63, 63, 63, 15, 19,  1,  1,  1,
       6, 36, 63,  1, 63, 63, 18, 63, 63,  1,
       7, 63, 48, 14,  1,  2, 19,  1,  4, 63,
      63, 63, 63, 63, 63,  4, 63, 63, 63, 63,
      63, 63, 63, 63, 63, 63, 63, 63, 63, 63,
      63, 63, 63, 63, 63, 63, 63, 63, 63, 63,
      63, 63, 63, 63, 63, 63, 63, 63
    };
  register int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[6]];
      /*FALLTHROUGH*/
      case 6:
        hval += asso_values[(unsigned char)str[5]];
      /*FALLTHROUGH*/
      case 5:
        hval += asso_values[(unsigned char)str[4]];
      /*FALLTHROUGH*/
      case 4:
      case 3:
      case 2:
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval + asso_values[(unsigned char)str[len - 1]];
}

struct stringpool_t
  {
    char stringpool_str8[sizeof("CP367")];
    char stringpool_str9[sizeof("UTF-7")];
    char stringpool_str10[sizeof("IBM367")];
    char stringpool_str11[sizeof("UTF-16")];
    char stringpool_str12[sizeof("CSASCII")];
    char stringpool_str13[sizeof("UTF-8")];
    char stringpool_str14[sizeof("CSUNICODE")];
    char stringpool_str15[sizeof("UCS-4")];
    char stringpool_str16[sizeof("CSUNICODE11")];
    char stringpool_str17[sizeof("UNICODE-1-1")];
    char stringpool_str18[sizeof("US")];
    char stringpool_str19[sizeof("UNICODELITTLE")];
    char stringpool_str20[sizeof("CSUNICODE11UTF7")];
    char stringpool_str21[sizeof("UCS-2")];
    char stringpool_str22[sizeof("ASCII")];
    char stringpool_str23[sizeof("UNICODE-1-1-UTF-7")];
    char stringpool_str24[sizeof("ISO_646.IRV:1991")];
    char stringpool_str25[sizeof("BIG5")];
    char stringpool_str27[sizeof("US-ASCII")];
    char stringpool_str28[sizeof("BIG-5")];
    char stringpool_str29[sizeof("CSUCS4")];
    char stringpool_str30[sizeof("ISO646-US")];
    char stringpool_str31[sizeof("UTF-16LE")];
    char stringpool_str32[sizeof("UTF-16BE")];
    char stringpool_str33[sizeof("UCS-4LE")];
    char stringpool_str34[sizeof("UCS-4BE")];
    char stringpool_str35[sizeof("UCS-4-SWAPPED")];
    char stringpool_str36[sizeof("UCS-2LE")];
    char stringpool_str37[sizeof("UCS-2BE")];
    char stringpool_str38[sizeof("UCS-2-SWAPPED")];
    char stringpool_str39[sizeof("ISO-10646-UCS-4")];
    char stringpool_str40[sizeof("UCS-4-INTERNAL")];
    char stringpool_str41[sizeof("UTF-32")];
    char stringpool_str42[sizeof("ISO-10646-UCS-2")];
    char stringpool_str43[sizeof("UCS-2-INTERNAL")];
    char stringpool_str44[sizeof("CSISO2022JP")];
    char stringpool_str45[sizeof("CSISO2022JP2")];
    char stringpool_str46[sizeof("ISO-2022-JP-1")];
    char stringpool_str47[sizeof("CSBIG5")];
    char stringpool_str48[sizeof("BIGFIVE")];
    char stringpool_str49[sizeof("CN-BIG5")];
    char stringpool_str50[sizeof("ISO-2022-JP")];
    char stringpool_str51[sizeof("UNICODEBIG")];
    char stringpool_str52[sizeof("ISO-2022-JP-2")];
    char stringpool_str53[sizeof("CHAR")];
    char stringpool_str54[sizeof("BIG-FIVE")];
    char stringpool_str55[sizeof("UTF-32LE")];
    char stringpool_str56[sizeof("UTF-32BE")];
    char stringpool_str57[sizeof("ANSI_X3.4-1986")];
    char stringpool_str59[sizeof("ANSI_X3.4-1968")];
    char stringpool_str60[sizeof("ISO-IR-6")];
    char stringpool_str62[sizeof("WCHAR_T")];
  };
static const struct stringpool_t stringpool_contents =
  {
    "CP367",
    "UTF-7",
    "IBM367",
    "UTF-16",
    "CSASCII",
    "UTF-8",
    "CSUNICODE",
    "UCS-4",
    "CSUNICODE11",
    "UNICODE-1-1",
    "US",
    "UNICODELITTLE",
    "CSUNICODE11UTF7",
    "UCS-2",
    "ASCII",
    "UNICODE-1-1-UTF-7",
    "ISO_646.IRV:1991",
    "BIG5",
    "US-ASCII",
    "BIG-5",
    "CSUCS4",
    "ISO646-US",
    "UTF-16LE",
    "UTF-16BE",
    "UCS-4LE",
    "UCS-4BE",
    "UCS-4-SWAPPED",
    "UCS-2LE",
    "UCS-2BE",
    "UCS-2-SWAPPED",
    "ISO-10646-UCS-4",
    "UCS-4-INTERNAL",
    "UTF-32",
    "ISO-10646-UCS-2",
    "UCS-2-INTERNAL",
    "CSISO2022JP",
    "CSISO2022JP2",
    "ISO-2022-JP-1",
    "CSBIG5",
    "BIGFIVE",
    "CN-BIG5",
    "ISO-2022-JP",
    "UNICODEBIG",
    "ISO-2022-JP-2",
    "CHAR",
    "BIG-FIVE",
    "UTF-32LE",
    "UTF-32BE",
    "ANSI_X3.4-1986",
    "ANSI_X3.4-1968",
    "ISO-IR-6",
    "WCHAR_T"
  };
#define stringpool ((const char *) &stringpool_contents)

static const struct alias aliases[] =
  {
    {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
#line 19 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str8, ei_ascii},
#line 44 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str9, ei_utf7},
#line 20 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str10, ei_ascii},
#line 38 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str11, ei_utf16},
#line 22 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str12, ei_ascii},
#line 23 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str13, ei_utf8},
#line 26 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str14, ei_ucs2},
#line 33 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str15, ei_ucs4},
#line 30 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str16, ei_ucs2be},
#line 29 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str17, ei_ucs2be},
#line 21 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str18, ei_ascii},
#line 32 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str19, ei_ucs2le},
#line 46 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str20, ei_utf7},
#line 24 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str21, ei_ucs2},
#line 13 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str22, ei_ascii},
#line 45 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str23, ei_utf7},
#line 15 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str24, ei_ascii},
#line 56 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str25, ei_ces_big5},
    {-1},
#line 12 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str27, ei_ascii},
#line 57 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str28, ei_ces_big5},
#line 35 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str29, ei_ucs4},
#line 14 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str30, ei_ascii},
#line 40 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str31, ei_utf16le},
#line 39 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str32, ei_utf16be},
#line 37 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str33, ei_ucs4le},
#line 36 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str34, ei_ucs4be},
#line 50 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str35, ei_ucs4swapped},
#line 31 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str36, ei_ucs2le},
#line 27 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str37, ei_ucs2be},
#line 48 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str38, ei_ucs2swapped},
#line 34 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str39, ei_ucs4},
#line 49 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str40, ei_ucs4internal},
#line 41 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str41, ei_utf32},
#line 25 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str42, ei_ucs2},
#line 47 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str43, ei_ucs2internal},
#line 52 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str44, ei_iso2022_jp},
#line 55 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str45, ei_iso2022_jp2},
#line 53 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str46, ei_iso2022_jp1},
#line 61 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str47, ei_ces_big5},
#line 59 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str48, ei_ces_big5},
#line 60 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str49, ei_ces_big5},
#line 51 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str50, ei_iso2022_jp},
#line 28 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str51, ei_ucs2be},
#line 54 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str52, ei_iso2022_jp2},
#line 62 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str53, ei_local_char},
#line 58 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str54, ei_ces_big5},
#line 43 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str55, ei_utf32le},
#line 42 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str56, ei_utf32be},
#line 18 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str57, ei_ascii},
    {-1},
#line 17 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str59, ei_ascii},
#line 16 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str60, ei_ascii},
    {-1},
#line 63 "lib/aliases.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str62, ei_local_wchar_t}
  };

#ifdef __GNUC__
__inline
#endif
const struct alias *
aliases_lookup (register const char *str, register unsigned int len)
{
  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = aliases_hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register int o = aliases[key].name;
          if (o >= 0)
            {
              register const char *s = o + stringpool;

              if (*str == *s && !strcmp (str + 1, s + 1))
                return &aliases[key];
            }
        }
    }
  return 0;
}
