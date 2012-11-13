/* machine dependent functions */
#ifdef  _WIN32
#define C_DIGIT(c)  ((c)!='�' && (c)!='�' && (c)!='�' && ( c)!='�' && (c)!='�' && (c)!='�' && isdigit(c))
#define C_LOWER(c)  ((c)!='�' && (c)!='�' && (c)!='�' && ((c)=='�' || (c)=='�' || (c)=='�' || islower(c))) 
#define C_UPPER(c)  ((c)!='�' && (c)!='�' && (c)!='�' && ((c)=='�' || (c)=='�' || (c)=='�' || isupper(c)))
#define C_ALPHA(c)  (C_LOWER(c) || C_UPPER(c))
#define C_SPACE(c)  ((c) != -89 && (c)!='�' && (c)!='�' && (c)!='�' && (c)!='�' && (c)!='�' && (c)!='�' && ((c)>0 && isspace(c)))
#define C_PUNCT(c)  ((c) != -89 && (c)!='�' && (c)!='�' && (c)!='�' && (c)!='�' && (c)!='�' && (c)!='�' && ispunct(c))

#else
/* machine rekfs1 */
#define C_DIGIT(c)  (isdigit(c))
#define C_LOWER(c)  ((c)=='�' || (c)=='�' || (c)=='�' || islower(c))
#define C_UPPER(c)  ((c)=='�' || (c)=='�' || (c)=='�' || isupper(c))
#define C_ALPHA(c)  (C_LOWER(c) || C_UPPER(c))
#define C_SPACE(c)  ((c)!='�' && (c)!='�' && (c)!='�' && isspace(c))
#define C_PUNCT(c)  ((c) != -89 && (c)!='�' && (c)!='�' && (c)!='�' && (c)!='�' && (c)!='�' && (c)!='�' && ispunct(c))
#endif
