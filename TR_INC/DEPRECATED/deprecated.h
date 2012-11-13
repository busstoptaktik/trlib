/* machine dependent functions */
#ifdef  _WIN32
#define C_DIGIT(c)  ((c)!='Æ' && (c)!='Ø' && (c)!='Å' && ( c)!='æ' && (c)!='ø' && (c)!='å' && isdigit(c))
#define C_LOWER(c)  ((c)!='Æ' && (c)!='Ø' && (c)!='Å' && ((c)=='æ' || (c)=='ø' || (c)=='å' || islower(c))) 
#define C_UPPER(c)  ((c)!='æ' && (c)!='ø' && (c)!='å' && ((c)=='Æ' || (c)=='Ø' || (c)=='Å' || isupper(c)))
#define C_ALPHA(c)  (C_LOWER(c) || C_UPPER(c))
#define C_SPACE(c)  ((c) != -89 && (c)!='Æ' && (c)!='Ø' && (c)!='Å' && (c)!='æ' && (c)!='ø' && (c)!='å' && ((c)>0 && isspace(c)))
#define C_PUNCT(c)  ((c) != -89 && (c)!='Æ' && (c)!='Ø' && (c)!='Å' && (c)!='æ' && (c)!='ø' && (c)!='å' && ispunct(c))

#else
/* machine rekfs1 */
#define C_DIGIT(c)  (isdigit(c))
#define C_LOWER(c)  ((c)=='æ' || (c)=='ø' || (c)=='å' || islower(c))
#define C_UPPER(c)  ((c)=='Æ' || (c)=='Ø' || (c)=='Å' || isupper(c))
#define C_ALPHA(c)  (C_LOWER(c) || C_UPPER(c))
#define C_SPACE(c)  ((c)!='Æ' && (c)!='Ø' && (c)!='Å' && isspace(c))
#define C_PUNCT(c)  ((c) != -89 && (c)!='Æ' && (c)!='Ø' && (c)!='Å' && (c)!='æ' && (c)!='ø' && (c)!='å' && ispunct(c))
#endif
