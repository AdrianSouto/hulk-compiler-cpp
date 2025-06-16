/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "parser.y"

#include "../include/AllHeaders.hpp"
#include "../include/Expressions/ForRangeNode.hpp"
#include "../include/AST/Parameter.hpp"
#include "../include/AST/Attribute.hpp"
#include <iostream>
#include <string>
#include <cmath>
#include <vector>

int yylex();
void yyerror(const char* s);
int line_count = 1;
Program program;
extern char* yytext;

#line 88 "hulk/parser.cpp"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "parser.hpp"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_NUMBER = 3,                     /* NUMBER  */
  YYSYMBOL_STRING = 4,                     /* STRING  */
  YYSYMBOL_IDENTIFIER = 5,                 /* IDENTIFIER  */
  YYSYMBOL_PRINT = 6,                      /* PRINT  */
  YYSYMBOL_EOL = 7,                        /* EOL  */
  YYSYMBOL_LET = 8,                        /* LET  */
  YYSYMBOL_IN = 9,                         /* IN  */
  YYSYMBOL_FUNCTION = 10,                  /* FUNCTION  */
  YYSYMBOL_ARROW = 11,                     /* ARROW  */
  YYSYMBOL_TYPE = 12,                      /* TYPE  */
  YYSYMBOL_INHERITS = 13,                  /* INHERITS  */
  YYSYMBOL_BASE = 14,                      /* BASE  */
  YYSYMBOL_NEW = 15,                       /* NEW  */
  YYSYMBOL_IF = 16,                        /* IF  */
  YYSYMBOL_ELIF = 17,                      /* ELIF  */
  YYSYMBOL_ELSE = 18,                      /* ELSE  */
  YYSYMBOL_IS = 19,                        /* IS  */
  YYSYMBOL_AS = 20,                        /* AS  */
  YYSYMBOL_LPAREN = 21,                    /* LPAREN  */
  YYSYMBOL_RPAREN = 22,                    /* RPAREN  */
  YYSYMBOL_LBRACE = 23,                    /* LBRACE  */
  YYSYMBOL_RBRACE = 24,                    /* RBRACE  */
  YYSYMBOL_TYPE_NUMBER = 25,               /* TYPE_NUMBER  */
  YYSYMBOL_TYPE_STRING = 26,               /* TYPE_STRING  */
  YYSYMBOL_TYPE_BOOLEAN = 27,              /* TYPE_BOOLEAN  */
  YYSYMBOL_TRUE = 28,                      /* TRUE  */
  YYSYMBOL_FALSE = 29,                     /* FALSE  */
  YYSYMBOL_WHILE = 30,                     /* WHILE  */
  YYSYMBOL_FOR = 31,                       /* FOR  */
  YYSYMBOL_PLUS = 32,                      /* PLUS  */
  YYSYMBOL_MINUS = 33,                     /* MINUS  */
  YYSYMBOL_MULTIPLY = 34,                  /* MULTIPLY  */
  YYSYMBOL_DIVIDE = 35,                    /* DIVIDE  */
  YYSYMBOL_MODULO = 36,                    /* MODULO  */
  YYSYMBOL_COLON = 37,                     /* COLON  */
  YYSYMBOL_POWER = 38,                     /* POWER  */
  YYSYMBOL_CONCAT = 39,                    /* CONCAT  */
  YYSYMBOL_CONCAT_SPACE = 40,              /* CONCAT_SPACE  */
  YYSYMBOL_SEMICOLON = 41,                 /* SEMICOLON  */
  YYSYMBOL_COMMA = 42,                     /* COMMA  */
  YYSYMBOL_EQUALS = 43,                    /* EQUALS  */
  YYSYMBOL_EQEQ = 44,                      /* EQEQ  */
  YYSYMBOL_AND = 45,                       /* AND  */
  YYSYMBOL_GREATEREQ = 46,                 /* GREATEREQ  */
  YYSYMBOL_NOTEQ = 47,                     /* NOTEQ  */
  YYSYMBOL_LESS = 48,                      /* LESS  */
  YYSYMBOL_GREATER = 49,                   /* GREATER  */
  YYSYMBOL_OR = 50,                        /* OR  */
  YYSYMBOL_NOT = 51,                       /* NOT  */
  YYSYMBOL_LESSEQ = 52,                    /* LESSEQ  */
  YYSYMBOL_ASSIGN = 53,                    /* ASSIGN  */
  YYSYMBOL_DOT = 54,                       /* DOT  */
  YYSYMBOL_YYACCEPT = 55,                  /* $accept  */
  YYSYMBOL_program = 56,                   /* program  */
  YYSYMBOL_statement_list = 57,            /* statement_list  */
  YYSYMBOL_statement = 58,                 /* statement  */
  YYSYMBOL_statement_block = 59,           /* statement_block  */
  YYSYMBOL_statement_list_block = 60,      /* statement_list_block  */
  YYSYMBOL_declaration_list = 61,          /* declaration_list  */
  YYSYMBOL_declaration = 62,               /* declaration  */
  YYSYMBOL_type = 63,                      /* type  */
  YYSYMBOL_expression = 64,                /* expression  */
  YYSYMBOL_postfix_expression = 65,        /* postfix_expression  */
  YYSYMBOL_primary_expression = 66,        /* primary_expression  */
  YYSYMBOL_expression_list = 67,           /* expression_list  */
  YYSYMBOL_identifier_list = 68,           /* identifier_list  */
  YYSYMBOL_parameter_list = 69,            /* parameter_list  */
  YYSYMBOL_parameter = 70,                 /* parameter  */
  YYSYMBOL_ast_construct = 71,             /* ast_construct  */
  YYSYMBOL_expression_block = 72,          /* expression_block  */
  YYSYMBOL_type_member_list = 73,          /* type_member_list  */
  YYSYMBOL_type_member = 74                /* type_member  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  54
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1966

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  55
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  20
/* YYNRULES -- Number of rules.  */
#define YYNRULES  161
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  377

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   309


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    79,    79,    80,    84,    88,    93,    94,    95,    96,
      97,    98,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   119,   128,   137,   146,   150,   154,   158,   162,   166,
     170,   174,   178,   179,   183,   193,   203,   213,   223,   233,
     253,   273,   284,   291,   298,   307,   320,   333,   345,   349,
     353,   357,   361,   362,   363,   367,   368,   375,   382,   392,
     401,   413,   414,   415,   416,   425,   426,   427,   428,   429,
     430,   431,   432,   433,   438,   439,   440,   441,   442,   443,
     444,   445,   446,   447,   448,   449,   450,   451,   452,   453,
     454,   455,   456,   467,   473,   479,   485,   491,   498,   505,
     512,   519,   526,   533,   541,   542,   543,   544,   545,   546,
     547,   625,   632,   633,   648,   653,   657,   658,   662,   671,
     672,   673,   674,   675,   676,   688,   693,   697,   701,   707,
     708,   709,   710,   714,   715,   716,   717,   718,   722,   723,
     730,   737,   744,   748,   757,   769,   770,   774,   778,   782,
     786,   790,   793,   796,   803,   804,   811,   815,   819,   828,
     833,   842
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "NUMBER", "STRING",
  "IDENTIFIER", "PRINT", "EOL", "LET", "IN", "FUNCTION", "ARROW", "TYPE",
  "INHERITS", "BASE", "NEW", "IF", "ELIF", "ELSE", "IS", "AS", "LPAREN",
  "RPAREN", "LBRACE", "RBRACE", "TYPE_NUMBER", "TYPE_STRING",
  "TYPE_BOOLEAN", "TRUE", "FALSE", "WHILE", "FOR", "PLUS", "MINUS",
  "MULTIPLY", "DIVIDE", "MODULO", "COLON", "POWER", "CONCAT",
  "CONCAT_SPACE", "SEMICOLON", "COMMA", "EQUALS", "EQEQ", "AND",
  "GREATEREQ", "NOTEQ", "LESS", "GREATER", "OR", "NOT", "LESSEQ", "ASSIGN",
  "DOT", "$accept", "program", "statement_list", "statement",
  "statement_block", "statement_list_block", "declaration_list",
  "declaration", "type", "expression", "postfix_expression",
  "primary_expression", "expression_list", "identifier_list",
  "parameter_list", "parameter", "ast_construct", "expression_block",
  "type_member_list", "type_member", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-260)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-118)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     465,  -260,  -260,    -7,  1147,  -260,    35,    80,   121,    74,
     161,  1147,   363,   496,  -260,  -260,  1178,    96,  1147,  1147,
     204,   401,    17,  -260,  1198,    97,  -260,   837,  1147,     9,
      35,   868,   231,  1221,   194,    13,  -260,   236,   254,   899,
     269,   415,   226,  1255,   218,  -260,   276,  1277,   806,   363,
     415,   263,  -260,  -260,  -260,    30,  -260,    35,  1935,   271,
     286,  -260,   160,   290,  1147,  1147,  1147,  1147,  1147,  1147,
    1147,  1147,  -260,  1147,  1147,  1147,  1147,  1147,  1147,  1147,
    1147,  1147,   291,   153,  1778,     3,  1300,   930,  1147,    14,
    -260,   292,  -260,   193,  1147,    33,   527,   136,   293,   296,
      31,  -260,  -260,    19,   961,  1147,  -260,  1778,   247,  -260,
    -260,   838,  -260,  -260,  -260,  -260,  -260,  1323,  1357,  -260,
     302,   992,  1147,    82,  -260,   310,  -260,  -260,  -260,  -260,
    -260,  -260,  -260,  -260,   306,   306,   278,   278,   278,   278,
     321,   321,  1844,  1822,   270,  1844,   270,   270,  1800,   270,
    1778,   299,  -260,  1147,   180,  1023,  -260,  -260,    85,  1778,
     187,  1054,   308,  -260,  -260,  -260,  -260,   279,  1778,   558,
    -260,   363,  -260,  1379,    35,  -260,   288,    86,    90,  -260,
     158,   288,   300,    91,   101,  -260,  -260,   112,   238,   222,
    1147,   589,  -260,  -260,  1894,  1147,   285,   116,  1300,   227,
     620,   329,  1085,  1778,  -260,  1147,  1778,  -260,  1116,  1147,
    1778,  1147,  1147,   363,  -260,  1402,  -260,  1425,  -260,  -260,
     193,   331,    24,   246,   340,    21,   264,  1147,  -260,  -260,
      20,    99,  -260,  -260,  -260,  1147,   651,   415,   363,  -260,
    -260,  1459,   307,   682,   363,  1379,  1147,  -260,   130,  1778,
    1147,  1778,  1778,  1493,  1778,  -260,  1515,  -260,  -260,  -260,
    -260,  1147,  1935,  -260,   342,  -260,  1147,  1935,   193,   340,
    -260,   138,   144,   151,   344,  -260,    32,   193,  1147,   415,
     363,  -260,    83,  -260,   363,   363,  1402,  1425,  1549,  -260,
    1778,   363,  -260,  1571,   327,  -260,  1594,  1906,    92,  -260,
     341,  -260,  -260,   178,   154,    98,   140,   309,  1617,   139,
    -260,   345,   713,   324,  1515,   363,  -260,  -260,  -260,  -260,
    -260,  1147,  1935,  -260,  1147,  -260,  -260,  1147,   193,   103,
    1147,  -260,   354,   744,   363,   363,  -260,  -260,   324,  1640,
    1926,   169,   141,   179,  1663,   365,  1147,   193,  1686,   775,
     363,  -260,  -260,  -260,  -260,  -260,  -260,   351,  -260,  -260,
    1147,  1709,   370,  -260,   363,  -260,  -260,  -260,  1732,  -260,
    1147,  -260,   186,  -260,  1755,  -260,  -260
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       5,   118,   119,   122,     0,    11,     0,     0,     0,     0,
       0,     0,     0,    54,   120,   121,     0,     0,     0,     0,
       0,     2,    10,    32,     4,    65,   112,     0,     0,   122,
       0,   153,     0,    13,     0,     0,    55,     0,     0,     0,
       0,     0,     0,     0,   152,    49,     0,   147,     0,     0,
       0,     0,    90,    89,     1,     0,     8,     0,    54,     0,
       7,     9,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     3,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    19,   129,     0,    15,     0,     0,     0,
     152,     0,    12,     0,     0,     0,     0,     0,   137,     0,
     142,   154,   128,     0,     0,     0,   146,   145,     0,   124,
     123,    52,    48,    50,   148,   151,   111,   149,     0,   109,
       0,     0,     0,     0,    53,     0,     6,    80,    81,    82,
      83,    84,    85,    86,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    87,    79,    75,    76,    78,    88,    77,
      92,   113,    17,     0,    18,     0,    14,   117,     0,    91,
       0,     0,     0,    64,    61,    62,    63,     0,    59,     0,
      58,     0,    20,    25,     0,    57,   133,     0,     0,   138,
       0,   143,     0,     0,     0,   127,   126,     0,    67,     0,
       0,     0,    51,   150,   123,     0,    19,     0,    15,     0,
       0,     0,     0,   132,    16,     0,   131,   116,     0,     0,
     104,     0,     0,     0,    21,    27,    22,    29,    24,    56,
       0,     0,     0,     0,     0,     0,     0,     0,   154,   154,
       0,     0,    43,   155,   125,     0,     0,     0,     0,    93,
     108,     0,    18,     0,     0,    25,     0,   115,     0,   130,
       0,   105,   106,     0,    60,    23,    31,    26,    28,   144,
     136,     0,    54,   135,     0,   141,     0,    54,     0,     0,
     140,     0,     0,     0,     0,   154,   142,     0,     0,     0,
       0,    94,     0,    95,     0,     0,    27,    29,     0,   114,
     107,     0,    30,     0,     0,   134,     0,     0,     0,   139,
       0,    44,    42,     0,     0,     0,     0,     0,     0,     0,
      96,     0,     0,    40,    31,     0,   110,    33,    36,    35,
      37,     0,    54,   154,     0,   154,    41,     0,     0,     0,
       0,   156,     0,     0,     0,     0,    97,    39,    40,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   100,    98,    99,    34,    38,    47,     0,    45,   159,
       0,     0,     0,   157,     0,   101,   102,   154,     0,   158,
       0,   103,     0,   161,     0,    46,   160
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -260,  -260,  -260,   118,  -260,  -259,    27,   -88,  -218,     0,
    -260,  -260,    -5,  -260,   -99,  -209,   -21,  -260,  -223,  -260
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,    20,    21,   106,    23,    46,    35,    36,   167,   107,
      25,    26,    85,   177,   178,   179,   108,    48,   184,   233
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      24,   183,   259,   294,    33,   272,   273,   170,   297,   175,
     153,    41,    43,    47,    27,   265,    50,   270,    52,    53,
      95,   160,    96,   161,    61,   154,   153,    84,    86,   119,
      87,    47,   266,   274,   103,   261,   181,   181,    34,    84,
      34,   185,   169,   275,   267,   155,    28,   262,   117,   118,
     298,   121,   304,   182,   305,    97,    97,    89,   268,   307,
     299,   155,    88,   340,   134,   135,   136,   137,   138,   139,
     140,   141,   170,   142,   143,   144,   145,   146,   147,   148,
     149,   150,   158,   122,   123,    37,   219,    84,   159,   199,
     311,   200,   153,   221,   168,    39,   173,   224,   224,   187,
     341,   312,   343,   321,    84,   188,   231,   207,   222,   327,
     345,   170,   225,   230,   346,   322,   197,    51,    22,   153,
     276,    84,   198,   153,    97,   232,    38,   155,   223,   362,
      42,    45,   226,   226,   234,   328,   277,   153,   242,    60,
     347,    34,   278,   174,   372,   153,   332,   224,   153,   231,
      81,    82,   289,   203,   155,   206,   231,   333,   155,   231,
     300,   210,   329,   357,   113,   127,    40,    42,   301,   215,
     239,   217,   155,   240,   231,   302,    45,   306,   326,   227,
     155,   228,   226,   155,   231,   128,   129,   130,   131,   132,
     237,   231,    34,   356,   152,   241,   208,   248,   163,   324,
     245,   325,    84,   358,    54,   249,  -117,  -117,   251,   252,
     375,   253,   254,   256,   172,   281,   282,   283,   164,   165,
     166,   204,   271,   -53,   -53,   -53,   -53,    84,   -53,   192,
     -53,    93,    34,  -116,  -116,   279,   243,    94,   -90,   235,
     236,   -53,   -53,   286,   287,   -90,   288,   -90,   109,   -53,
     290,   263,    91,   264,   189,   -90,   -90,    98,   309,   310,
     -90,   293,   -90,   313,   190,   191,   296,    99,   120,   181,
     316,   269,   -90,   -90,   -90,   100,   -90,   101,   308,   -90,
     -90,    55,     4,   111,    57,   314,     7,   214,     8,   216,
     104,   336,   125,   126,   338,   133,   151,   162,   176,    58,
     112,   180,    64,    65,    66,    67,    68,    59,    69,    70,
      71,   195,   351,   352,   353,   201,    69,   211,   172,   342,
     202,   339,   212,   229,    84,   220,   152,   344,   365,   366,
     348,   255,    55,     4,   111,    57,   260,     7,   246,     8,
      66,    67,    68,   371,    69,   181,   361,   295,   204,   303,
      58,   318,   330,    64,    65,    66,    67,    68,    59,    69,
     368,   214,   216,   334,   323,   337,     1,     2,     3,     4,
     374,     6,   349,     7,   367,     8,   360,     9,    10,    11,
      45,   370,     0,     0,    12,    45,    13,     0,     0,     0,
       0,    14,    15,    16,    17,     0,    18,     0,     0,     0,
       0,     0,     0,   255,     0,     0,    55,     4,    56,    57,
       0,     7,   113,     8,    19,   113,     0,     0,     1,     2,
       3,     4,     0,     6,    58,     7,     0,     8,     0,     9,
      10,    11,    59,     0,    62,    63,    12,     0,    13,     0,
      45,     0,     0,    14,    15,    16,    17,    64,   105,    66,
      67,    68,     0,    69,    70,    71,     0,     0,   113,    73,
      74,    75,    76,    77,    78,    79,    19,    80,     1,     2,
       3,     4,     5,     6,     0,     7,     0,     8,     0,     9,
      10,    11,     0,     0,     0,     0,    12,     0,    13,     0,
       0,     0,     0,    14,    15,    16,    17,     0,    18,     1,
       2,     3,     4,    44,     6,     0,     7,     0,     8,     0,
       9,    10,    11,     0,     0,     0,    19,    12,     0,    13,
       0,     0,     0,     0,    14,    15,    16,    17,     0,    18,
       1,     2,     3,     4,   171,     6,     0,     7,     0,     8,
       0,     9,    10,    11,     0,     0,     0,    19,    12,     0,
      13,     0,     0,     0,     0,    14,    15,    16,    17,     0,
      18,     1,     2,     3,     4,   213,     6,     0,     7,     0,
       8,     0,     9,    10,    11,     0,     0,     0,    19,    12,
       0,    13,     0,     0,     0,     0,    14,    15,    16,    17,
       0,    18,     1,     2,     3,     4,   238,     6,     0,     7,
       0,     8,     0,     9,    10,    11,     0,     0,     0,    19,
      12,     0,    13,     0,     0,     0,     0,    14,    15,    16,
      17,     0,    18,     1,     2,     3,     4,   244,     6,     0,
       7,     0,     8,     0,     9,    10,    11,     0,     0,     0,
      19,    12,     0,    13,     0,     0,     0,     0,    14,    15,
      16,    17,     0,    18,     1,     2,     3,     4,   280,     6,
       0,     7,     0,     8,     0,     9,    10,    11,     0,     0,
       0,    19,    12,     0,    13,     0,     0,     0,     0,    14,
      15,    16,    17,     0,    18,     1,     2,     3,     4,   285,
       6,     0,     7,     0,     8,     0,     9,    10,    11,     0,
       0,     0,    19,    12,     0,    13,     0,     0,     0,     0,
      14,    15,    16,    17,     0,    18,     1,     2,     3,     4,
     335,     6,     0,     7,     0,     8,     0,     9,    10,    11,
       0,     0,     0,    19,    12,     0,    13,     0,     0,     0,
       0,    14,    15,    16,    17,     0,    18,     1,     2,     3,
       4,   350,     6,     0,     7,     0,     8,     0,     9,    10,
      11,     0,     0,     0,    19,    12,     0,    13,     0,     0,
       0,     0,    14,    15,    16,    17,     0,    18,     1,     2,
       3,     4,   364,     6,     0,     7,     0,     8,     0,     9,
      10,    11,     0,     0,     0,    19,    12,     0,    13,     0,
       0,     0,     0,    14,    15,    16,    17,     0,    18,     1,
       2,    29,     0,   115,    30,     0,     0,     0,     0,     0,
       9,    10,    11,     0,     0,     0,    19,    12,     0,    31,
     116,     0,     0,     0,    14,    15,    16,    32,     0,    18,
       1,     2,    29,    55,     4,    30,    57,     0,     7,     0,
       8,     9,    10,    11,     0,     0,     0,    19,    12,    83,
      31,    58,     0,     0,     0,    14,    15,    16,    32,    59,
      18,     1,     2,    29,     0,    90,    30,     0,     0,     0,
       0,     0,     9,    10,    11,     0,     0,     0,    19,    12,
       0,    31,     0,     0,     0,     0,    14,    15,    16,    32,
       0,    18,     1,     2,    29,     0,     0,    30,     0,     0,
       0,     0,     0,     9,    10,    11,     0,     0,     0,    19,
      12,   102,    31,     0,     0,     0,     0,    14,    15,    16,
      32,     0,    18,     1,     2,    29,     0,     0,    30,     0,
       0,     0,     0,     0,     9,    10,    11,     0,     0,     0,
      19,    12,   157,    31,     0,     0,     0,     0,    14,    15,
      16,    32,     0,    18,     1,     2,    29,     0,     0,    30,
       0,     0,     0,     0,     0,     9,    10,    11,     0,     0,
       0,    19,    12,   186,    31,     0,     0,     0,     0,    14,
      15,    16,    32,     0,    18,     1,     2,    29,     0,     0,
      30,     0,     0,     0,     0,     0,     9,    10,    11,     0,
       0,     0,    19,    12,   196,    31,     0,     0,     0,     0,
      14,    15,    16,    32,     0,    18,     1,     2,    29,     0,
     205,    30,     0,     0,     0,     0,     0,     9,    10,    11,
       0,     0,     0,    19,    12,     0,    31,     0,     0,     0,
       0,    14,    15,    16,    32,     0,    18,     1,     2,    29,
       0,   209,    30,     0,     0,     0,     0,     0,     9,    10,
      11,     0,     0,     0,    19,    12,     0,    31,     0,     0,
       0,     0,    14,    15,    16,    32,     0,    18,     1,     2,
      29,     0,     0,    30,     0,     0,     0,     0,     0,     9,
      10,    11,     0,     0,     0,    19,    12,   247,    31,     0,
       0,     0,     0,    14,    15,    16,    32,     0,    18,     1,
       2,    29,     0,   250,    30,     0,     0,     0,     0,     0,
       9,    10,    11,     0,     0,     0,    19,    12,     0,    31,
       0,     0,     0,     0,    14,    15,    16,    32,     0,    18,
       1,     2,    29,     0,     0,    30,     0,     0,     0,     0,
       0,     9,    10,    11,     0,     0,     0,    19,    12,     0,
      31,     0,     0,     0,     0,    14,    15,    16,    32,     0,
      18,     1,     2,    29,     0,     0,    30,     0,     0,     0,
       0,     0,     9,    10,    11,     0,     0,     0,    19,    49,
       0,    31,     0,     0,     0,     0,    14,    15,    16,    32,
       0,    18,     0,     0,     0,     0,     0,    62,    63,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    19,
      64,    65,    66,    67,    68,     0,    69,    70,    71,    72,
      62,    63,    73,    74,    75,    76,    77,    78,    79,     0,
      80,     0,     0,    64,    65,    66,    67,    68,     0,    69,
      70,    71,    92,     0,     0,    73,    74,    75,    76,    77,
      78,    79,     0,    80,    62,    63,     0,   110,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    64,    65,    66,
      67,    68,     0,    69,    70,    71,    62,    63,     0,    73,
      74,    75,    76,    77,    78,    79,     0,    80,     0,    64,
      65,    66,    67,    68,     0,    69,    70,    71,   114,    62,
      63,    73,    74,    75,    76,    77,    78,    79,     0,    80,
       0,     0,    64,    65,    66,    67,    68,     0,    69,    70,
      71,   156,    62,    63,    73,    74,    75,    76,    77,    78,
      79,     0,    80,     0,     0,    64,    65,    66,    67,    68,
       0,    69,    70,    71,   193,     0,     0,    73,    74,    75,
      76,    77,    78,    79,     0,    80,    62,    63,     0,   194,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    64,
      65,    66,    67,    68,     0,    69,    70,    71,    62,    63,
       0,    73,    74,    75,    76,    77,    78,    79,     0,    80,
       0,    64,    65,    66,    67,    68,     0,    69,    70,    71,
     218,    62,    63,    73,    74,    75,    76,    77,    78,    79,
       0,    80,     0,     0,    64,    65,    66,    67,    68,     0,
      69,    70,    71,   257,    62,    63,    73,    74,    75,    76,
      77,    78,    79,     0,    80,     0,     0,    64,    65,    66,
      67,    68,     0,    69,    70,    71,   258,     0,     0,    73,
      74,    75,    76,    77,    78,    79,     0,    80,    62,    63,
       0,   284,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    64,    65,    66,    67,    68,     0,    69,    70,    71,
       0,     0,     0,    73,    74,    75,    76,    77,    78,    79,
       0,    80,    62,    63,     0,   291,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    64,    65,    66,    67,    68,
       0,    69,    70,    71,    62,    63,     0,    73,    74,    75,
      76,    77,    78,    79,     0,    80,     0,    64,    65,    66,
      67,    68,     0,    69,    70,    71,   292,     0,     0,    73,
      74,    75,    76,    77,    78,    79,     0,    80,    62,    63,
       0,   315,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    64,    65,    66,    67,    68,     0,    69,    70,    71,
      62,    63,     0,    73,    74,    75,    76,    77,    78,    79,
       0,    80,     0,    64,    65,    66,    67,    68,     0,    69,
      70,    71,   317,    62,    63,    73,    74,    75,    76,    77,
      78,    79,     0,    80,     0,     0,    64,    65,    66,    67,
      68,     0,    69,    70,    71,   319,    62,    63,    73,    74,
      75,    76,    77,    78,    79,     0,    80,     0,     0,    64,
      65,    66,    67,    68,     0,    69,    70,    71,   331,    62,
      63,    73,    74,    75,    76,    77,    78,    79,     0,    80,
       0,     0,    64,    65,    66,    67,    68,     0,    69,    70,
      71,   354,    62,    63,    73,    74,    75,    76,    77,    78,
      79,     0,    80,     0,     0,    64,    65,    66,    67,    68,
       0,    69,    70,    71,   359,    62,    63,    73,    74,    75,
      76,    77,    78,    79,     0,    80,     0,     0,    64,    65,
      66,    67,    68,     0,    69,    70,    71,   363,    62,    63,
      73,    74,    75,    76,    77,    78,    79,     0,    80,     0,
       0,    64,    65,    66,    67,    68,     0,    69,    70,    71,
     369,    62,    63,    73,    74,    75,    76,    77,    78,    79,
       0,    80,     0,     0,    64,    65,    66,    67,    68,     0,
      69,    70,    71,   373,    62,    63,    73,    74,    75,    76,
      77,    78,    79,     0,    80,     0,     0,    64,    65,    66,
      67,    68,     0,    69,    70,    71,   376,    62,    63,    73,
      74,    75,    76,    77,    78,    79,     0,    80,     0,     0,
      64,    65,    66,    67,    68,     0,    69,    70,    71,    62,
      63,     0,    73,    74,    75,    76,    77,    78,    79,     0,
      80,     0,    64,    65,    66,    67,    68,     0,    69,    70,
      71,    62,    63,     0,    73,    74,    75,    76,    77,    78,
       0,     0,    80,     0,    64,    65,    66,    67,    68,     0,
      69,    70,    71,    62,    63,     0,    73,     0,    75,    76,
      77,    78,     0,     0,    80,     0,    64,    65,    66,    67,
      68,     0,    69,    70,    71,     0,     0,     0,     0,     0,
      75,     0,    77,    78,     0,     0,    80,     1,     2,     3,
       4,     0,     6,     0,     7,     0,     8,     0,     9,    10,
      11,    55,     4,   111,    57,     0,     7,    13,     8,     0,
       0,     0,    14,    15,    16,    17,     0,     0,     0,    58,
     320,    55,     4,   111,    57,     0,     7,    59,     8,     0,
      55,     4,   124,    57,     0,     7,     0,     8,     0,    58,
     355,     0,     0,     0,     0,     0,     0,    59,    58,     0,
       0,     0,     0,     0,     0,     0,    59
};

static const yytype_int16 yycheck[] =
{
       0,   100,   220,   262,     4,   228,   229,    95,   267,    97,
       7,    11,    12,    13,    21,   224,    16,   226,    18,    19,
       7,     7,     9,     9,     7,    22,     7,    27,    28,    50,
      21,    31,    11,    13,    39,    11,     5,     5,     5,    39,
       5,    22,     9,    23,    23,    42,    53,    23,    48,    49,
     268,    21,   275,    22,    22,    42,    42,    30,    37,   277,
     269,    42,    53,   322,    64,    65,    66,    67,    68,    69,
      70,    71,   160,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    87,    53,    57,     5,   174,    87,    88,     7,
       7,     9,     7,     7,    94,    21,    96,     7,     7,   104,
     323,    18,   325,    11,   104,   105,     5,    22,    22,    11,
     328,   199,    22,    22,    11,    23,   121,    21,     0,     7,
      21,   121,   122,     7,    42,    24,     5,    42,    42,   347,
      12,    13,    42,    42,    22,    37,    37,     7,    22,    21,
      37,     5,    43,     7,   367,     7,     7,     7,     7,     5,
      53,    54,    22,   153,    42,   155,     5,    18,    42,     5,
      22,   161,    22,    22,    46,     5,     5,    49,    24,   169,
     191,   171,    42,   194,     5,    24,    58,   276,    24,    21,
      42,    23,    42,    42,     5,    25,    26,    27,    28,    29,
     190,     5,     5,    24,    41,   195,     9,   202,     5,    21,
     200,    23,   202,    24,     0,   205,    53,    54,   208,   209,
      24,   211,   212,   213,    96,   236,   237,   238,    25,    26,
      27,    41,   227,     5,     6,     7,     8,   227,    10,   111,
      12,    37,     5,    53,    54,   235,     9,    43,     0,    17,
      18,    23,    24,   243,   244,     7,   246,     9,    22,    31,
     250,     5,    21,     7,     7,    17,    18,    21,   279,   280,
      22,   261,    24,   284,    17,    18,   266,    13,     5,     5,
     291,     7,    34,    35,    36,    21,    38,    23,   278,    41,
      42,     5,     6,     7,     8,   285,    10,   169,    12,   171,
      21,   312,    21,     7,   315,     5,     5,     5,     5,    23,
      24,     5,    32,    33,    34,    35,    36,    31,    38,    39,
      40,     9,   333,   334,   335,     5,    38,     9,   200,   324,
      21,   321,    43,    23,   324,    37,    41,   327,   349,   350,
     330,   213,     5,     6,     7,     8,     5,    10,     9,    12,
      34,    35,    36,   364,    38,     5,   346,     5,    41,     5,
      23,    24,    43,    32,    33,    34,    35,    36,    31,    38,
     360,   243,   244,    18,    23,    41,     3,     4,     5,     6,
     370,     8,    18,    10,    23,    12,    11,    14,    15,    16,
     262,    11,    -1,    -1,    21,   267,    23,    -1,    -1,    -1,
      -1,    28,    29,    30,    31,    -1,    33,    -1,    -1,    -1,
      -1,    -1,    -1,   285,    -1,    -1,     5,     6,     7,     8,
      -1,    10,   294,    12,    51,   297,    -1,    -1,     3,     4,
       5,     6,    -1,     8,    23,    10,    -1,    12,    -1,    14,
      15,    16,    31,    -1,    19,    20,    21,    -1,    23,    -1,
     322,    -1,    -1,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    -1,    38,    39,    40,    -1,    -1,   340,    44,
      45,    46,    47,    48,    49,    50,    51,    52,     3,     4,
       5,     6,     7,     8,    -1,    10,    -1,    12,    -1,    14,
      15,    16,    -1,    -1,    -1,    -1,    21,    -1,    23,    -1,
      -1,    -1,    -1,    28,    29,    30,    31,    -1,    33,     3,
       4,     5,     6,     7,     8,    -1,    10,    -1,    12,    -1,
      14,    15,    16,    -1,    -1,    -1,    51,    21,    -1,    23,
      -1,    -1,    -1,    -1,    28,    29,    30,    31,    -1,    33,
       3,     4,     5,     6,     7,     8,    -1,    10,    -1,    12,
      -1,    14,    15,    16,    -1,    -1,    -1,    51,    21,    -1,
      23,    -1,    -1,    -1,    -1,    28,    29,    30,    31,    -1,
      33,     3,     4,     5,     6,     7,     8,    -1,    10,    -1,
      12,    -1,    14,    15,    16,    -1,    -1,    -1,    51,    21,
      -1,    23,    -1,    -1,    -1,    -1,    28,    29,    30,    31,
      -1,    33,     3,     4,     5,     6,     7,     8,    -1,    10,
      -1,    12,    -1,    14,    15,    16,    -1,    -1,    -1,    51,
      21,    -1,    23,    -1,    -1,    -1,    -1,    28,    29,    30,
      31,    -1,    33,     3,     4,     5,     6,     7,     8,    -1,
      10,    -1,    12,    -1,    14,    15,    16,    -1,    -1,    -1,
      51,    21,    -1,    23,    -1,    -1,    -1,    -1,    28,    29,
      30,    31,    -1,    33,     3,     4,     5,     6,     7,     8,
      -1,    10,    -1,    12,    -1,    14,    15,    16,    -1,    -1,
      -1,    51,    21,    -1,    23,    -1,    -1,    -1,    -1,    28,
      29,    30,    31,    -1,    33,     3,     4,     5,     6,     7,
       8,    -1,    10,    -1,    12,    -1,    14,    15,    16,    -1,
      -1,    -1,    51,    21,    -1,    23,    -1,    -1,    -1,    -1,
      28,    29,    30,    31,    -1,    33,     3,     4,     5,     6,
       7,     8,    -1,    10,    -1,    12,    -1,    14,    15,    16,
      -1,    -1,    -1,    51,    21,    -1,    23,    -1,    -1,    -1,
      -1,    28,    29,    30,    31,    -1,    33,     3,     4,     5,
       6,     7,     8,    -1,    10,    -1,    12,    -1,    14,    15,
      16,    -1,    -1,    -1,    51,    21,    -1,    23,    -1,    -1,
      -1,    -1,    28,    29,    30,    31,    -1,    33,     3,     4,
       5,     6,     7,     8,    -1,    10,    -1,    12,    -1,    14,
      15,    16,    -1,    -1,    -1,    51,    21,    -1,    23,    -1,
      -1,    -1,    -1,    28,    29,    30,    31,    -1,    33,     3,
       4,     5,    -1,     7,     8,    -1,    -1,    -1,    -1,    -1,
      14,    15,    16,    -1,    -1,    -1,    51,    21,    -1,    23,
      24,    -1,    -1,    -1,    28,    29,    30,    31,    -1,    33,
       3,     4,     5,     5,     6,     8,     8,    -1,    10,    -1,
      12,    14,    15,    16,    -1,    -1,    -1,    51,    21,    22,
      23,    23,    -1,    -1,    -1,    28,    29,    30,    31,    31,
      33,     3,     4,     5,    -1,     7,     8,    -1,    -1,    -1,
      -1,    -1,    14,    15,    16,    -1,    -1,    -1,    51,    21,
      -1,    23,    -1,    -1,    -1,    -1,    28,    29,    30,    31,
      -1,    33,     3,     4,     5,    -1,    -1,     8,    -1,    -1,
      -1,    -1,    -1,    14,    15,    16,    -1,    -1,    -1,    51,
      21,    22,    23,    -1,    -1,    -1,    -1,    28,    29,    30,
      31,    -1,    33,     3,     4,     5,    -1,    -1,     8,    -1,
      -1,    -1,    -1,    -1,    14,    15,    16,    -1,    -1,    -1,
      51,    21,    22,    23,    -1,    -1,    -1,    -1,    28,    29,
      30,    31,    -1,    33,     3,     4,     5,    -1,    -1,     8,
      -1,    -1,    -1,    -1,    -1,    14,    15,    16,    -1,    -1,
      -1,    51,    21,    22,    23,    -1,    -1,    -1,    -1,    28,
      29,    30,    31,    -1,    33,     3,     4,     5,    -1,    -1,
       8,    -1,    -1,    -1,    -1,    -1,    14,    15,    16,    -1,
      -1,    -1,    51,    21,    22,    23,    -1,    -1,    -1,    -1,
      28,    29,    30,    31,    -1,    33,     3,     4,     5,    -1,
       7,     8,    -1,    -1,    -1,    -1,    -1,    14,    15,    16,
      -1,    -1,    -1,    51,    21,    -1,    23,    -1,    -1,    -1,
      -1,    28,    29,    30,    31,    -1,    33,     3,     4,     5,
      -1,     7,     8,    -1,    -1,    -1,    -1,    -1,    14,    15,
      16,    -1,    -1,    -1,    51,    21,    -1,    23,    -1,    -1,
      -1,    -1,    28,    29,    30,    31,    -1,    33,     3,     4,
       5,    -1,    -1,     8,    -1,    -1,    -1,    -1,    -1,    14,
      15,    16,    -1,    -1,    -1,    51,    21,    22,    23,    -1,
      -1,    -1,    -1,    28,    29,    30,    31,    -1,    33,     3,
       4,     5,    -1,     7,     8,    -1,    -1,    -1,    -1,    -1,
      14,    15,    16,    -1,    -1,    -1,    51,    21,    -1,    23,
      -1,    -1,    -1,    -1,    28,    29,    30,    31,    -1,    33,
       3,     4,     5,    -1,    -1,     8,    -1,    -1,    -1,    -1,
      -1,    14,    15,    16,    -1,    -1,    -1,    51,    21,    -1,
      23,    -1,    -1,    -1,    -1,    28,    29,    30,    31,    -1,
      33,     3,     4,     5,    -1,    -1,     8,    -1,    -1,    -1,
      -1,    -1,    14,    15,    16,    -1,    -1,    -1,    51,    21,
      -1,    23,    -1,    -1,    -1,    -1,    28,    29,    30,    31,
      -1,    33,    -1,    -1,    -1,    -1,    -1,    19,    20,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    51,
      32,    33,    34,    35,    36,    -1,    38,    39,    40,    41,
      19,    20,    44,    45,    46,    47,    48,    49,    50,    -1,
      52,    -1,    -1,    32,    33,    34,    35,    36,    -1,    38,
      39,    40,    41,    -1,    -1,    44,    45,    46,    47,    48,
      49,    50,    -1,    52,    19,    20,    -1,    22,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    32,    33,    34,
      35,    36,    -1,    38,    39,    40,    19,    20,    -1,    44,
      45,    46,    47,    48,    49,    50,    -1,    52,    -1,    32,
      33,    34,    35,    36,    -1,    38,    39,    40,    41,    19,
      20,    44,    45,    46,    47,    48,    49,    50,    -1,    52,
      -1,    -1,    32,    33,    34,    35,    36,    -1,    38,    39,
      40,    41,    19,    20,    44,    45,    46,    47,    48,    49,
      50,    -1,    52,    -1,    -1,    32,    33,    34,    35,    36,
      -1,    38,    39,    40,    41,    -1,    -1,    44,    45,    46,
      47,    48,    49,    50,    -1,    52,    19,    20,    -1,    22,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    32,
      33,    34,    35,    36,    -1,    38,    39,    40,    19,    20,
      -1,    44,    45,    46,    47,    48,    49,    50,    -1,    52,
      -1,    32,    33,    34,    35,    36,    -1,    38,    39,    40,
      41,    19,    20,    44,    45,    46,    47,    48,    49,    50,
      -1,    52,    -1,    -1,    32,    33,    34,    35,    36,    -1,
      38,    39,    40,    41,    19,    20,    44,    45,    46,    47,
      48,    49,    50,    -1,    52,    -1,    -1,    32,    33,    34,
      35,    36,    -1,    38,    39,    40,    41,    -1,    -1,    44,
      45,    46,    47,    48,    49,    50,    -1,    52,    19,    20,
      -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    32,    33,    34,    35,    36,    -1,    38,    39,    40,
      -1,    -1,    -1,    44,    45,    46,    47,    48,    49,    50,
      -1,    52,    19,    20,    -1,    22,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    32,    33,    34,    35,    36,
      -1,    38,    39,    40,    19,    20,    -1,    44,    45,    46,
      47,    48,    49,    50,    -1,    52,    -1,    32,    33,    34,
      35,    36,    -1,    38,    39,    40,    41,    -1,    -1,    44,
      45,    46,    47,    48,    49,    50,    -1,    52,    19,    20,
      -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    32,    33,    34,    35,    36,    -1,    38,    39,    40,
      19,    20,    -1,    44,    45,    46,    47,    48,    49,    50,
      -1,    52,    -1,    32,    33,    34,    35,    36,    -1,    38,
      39,    40,    41,    19,    20,    44,    45,    46,    47,    48,
      49,    50,    -1,    52,    -1,    -1,    32,    33,    34,    35,
      36,    -1,    38,    39,    40,    41,    19,    20,    44,    45,
      46,    47,    48,    49,    50,    -1,    52,    -1,    -1,    32,
      33,    34,    35,    36,    -1,    38,    39,    40,    41,    19,
      20,    44,    45,    46,    47,    48,    49,    50,    -1,    52,
      -1,    -1,    32,    33,    34,    35,    36,    -1,    38,    39,
      40,    41,    19,    20,    44,    45,    46,    47,    48,    49,
      50,    -1,    52,    -1,    -1,    32,    33,    34,    35,    36,
      -1,    38,    39,    40,    41,    19,    20,    44,    45,    46,
      47,    48,    49,    50,    -1,    52,    -1,    -1,    32,    33,
      34,    35,    36,    -1,    38,    39,    40,    41,    19,    20,
      44,    45,    46,    47,    48,    49,    50,    -1,    52,    -1,
      -1,    32,    33,    34,    35,    36,    -1,    38,    39,    40,
      41,    19,    20,    44,    45,    46,    47,    48,    49,    50,
      -1,    52,    -1,    -1,    32,    33,    34,    35,    36,    -1,
      38,    39,    40,    41,    19,    20,    44,    45,    46,    47,
      48,    49,    50,    -1,    52,    -1,    -1,    32,    33,    34,
      35,    36,    -1,    38,    39,    40,    41,    19,    20,    44,
      45,    46,    47,    48,    49,    50,    -1,    52,    -1,    -1,
      32,    33,    34,    35,    36,    -1,    38,    39,    40,    19,
      20,    -1,    44,    45,    46,    47,    48,    49,    50,    -1,
      52,    -1,    32,    33,    34,    35,    36,    -1,    38,    39,
      40,    19,    20,    -1,    44,    45,    46,    47,    48,    49,
      -1,    -1,    52,    -1,    32,    33,    34,    35,    36,    -1,
      38,    39,    40,    19,    20,    -1,    44,    -1,    46,    47,
      48,    49,    -1,    -1,    52,    -1,    32,    33,    34,    35,
      36,    -1,    38,    39,    40,    -1,    -1,    -1,    -1,    -1,
      46,    -1,    48,    49,    -1,    -1,    52,     3,     4,     5,
       6,    -1,     8,    -1,    10,    -1,    12,    -1,    14,    15,
      16,     5,     6,     7,     8,    -1,    10,    23,    12,    -1,
      -1,    -1,    28,    29,    30,    31,    -1,    -1,    -1,    23,
      24,     5,     6,     7,     8,    -1,    10,    31,    12,    -1,
       5,     6,     7,     8,    -1,    10,    -1,    12,    -1,    23,
      24,    -1,    -1,    -1,    -1,    -1,    -1,    31,    23,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    31
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,     4,     5,     6,     7,     8,    10,    12,    14,
      15,    16,    21,    23,    28,    29,    30,    31,    33,    51,
      56,    57,    58,    59,    64,    65,    66,    21,    53,     5,
       8,    23,    31,    64,     5,    61,    62,     5,     5,    21,
       5,    64,    58,    64,     7,    58,    60,    64,    72,    21,
      64,    21,    64,    64,     0,     5,     7,     8,    23,    31,
      58,     7,    19,    20,    32,    33,    34,    35,    36,    38,
      39,    40,    41,    44,    45,    46,    47,    48,    49,    50,
      52,    53,    54,    22,    64,    67,    64,    21,    53,    61,
       7,    21,    41,    37,    43,     7,     9,    42,    21,    13,
      21,    23,    22,    67,    21,    33,    58,    64,    71,    22,
      22,     7,    24,    58,    41,     7,    24,    64,    64,    71,
       5,    21,    53,    61,     7,    21,     7,     5,    25,    26,
      27,    28,    29,     5,    64,    64,    64,    64,    64,    64,
      64,    64,    64,    64,    64,    64,    64,    64,    64,    64,
      64,     5,    41,     7,    22,    42,    41,    22,    67,    64,
       7,     9,     5,     5,    25,    26,    27,    63,    64,     9,
      62,     7,    58,    64,     7,    62,     5,    68,    69,    70,
       5,     5,    22,    69,    73,    22,    22,    67,    64,     7,
      17,    18,    58,    41,    22,     9,    22,    67,    64,     7,
       9,     5,    21,    64,    41,     7,    64,    22,     9,     7,
      64,     9,    43,     7,    58,    64,    58,    64,    41,    62,
      37,     7,    22,    42,     7,    22,    42,    21,    23,    23,
      22,     5,    24,    74,    22,    17,    18,    64,     7,    71,
      71,    64,    22,     9,     7,    64,     9,    22,    67,    64,
       7,    64,    64,    64,    64,    58,    64,    41,    41,    63,
       5,    11,    23,     5,     7,    70,    11,    23,    37,     7,
      70,    67,    73,    73,    13,    23,    21,    37,    43,    64,
       7,    71,    71,    71,    22,     7,    64,    64,    64,    22,
      64,    22,    41,    64,    60,     5,    64,    60,    63,    70,
      22,    24,    24,     5,    73,    22,    69,    63,    64,    71,
      71,     7,    18,    71,    64,    22,    71,    41,    24,    41,
      24,    11,    23,    23,    21,    23,    24,    11,    37,    22,
      43,    41,     7,    18,    18,     7,    71,    41,    71,    64,
      60,    73,    67,    73,    64,    63,    11,    37,    64,    18,
       7,    71,    71,    71,    41,    24,    24,    22,    24,    41,
      11,    64,    63,    41,     7,    71,    71,    23,    64,    41,
      11,    71,    73,    41,    64,    24,    41
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    55,    56,    56,    56,    56,    57,    57,    57,    57,
      57,    57,    58,    58,    58,    58,    58,    58,    58,    58,
      58,    58,    58,    58,    58,    58,    58,    58,    58,    58,
      58,    58,    58,    58,    58,    58,    58,    58,    58,    58,
      58,    58,    58,    58,    58,    58,    58,    58,    59,    60,
      60,    60,    60,    60,    60,    61,    61,    61,    61,    62,
      62,    63,    63,    63,    63,    64,    64,    64,    64,    64,
      64,    64,    64,    64,    64,    64,    64,    64,    64,    64,
      64,    64,    64,    64,    64,    64,    64,    64,    64,    64,
      64,    64,    64,    64,    64,    64,    64,    64,    64,    64,
      64,    64,    64,    64,    64,    64,    64,    64,    64,    64,
      64,    64,    65,    65,    65,    65,    65,    65,    66,    66,
      66,    66,    66,    66,    66,    66,    66,    66,    66,    67,
      67,    67,    67,    68,    68,    68,    68,    68,    69,    69,
      69,    69,    69,    70,    70,    71,    71,    72,    72,    72,
      72,    72,    72,    72,    73,    73,    74,    74,    74,    74,
      74,    74
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     2,     1,     0,     3,     2,     2,     2,
       1,     1,     3,     2,     4,     3,     5,     4,     4,     3,
       4,     5,     5,     6,     5,     4,     6,     5,     6,     5,
       7,     6,     1,     8,    10,     8,     8,     8,    10,     8,
       7,     8,     7,     5,     7,    10,    13,    10,     3,     1,
       2,     3,     2,     1,     0,     1,     4,     3,     3,     3,
       5,     1,     1,     1,     1,     1,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     2,
       2,     3,     3,     5,     6,     6,     7,     8,     9,     9,
       9,    10,    10,    11,     4,     5,     5,     6,     5,     3,
       7,     3,     1,     3,     6,     5,     4,     3,     1,     1,
       1,     1,     1,     3,     3,     5,     4,     4,     3,     1,
       4,     3,     3,     1,     4,     3,     3,     0,     1,     4,
       3,     3,     0,     1,     3,     1,     1,     1,     2,     2,
       3,     2,     1,     0,     0,     2,     4,     6,     7,     6,
       9,     8
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 3: /* program: expression SEMICOLON  */
#line 80 "parser.y"
                           {

        program.Statements.push_back(new ExpressionStatementNode((yyvsp[-1].expression)));
    }
#line 1708 "hulk/parser.cpp"
    break;

  case 4: /* program: expression  */
#line 84 "parser.y"
                 {

        program.Statements.push_back(new ExpressionStatementNode((yyvsp[0].expression)));
    }
#line 1717 "hulk/parser.cpp"
    break;

  case 5: /* program: %empty  */
#line 88 "parser.y"
      { }
#line 1723 "hulk/parser.cpp"
    break;

  case 6: /* statement_list: statement_list statement EOL  */
#line 93 "parser.y"
                                 { program.Statements.push_back((yyvsp[-1].statement)); }
#line 1729 "hulk/parser.cpp"
    break;

  case 7: /* statement_list: statement_list statement  */
#line 94 "parser.y"
                               { program.Statements.push_back((yyvsp[0].statement)); }
#line 1735 "hulk/parser.cpp"
    break;

  case 8: /* statement_list: statement_list EOL  */
#line 95 "parser.y"
                         {  }
#line 1741 "hulk/parser.cpp"
    break;

  case 9: /* statement_list: statement EOL  */
#line 96 "parser.y"
                    { program.Statements.push_back((yyvsp[-1].statement)); }
#line 1747 "hulk/parser.cpp"
    break;

  case 10: /* statement_list: statement  */
#line 97 "parser.y"
                { program.Statements.push_back((yyvsp[0].statement)); }
#line 1753 "hulk/parser.cpp"
    break;

  case 11: /* statement_list: EOL  */
#line 98 "parser.y"
          { }
#line 1759 "hulk/parser.cpp"
    break;

  case 12: /* statement: PRINT expression SEMICOLON  */
#line 102 "parser.y"
                               { (yyval.statement) = new PrintStatementNode((yyvsp[-1].expression)); }
#line 1765 "hulk/parser.cpp"
    break;

  case 13: /* statement: PRINT expression  */
#line 103 "parser.y"
                       { (yyval.statement) = new PrintStatementNode((yyvsp[0].expression)); }
#line 1771 "hulk/parser.cpp"
    break;

  case 14: /* statement: IDENTIFIER ASSIGN expression SEMICOLON  */
#line 104 "parser.y"
                                             { (yyval.statement) = new ExpressionStatementNode(new AssignmentNode((yyvsp[-3].string), (yyvsp[-1].expression))); free((yyvsp[-3].string)); }
#line 1777 "hulk/parser.cpp"
    break;

  case 15: /* statement: IDENTIFIER ASSIGN expression  */
#line 105 "parser.y"
                                   { (yyval.statement) = new ExpressionStatementNode(new AssignmentNode((yyvsp[-2].string), (yyvsp[0].expression))); free((yyvsp[-2].string)); }
#line 1783 "hulk/parser.cpp"
    break;

  case 16: /* statement: IDENTIFIER LPAREN expression_list RPAREN SEMICOLON  */
#line 106 "parser.y"
                                                         { (yyval.statement) = new ExpressionStatementNode(new FuncCallNode((yyvsp[-4].string), *(yyvsp[-2].explist))); delete (yyvsp[-2].explist); free((yyvsp[-4].string)); }
#line 1789 "hulk/parser.cpp"
    break;

  case 17: /* statement: IDENTIFIER LPAREN RPAREN SEMICOLON  */
#line 107 "parser.y"
                                         { (yyval.statement) = new ExpressionStatementNode(new FuncCallNode((yyvsp[-3].string), {})); free((yyvsp[-3].string)); }
#line 1795 "hulk/parser.cpp"
    break;

  case 18: /* statement: IDENTIFIER LPAREN expression_list RPAREN  */
#line 108 "parser.y"
                                               { (yyval.statement) = new ExpressionStatementNode(new FuncCallNode((yyvsp[-3].string), *(yyvsp[-1].explist))); delete (yyvsp[-1].explist); free((yyvsp[-3].string)); }
#line 1801 "hulk/parser.cpp"
    break;

  case 19: /* statement: IDENTIFIER LPAREN RPAREN  */
#line 109 "parser.y"
                               { (yyval.statement) = new ExpressionStatementNode(new FuncCallNode((yyvsp[-2].string), {})); free((yyvsp[-2].string)); }
#line 1807 "hulk/parser.cpp"
    break;

  case 20: /* statement: LET declaration_list IN statement  */
#line 110 "parser.y"
                                        {
        ASTNode* stmt = (yyvsp[0].statement);
        for (int i = (yyvsp[-2].decllist)->size()-1; i >= 0; i--) {
            VarDeclPair pair = (*(yyvsp[-2].decllist))[i];
            stmt = new LetVarNode(pair.id.c_str(), pair.expr, stmt, pair.type);
        }
        (yyval.statement) = dynamic_cast<StatementNode*>(stmt);
        delete (yyvsp[-2].decllist);
    }
#line 1821 "hulk/parser.cpp"
    break;

  case 21: /* statement: LET declaration_list EOL IN statement  */
#line 119 "parser.y"
                                            {
        ASTNode* stmt = (yyvsp[0].statement);
        for (int i = (yyvsp[-3].decllist)->size()-1; i >= 0; i--) {
            VarDeclPair pair = (*(yyvsp[-3].decllist))[i];
            stmt = new LetVarNode(pair.id.c_str(), pair.expr, stmt, pair.type);
        }
        (yyval.statement) = dynamic_cast<StatementNode*>(stmt);
        delete (yyvsp[-3].decllist);
    }
#line 1835 "hulk/parser.cpp"
    break;

  case 22: /* statement: LET declaration_list IN EOL statement  */
#line 128 "parser.y"
                                            {
        ASTNode* stmt = (yyvsp[0].statement);
        for (int i = (yyvsp[-3].decllist)->size()-1; i >= 0; i--) {
            VarDeclPair pair = (*(yyvsp[-3].decllist))[i];
            stmt = new LetVarNode(pair.id.c_str(), pair.expr, stmt, pair.type);
        }
        (yyval.statement) = dynamic_cast<StatementNode*>(stmt);
        delete (yyvsp[-3].decllist);
    }
#line 1849 "hulk/parser.cpp"
    break;

  case 23: /* statement: LET declaration_list EOL IN EOL statement  */
#line 137 "parser.y"
                                                {
        ASTNode* stmt = (yyvsp[0].statement);
        for (int i = (yyvsp[-4].decllist)->size()-1; i >= 0; i--) {
            VarDeclPair pair = (*(yyvsp[-4].decllist))[i];
            stmt = new LetVarNode(pair.id.c_str(), pair.expr, stmt, pair.type);
        }
        (yyval.statement) = dynamic_cast<StatementNode*>(stmt);
        delete (yyvsp[-4].decllist);
    }
#line 1863 "hulk/parser.cpp"
    break;

  case 24: /* statement: LET declaration_list IN expression SEMICOLON  */
#line 146 "parser.y"
                                                   {
        (yyval.statement) = new ExpressionStatementNode(new LetExprNode(*(yyvsp[-3].decllist), (yyvsp[-1].expression)));
        delete (yyvsp[-3].decllist);
    }
#line 1872 "hulk/parser.cpp"
    break;

  case 25: /* statement: LET declaration_list IN expression  */
#line 150 "parser.y"
                                         {
        (yyval.statement) = new ExpressionStatementNode(new LetExprNode(*(yyvsp[-2].decllist), (yyvsp[0].expression)));
        delete (yyvsp[-2].decllist);
    }
#line 1881 "hulk/parser.cpp"
    break;

  case 26: /* statement: LET declaration_list EOL IN expression SEMICOLON  */
#line 154 "parser.y"
                                                       {
        (yyval.statement) = new ExpressionStatementNode(new LetExprNode(*(yyvsp[-4].decllist), (yyvsp[-1].expression)));
        delete (yyvsp[-4].decllist);
    }
#line 1890 "hulk/parser.cpp"
    break;

  case 27: /* statement: LET declaration_list EOL IN expression  */
#line 158 "parser.y"
                                             {
        (yyval.statement) = new ExpressionStatementNode(new LetExprNode(*(yyvsp[-3].decllist), (yyvsp[0].expression)));
        delete (yyvsp[-3].decllist);
    }
#line 1899 "hulk/parser.cpp"
    break;

  case 28: /* statement: LET declaration_list IN EOL expression SEMICOLON  */
#line 162 "parser.y"
                                                       {
        (yyval.statement) = new ExpressionStatementNode(new LetExprNode(*(yyvsp[-4].decllist), (yyvsp[-1].expression)));
        delete (yyvsp[-4].decllist);
    }
#line 1908 "hulk/parser.cpp"
    break;

  case 29: /* statement: LET declaration_list IN EOL expression  */
#line 166 "parser.y"
                                             {
        (yyval.statement) = new ExpressionStatementNode(new LetExprNode(*(yyvsp[-3].decllist), (yyvsp[0].expression)));
        delete (yyvsp[-3].decllist);
    }
#line 1917 "hulk/parser.cpp"
    break;

  case 30: /* statement: LET declaration_list EOL IN EOL expression SEMICOLON  */
#line 170 "parser.y"
                                                           {
        (yyval.statement) = new ExpressionStatementNode(new LetExprNode(*(yyvsp[-5].decllist), (yyvsp[-1].expression)));
        delete (yyvsp[-5].decllist);
    }
#line 1926 "hulk/parser.cpp"
    break;

  case 31: /* statement: LET declaration_list EOL IN EOL expression  */
#line 174 "parser.y"
                                                 {
        (yyval.statement) = new ExpressionStatementNode(new LetExprNode(*(yyvsp[-4].decllist), (yyvsp[0].expression)));
        delete (yyvsp[-4].decllist);
    }
#line 1935 "hulk/parser.cpp"
    break;

  case 32: /* statement: statement_block  */
#line 178 "parser.y"
                      { (yyval.statement) = new BlockNode(*(yyvsp[0].stmtlist)); delete (yyvsp[0].stmtlist); }
#line 1941 "hulk/parser.cpp"
    break;

  case 33: /* statement: FUNCTION IDENTIFIER LPAREN identifier_list RPAREN ARROW expression SEMICOLON  */
#line 179 "parser.y"
                                                                                   {
        (yyval.statement) = new DefFuncNode((yyvsp[-6].string), *(yyvsp[-4].idlist), (yyvsp[-1].expression));
        delete (yyvsp[-4].idlist);
    }
#line 1950 "hulk/parser.cpp"
    break;

  case 34: /* statement: FUNCTION IDENTIFIER LPAREN parameter_list RPAREN COLON type ARROW expression SEMICOLON  */
#line 183 "parser.y"
                                                                                             {

        std::vector<Parameter> params;
        for (const auto& pair : *(yyvsp[-6].decllist)) {
            params.emplace_back(pair.id, pair.type);
        }
        (yyval.statement) = new DefFuncNode((yyvsp[-8].string), params, (yyvsp[-1].expression), (yyvsp[-3].type));
        delete (yyvsp[-6].decllist);
        free((yyvsp[-8].string));
    }
#line 1965 "hulk/parser.cpp"
    break;

  case 35: /* statement: FUNCTION IDENTIFIER LPAREN parameter_list RPAREN ARROW expression SEMICOLON  */
#line 193 "parser.y"
                                                                                  {

        std::vector<Parameter> params;
        for (const auto& pair : *(yyvsp[-4].decllist)) {
            params.emplace_back(pair.id, pair.type);
        }
        (yyval.statement) = new DefFuncNode((yyvsp[-6].string), params, (yyvsp[-1].expression), nullptr);
        delete (yyvsp[-4].decllist);
        free((yyvsp[-6].string));
    }
#line 1980 "hulk/parser.cpp"
    break;

  case 36: /* statement: FUNCTION IDENTIFIER LPAREN identifier_list RPAREN LBRACE statement_list_block RBRACE  */
#line 203 "parser.y"
                                                                                           {
        std::vector<Parameter> params;
        for (const auto& id : *(yyvsp[-4].idlist)) {
            params.emplace_back(id, nullptr);
        }
        (yyval.statement) = new DefFuncNode((yyvsp[-6].string), params, *(yyvsp[-1].stmtlist), nullptr);
        delete (yyvsp[-4].idlist);
        delete (yyvsp[-1].stmtlist);
        free((yyvsp[-6].string));
    }
#line 1995 "hulk/parser.cpp"
    break;

  case 37: /* statement: FUNCTION IDENTIFIER LPAREN parameter_list RPAREN LBRACE statement_list_block RBRACE  */
#line 213 "parser.y"
                                                                                          {
        std::vector<Parameter> params;
        for (const auto& pair : *(yyvsp[-4].decllist)) {
            params.emplace_back(pair.id, pair.type);
        }
        (yyval.statement) = new DefFuncNode((yyvsp[-6].string), params, *(yyvsp[-1].stmtlist), nullptr);
        delete (yyvsp[-4].decllist);
        delete (yyvsp[-1].stmtlist);
        free((yyvsp[-6].string));
    }
#line 2010 "hulk/parser.cpp"
    break;

  case 38: /* statement: FUNCTION IDENTIFIER LPAREN parameter_list RPAREN COLON type LBRACE statement_list_block RBRACE  */
#line 223 "parser.y"
                                                                                                     {
        std::vector<Parameter> params;
        for (const auto& pair : *(yyvsp[-6].decllist)) {
            params.emplace_back(pair.id, pair.type);
        }
        (yyval.statement) = new DefFuncNode((yyvsp[-8].string), params, *(yyvsp[-1].stmtlist), (yyvsp[-3].type));
        delete (yyvsp[-6].decllist);
        delete (yyvsp[-1].stmtlist);
        free((yyvsp[-8].string));
    }
#line 2025 "hulk/parser.cpp"
    break;

  case 39: /* statement: FOR LPAREN IDENTIFIER IN expression RPAREN ast_construct SEMICOLON  */
#line 233 "parser.y"
                                                                         {

        if (auto* funcCall = dynamic_cast<FuncCallNode*>((yyvsp[-3].expression))) {
            if (funcCall->identifier == "range" && funcCall->args.size() == 2) {

                ForRangeNode* forNode = new ForRangeNode((yyvsp[-5].string), funcCall->args[0], funcCall->args[1], (yyvsp[-1].ast_node));
                (yyval.statement) = new ExpressionStatementNode(forNode);
                funcCall->args.clear();
                delete funcCall;
                free((yyvsp[-5].string));
            } else {

                (yyval.statement) = new ExpressionStatementNode(new NumberNode(0));
                free((yyvsp[-5].string));
            }
        } else {
            (yyval.statement) = new ExpressionStatementNode(new NumberNode(0));
            free((yyvsp[-5].string));
        }
    }
#line 2050 "hulk/parser.cpp"
    break;

  case 40: /* statement: FOR LPAREN IDENTIFIER IN expression RPAREN ast_construct  */
#line 253 "parser.y"
                                                               {

        if (auto* funcCall = dynamic_cast<FuncCallNode*>((yyvsp[-2].expression))) {
            if (funcCall->identifier == "range" && funcCall->args.size() == 2) {

                ForRangeNode* forNode = new ForRangeNode((yyvsp[-4].string), funcCall->args[0], funcCall->args[1], (yyvsp[0].ast_node));
                (yyval.statement) = new ExpressionStatementNode(forNode);
                funcCall->args.clear();
                delete funcCall;
                free((yyvsp[-4].string));
            } else {

                (yyval.statement) = new ExpressionStatementNode(new NumberNode(0));
                free((yyvsp[-4].string));
            }
        } else {
            (yyval.statement) = new ExpressionStatementNode(new NumberNode(0));
            free((yyvsp[-4].string));
        }
    }
#line 2075 "hulk/parser.cpp"
    break;

  case 41: /* statement: TYPE IDENTIFIER LPAREN parameter_list RPAREN LBRACE type_member_list RBRACE  */
#line 273 "parser.y"
                                                                                  {

        std::vector<Parameter> params;
        for (const auto& pair : *(yyvsp[-4].decllist)) {
            params.emplace_back(pair.id, pair.type);
        }
        (yyval.statement) = new TypeDefNode((yyvsp[-6].string), params, *(yyvsp[-1].typemembers));
        delete (yyvsp[-4].decllist);
        delete (yyvsp[-1].typemembers);
        free((yyvsp[-6].string));
    }
#line 2091 "hulk/parser.cpp"
    break;

  case 42: /* statement: TYPE IDENTIFIER LPAREN RPAREN LBRACE type_member_list RBRACE  */
#line 284 "parser.y"
                                                                   {

        std::vector<Parameter> params;
        (yyval.statement) = new TypeDefNode((yyvsp[-5].string), params, *(yyvsp[-1].typemembers));
        delete (yyvsp[-1].typemembers);
        free((yyvsp[-5].string));
    }
#line 2103 "hulk/parser.cpp"
    break;

  case 43: /* statement: TYPE IDENTIFIER LBRACE type_member_list RBRACE  */
#line 291 "parser.y"
                                                     {

        std::vector<Parameter> params;
        (yyval.statement) = new TypeDefNode((yyvsp[-3].string), params, *(yyvsp[-1].typemembers));
        delete (yyvsp[-1].typemembers);
        free((yyvsp[-3].string));
    }
#line 2115 "hulk/parser.cpp"
    break;

  case 44: /* statement: TYPE IDENTIFIER INHERITS IDENTIFIER LBRACE type_member_list RBRACE  */
#line 298 "parser.y"
                                                                         {

        std::vector<Parameter> params;
        std::vector<ExpressionNode*> parentArgs;
        (yyval.statement) = new TypeDefNode((yyvsp[-5].string), params, (yyvsp[-3].string), parentArgs, *(yyvsp[-1].typemembers));
        delete (yyvsp[-1].typemembers);
        free((yyvsp[-5].string));
        free((yyvsp[-3].string));
    }
#line 2129 "hulk/parser.cpp"
    break;

  case 45: /* statement: TYPE IDENTIFIER LPAREN parameter_list RPAREN INHERITS IDENTIFIER LBRACE type_member_list RBRACE  */
#line 307 "parser.y"
                                                                                                      {

        std::vector<Parameter> params;
        for (const auto& pair : *(yyvsp[-6].decllist)) {
            params.emplace_back(pair.id, pair.type);
        }
        std::vector<ExpressionNode*> parentArgs;
        (yyval.statement) = new TypeDefNode((yyvsp[-8].string), params, (yyvsp[-3].string), parentArgs, *(yyvsp[-1].typemembers));
        delete (yyvsp[-6].decllist);
        delete (yyvsp[-1].typemembers);
        free((yyvsp[-8].string));
        free((yyvsp[-3].string));
    }
#line 2147 "hulk/parser.cpp"
    break;

  case 46: /* statement: TYPE IDENTIFIER LPAREN parameter_list RPAREN INHERITS IDENTIFIER LPAREN expression_list RPAREN LBRACE type_member_list RBRACE  */
#line 320 "parser.y"
                                                                                                                                    {

        std::vector<Parameter> params;
        for (const auto& pair : *(yyvsp[-9].decllist)) {
            params.emplace_back(pair.id, pair.type);
        }
        (yyval.statement) = new TypeDefNode((yyvsp[-11].string), params, (yyvsp[-6].string), *(yyvsp[-4].explist), *(yyvsp[-1].typemembers));
        delete (yyvsp[-9].decllist);
        delete (yyvsp[-4].explist);
        delete (yyvsp[-1].typemembers);
        free((yyvsp[-11].string));
        free((yyvsp[-6].string));
    }
#line 2165 "hulk/parser.cpp"
    break;

  case 47: /* statement: TYPE IDENTIFIER INHERITS IDENTIFIER LPAREN expression_list RPAREN LBRACE type_member_list RBRACE  */
#line 333 "parser.y"
                                                                                                       {

        std::vector<Parameter> params;
        (yyval.statement) = new TypeDefNode((yyvsp[-8].string), params, (yyvsp[-6].string), *(yyvsp[-4].explist), *(yyvsp[-1].typemembers));
        delete (yyvsp[-4].explist);
        delete (yyvsp[-1].typemembers);
        free((yyvsp[-8].string));
        free((yyvsp[-6].string));
    }
#line 2179 "hulk/parser.cpp"
    break;

  case 48: /* statement_block: LBRACE statement_list_block RBRACE  */
#line 345 "parser.y"
                                       { (yyval.stmtlist) = (yyvsp[-1].stmtlist); }
#line 2185 "hulk/parser.cpp"
    break;

  case 49: /* statement_list_block: statement  */
#line 349 "parser.y"
              {
        (yyval.stmtlist) = new std::vector<StatementNode*>;
        (yyval.stmtlist)->push_back((yyvsp[0].statement));
    }
#line 2194 "hulk/parser.cpp"
    break;

  case 50: /* statement_list_block: statement_list_block statement  */
#line 353 "parser.y"
                                     {
        (yyvsp[-1].stmtlist)->push_back((yyvsp[0].statement));
        (yyval.stmtlist) = (yyvsp[-1].stmtlist);
    }
#line 2203 "hulk/parser.cpp"
    break;

  case 51: /* statement_list_block: statement_list_block EOL statement  */
#line 357 "parser.y"
                                         {
        (yyvsp[-2].stmtlist)->push_back((yyvsp[0].statement));
        (yyval.stmtlist) = (yyvsp[-2].stmtlist);
    }
#line 2212 "hulk/parser.cpp"
    break;

  case 52: /* statement_list_block: statement_list_block EOL  */
#line 361 "parser.y"
                               { (yyval.stmtlist) = (yyvsp[-1].stmtlist); }
#line 2218 "hulk/parser.cpp"
    break;

  case 53: /* statement_list_block: EOL  */
#line 362 "parser.y"
          { (yyval.stmtlist) = new std::vector<StatementNode*>; }
#line 2224 "hulk/parser.cpp"
    break;

  case 54: /* statement_list_block: %empty  */
#line 363 "parser.y"
       { (yyval.stmtlist) = new std::vector<StatementNode*>; }
#line 2230 "hulk/parser.cpp"
    break;

  case 55: /* declaration_list: declaration  */
#line 367 "parser.y"
                { (yyval.decllist) = (yyvsp[0].decllist); }
#line 2236 "hulk/parser.cpp"
    break;

  case 56: /* declaration_list: declaration_list COMMA EOL declaration  */
#line 368 "parser.y"
                                             {
        (yyval.decllist) = (yyvsp[-3].decllist);
        for (const auto& pair : *(yyvsp[0].decllist)) {
            (yyval.decllist)->push_back(pair);
        }
        delete (yyvsp[0].decllist);
    }
#line 2248 "hulk/parser.cpp"
    break;

  case 57: /* declaration_list: declaration_list COMMA declaration  */
#line 375 "parser.y"
                                         {
        (yyval.decllist) = (yyvsp[-2].decllist);
        for (const auto& pair : *(yyvsp[0].decllist)) {
            (yyval.decllist)->push_back(pair);
        }
        delete (yyvsp[0].decllist);
    }
#line 2260 "hulk/parser.cpp"
    break;

  case 58: /* declaration_list: declaration_list EOL declaration  */
#line 382 "parser.y"
                                       {
        (yyval.decllist) = (yyvsp[-2].decllist);
        for (const auto& pair : *(yyvsp[0].decllist)) {
            (yyval.decllist)->push_back(pair);
        }
        delete (yyvsp[0].decllist);
    }
#line 2272 "hulk/parser.cpp"
    break;

  case 59: /* declaration: IDENTIFIER EQUALS expression  */
#line 392 "parser.y"
                                 {
        (yyval.decllist) = new std::vector<VarDeclPair>;
        VarDeclPair pair;
        pair.id = (yyvsp[-2].string);
        pair.expr = (yyvsp[0].expression);
        pair.type = Type::getUnknownType();
        (yyval.decllist)->push_back(pair);
        free((yyvsp[-2].string));
    }
#line 2286 "hulk/parser.cpp"
    break;

  case 60: /* declaration: IDENTIFIER COLON type EQUALS expression  */
#line 401 "parser.y"
                                              {
        (yyval.decllist) = new std::vector<VarDeclPair>;
        VarDeclPair pair;
        pair.id = (yyvsp[-4].string);
        pair.expr = (yyvsp[0].expression);
        pair.type = (yyvsp[-2].type);
        (yyval.decllist)->push_back(pair);
        free((yyvsp[-4].string));
    }
#line 2300 "hulk/parser.cpp"
    break;

  case 61: /* type: TYPE_NUMBER  */
#line 413 "parser.y"
                { (yyval.type) = Type::getNumberType(); }
#line 2306 "hulk/parser.cpp"
    break;

  case 62: /* type: TYPE_STRING  */
#line 414 "parser.y"
                  { (yyval.type) = Type::getStringType(); }
#line 2312 "hulk/parser.cpp"
    break;

  case 63: /* type: TYPE_BOOLEAN  */
#line 415 "parser.y"
                   { (yyval.type) = Type::getBooleanType(); }
#line 2318 "hulk/parser.cpp"
    break;

  case 64: /* type: IDENTIFIER  */
#line 416 "parser.y"
                 {

        Type* foundType = getTypeByName((yyvsp[0].string));
        (yyval.type) = foundType;
        free((yyvsp[0].string));
    }
#line 2329 "hulk/parser.cpp"
    break;

  case 65: /* expression: postfix_expression  */
#line 425 "parser.y"
                       { (yyval.expression) = (yyvsp[0].expression); }
#line 2335 "hulk/parser.cpp"
    break;

  case 66: /* expression: expression PLUS expression  */
#line 426 "parser.y"
                                 { (yyval.expression) = new AdditionNode((yyvsp[-2].expression), (yyvsp[0].expression)); }
#line 2341 "hulk/parser.cpp"
    break;

  case 67: /* expression: expression MINUS expression  */
#line 427 "parser.y"
                                  { (yyval.expression) = new SubtractionNode((yyvsp[-2].expression), (yyvsp[0].expression)); }
#line 2347 "hulk/parser.cpp"
    break;

  case 68: /* expression: expression MULTIPLY expression  */
#line 428 "parser.y"
                                     { (yyval.expression) = new MultiplicationNode((yyvsp[-2].expression), (yyvsp[0].expression)); }
#line 2353 "hulk/parser.cpp"
    break;

  case 69: /* expression: expression DIVIDE expression  */
#line 429 "parser.y"
                                   { (yyval.expression) = new DivisionNode((yyvsp[-2].expression), (yyvsp[0].expression)); }
#line 2359 "hulk/parser.cpp"
    break;

  case 70: /* expression: expression MODULO expression  */
#line 430 "parser.y"
                                   { (yyval.expression) = new ModuloNode((yyvsp[-2].expression), (yyvsp[0].expression)); }
#line 2365 "hulk/parser.cpp"
    break;

  case 71: /* expression: expression POWER expression  */
#line 431 "parser.y"
                                  { (yyval.expression) = new PowerNode((yyvsp[-2].expression), (yyvsp[0].expression)); }
#line 2371 "hulk/parser.cpp"
    break;

  case 72: /* expression: expression CONCAT expression  */
#line 432 "parser.y"
                                   { (yyval.expression) = new ConcatenationNode((yyvsp[-2].expression), (yyvsp[0].expression)); }
#line 2377 "hulk/parser.cpp"
    break;

  case 73: /* expression: expression CONCAT_SPACE expression  */
#line 433 "parser.y"
                                         {
        ExpressionNode* space = new StringLiteralNode(strdup(" "));
        ExpressionNode* leftConcat = new ConcatenationNode((yyvsp[-2].expression), space);
        (yyval.expression) = new ConcatenationNode(leftConcat, (yyvsp[0].expression));
    }
#line 2387 "hulk/parser.cpp"
    break;

  case 74: /* expression: expression EQEQ expression  */
#line 438 "parser.y"
                                 { (yyval.expression) = new EqualNode((yyvsp[-2].expression), (yyvsp[0].expression)); }
#line 2393 "hulk/parser.cpp"
    break;

  case 75: /* expression: expression NOTEQ expression  */
#line 439 "parser.y"
                                  { (yyval.expression) = new NotEqualNode((yyvsp[-2].expression), (yyvsp[0].expression)); }
#line 2399 "hulk/parser.cpp"
    break;

  case 76: /* expression: expression LESS expression  */
#line 440 "parser.y"
                                 { (yyval.expression) = new LessNode((yyvsp[-2].expression), (yyvsp[0].expression)); }
#line 2405 "hulk/parser.cpp"
    break;

  case 77: /* expression: expression LESSEQ expression  */
#line 441 "parser.y"
                                   { (yyval.expression) = new LessEqNode((yyvsp[-2].expression), (yyvsp[0].expression)); }
#line 2411 "hulk/parser.cpp"
    break;

  case 78: /* expression: expression GREATER expression  */
#line 442 "parser.y"
                                    { (yyval.expression) = new GreaterNode((yyvsp[-2].expression), (yyvsp[0].expression)); }
#line 2417 "hulk/parser.cpp"
    break;

  case 79: /* expression: expression GREATEREQ expression  */
#line 443 "parser.y"
                                      { (yyval.expression) = new GreaterEqNode((yyvsp[-2].expression), (yyvsp[0].expression)); }
#line 2423 "hulk/parser.cpp"
    break;

  case 80: /* expression: expression IS IDENTIFIER  */
#line 444 "parser.y"
                               { (yyval.expression) = new IsNode((yyvsp[-2].expression), (yyvsp[0].string)); free((yyvsp[0].string)); }
#line 2429 "hulk/parser.cpp"
    break;

  case 81: /* expression: expression IS TYPE_NUMBER  */
#line 445 "parser.y"
                                { (yyval.expression) = new IsNode((yyvsp[-2].expression), "Number"); }
#line 2435 "hulk/parser.cpp"
    break;

  case 82: /* expression: expression IS TYPE_STRING  */
#line 446 "parser.y"
                                { (yyval.expression) = new IsNode((yyvsp[-2].expression), "String"); }
#line 2441 "hulk/parser.cpp"
    break;

  case 83: /* expression: expression IS TYPE_BOOLEAN  */
#line 447 "parser.y"
                                 { (yyval.expression) = new IsNode((yyvsp[-2].expression), "Boolean"); }
#line 2447 "hulk/parser.cpp"
    break;

  case 84: /* expression: expression IS TRUE  */
#line 448 "parser.y"
                         { (yyval.expression) = new IsNode((yyvsp[-2].expression), "Boolean"); }
#line 2453 "hulk/parser.cpp"
    break;

  case 85: /* expression: expression IS FALSE  */
#line 449 "parser.y"
                          { (yyval.expression) = new IsNode((yyvsp[-2].expression), "Boolean"); }
#line 2459 "hulk/parser.cpp"
    break;

  case 86: /* expression: expression AS IDENTIFIER  */
#line 450 "parser.y"
                               { (yyval.expression) = new AsNode((yyvsp[-2].expression), (yyvsp[0].string)); free((yyvsp[0].string)); }
#line 2465 "hulk/parser.cpp"
    break;

  case 87: /* expression: expression AND expression  */
#line 451 "parser.y"
                                { (yyval.expression) = new AndNode((yyvsp[-2].expression), (yyvsp[0].expression)); }
#line 2471 "hulk/parser.cpp"
    break;

  case 88: /* expression: expression OR expression  */
#line 452 "parser.y"
                               { (yyval.expression) = new OrNode((yyvsp[-2].expression), (yyvsp[0].expression)); }
#line 2477 "hulk/parser.cpp"
    break;

  case 89: /* expression: NOT expression  */
#line 453 "parser.y"
                     { (yyval.expression) = new NotNode((yyvsp[0].expression)); }
#line 2483 "hulk/parser.cpp"
    break;

  case 90: /* expression: MINUS expression  */
#line 454 "parser.y"
                                 { (yyval.expression) = new UnaryMinusNode((yyvsp[0].expression)); }
#line 2489 "hulk/parser.cpp"
    break;

  case 91: /* expression: IDENTIFIER ASSIGN expression  */
#line 455 "parser.y"
                                   { (yyval.expression) = new AssignmentNode((yyvsp[-2].string), (yyvsp[0].expression)); free((yyvsp[-2].string)); }
#line 2495 "hulk/parser.cpp"
    break;

  case 92: /* expression: postfix_expression ASSIGN expression  */
#line 456 "parser.y"
                                           { 
        if (auto memberAccess = dynamic_cast<MemberAccessNode*>((yyvsp[-2].expression))) {
            (yyval.expression) = new MemberAssignmentNode(memberAccess->object, memberAccess->memberName, (yyvsp[0].expression));
            delete memberAccess;
        } else if (auto selfMemberAccess = dynamic_cast<SelfMemberAccessNode*>((yyvsp[-2].expression))) {
            (yyval.expression) = new SelfMemberAssignmentNode(selfMemberAccess->attributeName, (yyvsp[0].expression));
            delete selfMemberAccess;
        } else {
            (yyval.expression) = new AssignmentNode("", (yyvsp[0].expression));
        }
    }
#line 2511 "hulk/parser.cpp"
    break;

  case 93: /* expression: IF expression ast_construct ELSE ast_construct  */
#line 467 "parser.y"
                                                     {
            ConditionalNode* condNode = new ConditionalNode();
            condNode->addBranch((yyvsp[-3].expression), (yyvsp[-2].ast_node));
            condNode->setElse((yyvsp[0].ast_node));
            (yyval.expression) = condNode ;
        }
#line 2522 "hulk/parser.cpp"
    break;

  case 94: /* expression: IF expression ast_construct EOL ELSE ast_construct  */
#line 473 "parser.y"
                                                         {
            ConditionalNode* condNode = new ConditionalNode();
            condNode->addBranch((yyvsp[-4].expression), (yyvsp[-3].ast_node));
            condNode->setElse((yyvsp[0].ast_node));
            (yyval.expression) = condNode ;
        }
#line 2533 "hulk/parser.cpp"
    break;

  case 95: /* expression: IF expression ast_construct ELSE EOL ast_construct  */
#line 479 "parser.y"
                                                         {
            ConditionalNode* condNode = new ConditionalNode();
            condNode->addBranch((yyvsp[-4].expression), (yyvsp[-3].ast_node));
            condNode->setElse((yyvsp[0].ast_node));
            (yyval.expression) = condNode ;
        }
#line 2544 "hulk/parser.cpp"
    break;

  case 96: /* expression: IF expression ast_construct EOL ELSE EOL ast_construct  */
#line 485 "parser.y"
                                                             {
            ConditionalNode* condNode = new ConditionalNode();
            condNode->addBranch((yyvsp[-5].expression), (yyvsp[-4].ast_node));
            condNode->setElse((yyvsp[0].ast_node));
            (yyval.expression) = condNode ;
        }
#line 2555 "hulk/parser.cpp"
    break;

  case 97: /* expression: IF expression ast_construct ELIF expression ast_construct ELSE ast_construct  */
#line 491 "parser.y"
                                                                                   {
            ConditionalNode* condNode = new ConditionalNode();
            condNode->addBranch((yyvsp[-6].expression), (yyvsp[-5].ast_node));
            condNode->addBranch((yyvsp[-3].expression), (yyvsp[-2].ast_node));
            condNode->setElse((yyvsp[0].ast_node));
            (yyval.expression) = condNode ;
        }
#line 2567 "hulk/parser.cpp"
    break;

  case 98: /* expression: IF expression ast_construct ELIF expression ast_construct EOL ELSE ast_construct  */
#line 498 "parser.y"
                                                                                       {
            ConditionalNode* condNode = new ConditionalNode();
            condNode->addBranch((yyvsp[-7].expression), (yyvsp[-6].ast_node));
            condNode->addBranch((yyvsp[-4].expression), (yyvsp[-3].ast_node));
            condNode->setElse((yyvsp[0].ast_node));
            (yyval.expression) = condNode ;
        }
#line 2579 "hulk/parser.cpp"
    break;

  case 99: /* expression: IF expression ast_construct ELIF expression ast_construct ELSE EOL ast_construct  */
#line 505 "parser.y"
                                                                                       {
            ConditionalNode* condNode = new ConditionalNode();
            condNode->addBranch((yyvsp[-7].expression), (yyvsp[-6].ast_node));
            condNode->addBranch((yyvsp[-4].expression), (yyvsp[-3].ast_node));
            condNode->setElse((yyvsp[0].ast_node));
            (yyval.expression) = condNode ;
        }
#line 2591 "hulk/parser.cpp"
    break;

  case 100: /* expression: IF expression ast_construct EOL ELIF expression ast_construct ELSE ast_construct  */
#line 512 "parser.y"
                                                                                       {
            ConditionalNode* condNode = new ConditionalNode();
            condNode->addBranch((yyvsp[-7].expression), (yyvsp[-6].ast_node));
            condNode->addBranch((yyvsp[-3].expression), (yyvsp[-2].ast_node));
            condNode->setElse((yyvsp[0].ast_node));
            (yyval.expression) = condNode ;
        }
#line 2603 "hulk/parser.cpp"
    break;

  case 101: /* expression: IF expression ast_construct EOL ELIF expression ast_construct EOL ELSE ast_construct  */
#line 519 "parser.y"
                                                                                           {
            ConditionalNode* condNode = new ConditionalNode();
            condNode->addBranch((yyvsp[-8].expression), (yyvsp[-7].ast_node));
            condNode->addBranch((yyvsp[-4].expression), (yyvsp[-3].ast_node));
            condNode->setElse((yyvsp[0].ast_node));
            (yyval.expression) = condNode ;
        }
#line 2615 "hulk/parser.cpp"
    break;

  case 102: /* expression: IF expression ast_construct EOL ELIF expression ast_construct ELSE EOL ast_construct  */
#line 526 "parser.y"
                                                                                           {
            ConditionalNode* condNode = new ConditionalNode();
            condNode->addBranch((yyvsp[-8].expression), (yyvsp[-7].ast_node));
            condNode->addBranch((yyvsp[-4].expression), (yyvsp[-3].ast_node));
            condNode->setElse((yyvsp[0].ast_node));
            (yyval.expression) = condNode ;
        }
#line 2627 "hulk/parser.cpp"
    break;

  case 103: /* expression: IF expression ast_construct EOL ELIF expression ast_construct EOL ELSE EOL ast_construct  */
#line 533 "parser.y"
                                                                                               {
            ConditionalNode* condNode = new ConditionalNode();
            condNode->addBranch((yyvsp[-9].expression), (yyvsp[-8].ast_node));
            condNode->addBranch((yyvsp[-5].expression), (yyvsp[-4].ast_node));
            condNode->setElse((yyvsp[0].ast_node));
            (yyval.expression) = condNode ;
        }
#line 2639 "hulk/parser.cpp"
    break;

  case 104: /* expression: LET declaration_list IN expression  */
#line 541 "parser.y"
                                         { (yyval.expression) = new LetExprNode(*(yyvsp[-2].decllist), (yyvsp[0].expression)); delete (yyvsp[-2].decllist); }
#line 2645 "hulk/parser.cpp"
    break;

  case 105: /* expression: LET declaration_list EOL IN expression  */
#line 542 "parser.y"
                                             { (yyval.expression) = new LetExprNode(*(yyvsp[-3].decllist), (yyvsp[0].expression)); delete (yyvsp[-3].decllist); }
#line 2651 "hulk/parser.cpp"
    break;

  case 106: /* expression: LET declaration_list IN EOL expression  */
#line 543 "parser.y"
                                             { (yyval.expression) = new LetExprNode(*(yyvsp[-3].decllist), (yyvsp[0].expression)); delete (yyvsp[-3].decllist); }
#line 2657 "hulk/parser.cpp"
    break;

  case 107: /* expression: LET declaration_list EOL IN EOL expression  */
#line 544 "parser.y"
                                                 { (yyval.expression) = new LetExprNode(*(yyvsp[-4].decllist), (yyvsp[0].expression)); delete (yyvsp[-4].decllist); }
#line 2663 "hulk/parser.cpp"
    break;

  case 108: /* expression: WHILE LPAREN expression RPAREN ast_construct  */
#line 545 "parser.y"
                                                   { (yyval.expression) = new WhileNode((yyvsp[-2].expression), (yyvsp[0].ast_node)); }
#line 2669 "hulk/parser.cpp"
    break;

  case 109: /* expression: WHILE expression ast_construct  */
#line 546 "parser.y"
                                     { (yyval.expression) = new WhileNode((yyvsp[-1].expression), (yyvsp[0].ast_node)); }
#line 2675 "hulk/parser.cpp"
    break;

  case 110: /* expression: FOR LPAREN IDENTIFIER IN expression RPAREN ast_construct  */
#line 547 "parser.y"
                                                               {

        if (auto* funcCall = dynamic_cast<FuncCallNode*>((yyvsp[-2].expression))) {
            if (funcCall->identifier == "range" && funcCall->args.size() == 2) {

                (yyval.expression) = new ForRangeNode((yyvsp[-4].string), funcCall->args[0], funcCall->args[1], (yyvsp[0].ast_node));

                funcCall->args.clear();
                delete funcCall;
                free((yyvsp[-4].string));
            } else {


                std::string iterableVar = std::string("_iterable_") + (yyvsp[-4].string);

                ExpressionNode* nextCall = new FuncCallNode("next", {new VariableNode(iterableVar.c_str())});
                ExpressionNode* currentCall = new FuncCallNode("current", {new VariableNode(iterableVar.c_str())});

                std::vector<VarDeclPair> innerDecls;
                VarDeclPair innerPair;
                innerPair.id = (yyvsp[-4].string);
                innerPair.expr = currentCall;
                innerPair.type = Type::getUnknownType();
                innerDecls.push_back(innerPair);

                ASTNode* letBody = nullptr;
                if (dynamic_cast<ExpressionNode*>((yyvsp[0].ast_node))) {
                    letBody = new LetExprNode(innerDecls, dynamic_cast<ExpressionNode*>((yyvsp[0].ast_node)));
                } else {
                    letBody = new LetVarNode((yyvsp[-4].string), currentCall, (yyvsp[0].ast_node), Type::getUnknownType());
                }

                WhileNode* whileLoop = new WhileNode(nextCall, letBody);

                std::vector<VarDeclPair> outerDecls;
                VarDeclPair outerPair;
                outerPair.id = iterableVar;
                outerPair.expr = (yyvsp[-2].expression);
                outerPair.type = Type::getUnknownType();
                outerDecls.push_back(outerPair);

                (yyval.expression) = new LetExprNode(outerDecls, whileLoop);
                free((yyvsp[-4].string));
            }
        } else {

            std::string iterableVar = std::string("_iterable_") + (yyvsp[-4].string);

            ExpressionNode* nextCall = new FuncCallNode("next", {new VariableNode(iterableVar.c_str())});
            ExpressionNode* currentCall = new FuncCallNode("current", {new VariableNode(iterableVar.c_str())});

            std::vector<VarDeclPair> innerDecls;
            VarDeclPair innerPair;
            innerPair.id = (yyvsp[-4].string);
            innerPair.expr = currentCall;
            innerPair.type = Type::getUnknownType();
            innerDecls.push_back(innerPair);

            ASTNode* letBody = nullptr;
            if (dynamic_cast<ExpressionNode*>((yyvsp[0].ast_node))) {
                letBody = new LetExprNode(innerDecls, dynamic_cast<ExpressionNode*>((yyvsp[0].ast_node)));
            } else {
                letBody = new LetVarNode((yyvsp[-4].string), currentCall, (yyvsp[0].ast_node), Type::getUnknownType());
            }

            WhileNode* whileLoop = new WhileNode(nextCall, letBody);

            std::vector<VarDeclPair> outerDecls;
            VarDeclPair outerPair;
            outerPair.id = iterableVar;
            outerPair.expr = (yyvsp[-2].expression);
            outerPair.type = Type::getUnknownType();
            outerDecls.push_back(outerPair);

            (yyval.expression) = new LetExprNode(outerDecls, whileLoop);
            free((yyvsp[-4].string));
        }
    }
#line 2758 "hulk/parser.cpp"
    break;

  case 111: /* expression: LBRACE expression_block RBRACE  */
#line 625 "parser.y"
                                     {
        (yyval.expression) = new BlockExprNode(*(yyvsp[-1].explist));
        delete (yyvsp[-1].explist);
    }
#line 2767 "hulk/parser.cpp"
    break;

  case 112: /* postfix_expression: primary_expression  */
#line 632 "parser.y"
                       { (yyval.expression) = (yyvsp[0].expression); }
#line 2773 "hulk/parser.cpp"
    break;

  case 113: /* postfix_expression: postfix_expression DOT IDENTIFIER  */
#line 633 "parser.y"
                                        {

        if (auto varNode = dynamic_cast<VariableNode*>((yyvsp[-2].expression))) {
            if (varNode->identifier == "self") {

                (yyval.expression) = new SelfMemberAccessNode((yyvsp[0].string));
                delete varNode;
            } else {
                (yyval.expression) = new MemberAccessNode((yyvsp[-2].expression), (yyvsp[0].string));
            }
        } else {
            (yyval.expression) = new MemberAccessNode((yyvsp[-2].expression), (yyvsp[0].string));
        }
        free((yyvsp[0].string));
    }
#line 2793 "hulk/parser.cpp"
    break;

  case 114: /* postfix_expression: postfix_expression DOT IDENTIFIER LPAREN expression_list RPAREN  */
#line 648 "parser.y"
                                                                      {
        (yyval.expression) = new MethodCallNode((yyvsp[-5].expression), (yyvsp[-3].string), *(yyvsp[-1].explist));
        delete (yyvsp[-1].explist);
        free((yyvsp[-3].string));
    }
#line 2803 "hulk/parser.cpp"
    break;

  case 115: /* postfix_expression: postfix_expression DOT IDENTIFIER LPAREN RPAREN  */
#line 653 "parser.y"
                                                      {
        (yyval.expression) = new MethodCallNode((yyvsp[-4].expression), (yyvsp[-2].string), {});
        free((yyvsp[-2].string));
    }
#line 2812 "hulk/parser.cpp"
    break;

  case 116: /* postfix_expression: IDENTIFIER LPAREN expression_list RPAREN  */
#line 657 "parser.y"
                                               { (yyval.expression) = new FuncCallNode((yyvsp[-3].string), *(yyvsp[-1].explist)); delete (yyvsp[-1].explist); }
#line 2818 "hulk/parser.cpp"
    break;

  case 117: /* postfix_expression: IDENTIFIER LPAREN RPAREN  */
#line 658 "parser.y"
                               { (yyval.expression) = new FuncCallNode((yyvsp[-2].string), {}); }
#line 2824 "hulk/parser.cpp"
    break;

  case 118: /* primary_expression: NUMBER  */
#line 662 "parser.y"
           { 

        std::string token_str(yytext);
        if (token_str.find('.') != std::string::npos) {
            (yyval.expression) = new NumberNode(yylval.decimal);
        } else {
            (yyval.expression) = new NumberNode((yyvsp[0].number));
        }
    }
#line 2838 "hulk/parser.cpp"
    break;

  case 119: /* primary_expression: STRING  */
#line 671 "parser.y"
             { (yyval.expression) = new StringLiteralNode((yyvsp[0].string)); }
#line 2844 "hulk/parser.cpp"
    break;

  case 120: /* primary_expression: TRUE  */
#line 672 "parser.y"
           { (yyval.expression) = new BooleanNode(true); }
#line 2850 "hulk/parser.cpp"
    break;

  case 121: /* primary_expression: FALSE  */
#line 673 "parser.y"
            { (yyval.expression) = new BooleanNode(false); }
#line 2856 "hulk/parser.cpp"
    break;

  case 122: /* primary_expression: IDENTIFIER  */
#line 674 "parser.y"
                 { (yyval.expression) = new VariableNode((yyvsp[0].string)); }
#line 2862 "hulk/parser.cpp"
    break;

  case 123: /* primary_expression: LPAREN expression RPAREN  */
#line 675 "parser.y"
                               { (yyval.expression) = (yyvsp[-1].expression); }
#line 2868 "hulk/parser.cpp"
    break;

  case 124: /* primary_expression: LPAREN statement RPAREN  */
#line 676 "parser.y"
                              { 
        if (auto* exprStmt = dynamic_cast<ExpressionStatementNode*>((yyvsp[-1].statement))) {
            (yyval.expression) = exprStmt->expression;
        } else if (auto* printStmt = dynamic_cast<PrintStatementNode*>((yyvsp[-1].statement))) {

            (yyval.expression) = new PrintExpressionNode(printStmt->expression);
            printStmt->expression = nullptr;
            delete printStmt;
        } else {
            (yyval.expression) = new NumberNode(0);
        }
    }
#line 2885 "hulk/parser.cpp"
    break;

  case 125: /* primary_expression: NEW IDENTIFIER LPAREN expression_list RPAREN  */
#line 688 "parser.y"
                                                   {
        (yyval.expression) = new TypeInstantiationNode((yyvsp[-3].string), *(yyvsp[-1].explist));
        delete (yyvsp[-1].explist);
        free((yyvsp[-3].string));
    }
#line 2895 "hulk/parser.cpp"
    break;

  case 126: /* primary_expression: NEW IDENTIFIER LPAREN RPAREN  */
#line 693 "parser.y"
                                   {
        (yyval.expression) = new TypeInstantiationNode((yyvsp[-2].string), {});
        free((yyvsp[-2].string));
    }
#line 2904 "hulk/parser.cpp"
    break;

  case 127: /* primary_expression: BASE LPAREN expression_list RPAREN  */
#line 697 "parser.y"
                                         {
        (yyval.expression) = new BaseCallNode(*(yyvsp[-1].explist));
        delete (yyvsp[-1].explist);
    }
#line 2913 "hulk/parser.cpp"
    break;

  case 128: /* primary_expression: BASE LPAREN RPAREN  */
#line 701 "parser.y"
                         {
        (yyval.expression) = new BaseCallNode({});
    }
#line 2921 "hulk/parser.cpp"
    break;

  case 129: /* expression_list: expression  */
#line 707 "parser.y"
               { (yyval.explist) = new std::vector<ExpressionNode*>; (yyval.explist)->push_back((yyvsp[0].expression)); }
#line 2927 "hulk/parser.cpp"
    break;

  case 130: /* expression_list: expression_list COMMA EOL expression  */
#line 708 "parser.y"
                                           { (yyvsp[-3].explist)->push_back((yyvsp[0].expression)); (yyval.explist) = (yyvsp[-3].explist); }
#line 2933 "hulk/parser.cpp"
    break;

  case 131: /* expression_list: expression_list COMMA expression  */
#line 709 "parser.y"
                                       { (yyvsp[-2].explist)->push_back((yyvsp[0].expression)); (yyval.explist) = (yyvsp[-2].explist); }
#line 2939 "hulk/parser.cpp"
    break;

  case 132: /* expression_list: expression_list EOL expression  */
#line 710 "parser.y"
                                     { (yyvsp[-2].explist)->push_back((yyvsp[0].expression)); (yyval.explist) = (yyvsp[-2].explist); }
#line 2945 "hulk/parser.cpp"
    break;

  case 133: /* identifier_list: IDENTIFIER  */
#line 714 "parser.y"
               { (yyval.idlist) = new std::vector<std::string>; (yyval.idlist)->push_back((yyvsp[0].string)); }
#line 2951 "hulk/parser.cpp"
    break;

  case 134: /* identifier_list: identifier_list COMMA EOL IDENTIFIER  */
#line 715 "parser.y"
                                           { (yyvsp[-3].idlist)->push_back((yyvsp[0].string)); (yyval.idlist) = (yyvsp[-3].idlist); }
#line 2957 "hulk/parser.cpp"
    break;

  case 135: /* identifier_list: identifier_list COMMA IDENTIFIER  */
#line 716 "parser.y"
                                       { (yyvsp[-2].idlist)->push_back((yyvsp[0].string)); (yyval.idlist) = (yyvsp[-2].idlist); }
#line 2963 "hulk/parser.cpp"
    break;

  case 136: /* identifier_list: identifier_list EOL IDENTIFIER  */
#line 717 "parser.y"
                                     { (yyvsp[-2].idlist)->push_back((yyvsp[0].string)); (yyval.idlist) = (yyvsp[-2].idlist); }
#line 2969 "hulk/parser.cpp"
    break;

  case 137: /* identifier_list: %empty  */
#line 718 "parser.y"
       { (yyval.idlist) = new std::vector<std::string>; }
#line 2975 "hulk/parser.cpp"
    break;

  case 138: /* parameter_list: parameter  */
#line 722 "parser.y"
              { (yyval.decllist) = (yyvsp[0].decllist); }
#line 2981 "hulk/parser.cpp"
    break;

  case 139: /* parameter_list: parameter_list COMMA EOL parameter  */
#line 723 "parser.y"
                                         {
        (yyval.decllist) = (yyvsp[-3].decllist);
        for (const auto& pair : *(yyvsp[0].decllist)) {
            (yyval.decllist)->push_back(pair);
        }
        delete (yyvsp[0].decllist);
    }
#line 2993 "hulk/parser.cpp"
    break;

  case 140: /* parameter_list: parameter_list COMMA parameter  */
#line 730 "parser.y"
                                     {
        (yyval.decllist) = (yyvsp[-2].decllist);
        for (const auto& pair : *(yyvsp[0].decllist)) {
            (yyval.decllist)->push_back(pair);
        }
        delete (yyvsp[0].decllist);
    }
#line 3005 "hulk/parser.cpp"
    break;

  case 141: /* parameter_list: parameter_list EOL parameter  */
#line 737 "parser.y"
                                   {
        (yyval.decllist) = (yyvsp[-2].decllist);
        for (const auto& pair : *(yyvsp[0].decllist)) {
            (yyval.decllist)->push_back(pair);
        }
        delete (yyvsp[0].decllist);
    }
#line 3017 "hulk/parser.cpp"
    break;

  case 142: /* parameter_list: %empty  */
#line 744 "parser.y"
       { (yyval.decllist) = new std::vector<VarDeclPair>; }
#line 3023 "hulk/parser.cpp"
    break;

  case 143: /* parameter: IDENTIFIER  */
#line 748 "parser.y"
               {
        (yyval.decllist) = new std::vector<VarDeclPair>;
        VarDeclPair pair;
        pair.id = (yyvsp[0].string);
        pair.expr = nullptr;
        pair.type = nullptr;
        (yyval.decllist)->push_back(pair);
        free((yyvsp[0].string));
    }
#line 3037 "hulk/parser.cpp"
    break;

  case 144: /* parameter: IDENTIFIER COLON type  */
#line 757 "parser.y"
                            {
        (yyval.decllist) = new std::vector<VarDeclPair>;
        VarDeclPair pair;
        pair.id = (yyvsp[-2].string);
        pair.expr = nullptr;
        pair.type = (yyvsp[0].type);
        (yyval.decllist)->push_back(pair);
        free((yyvsp[-2].string));
    }
#line 3051 "hulk/parser.cpp"
    break;

  case 145: /* ast_construct: expression  */
#line 769 "parser.y"
               { (yyval.ast_node) = (yyvsp[0].expression); }
#line 3057 "hulk/parser.cpp"
    break;

  case 146: /* ast_construct: statement  */
#line 770 "parser.y"
                 { (yyval.ast_node) = (yyvsp[0].statement); }
#line 3063 "hulk/parser.cpp"
    break;

  case 147: /* expression_block: expression  */
#line 774 "parser.y"
               {
        (yyval.explist) = new std::vector<ExpressionNode*>;
        (yyval.explist)->push_back((yyvsp[0].expression));
    }
#line 3072 "hulk/parser.cpp"
    break;

  case 148: /* expression_block: expression SEMICOLON  */
#line 778 "parser.y"
                           {
        (yyval.explist) = new std::vector<ExpressionNode*>;
        (yyval.explist)->push_back((yyvsp[-1].expression));
    }
#line 3081 "hulk/parser.cpp"
    break;

  case 149: /* expression_block: expression_block expression  */
#line 782 "parser.y"
                                  {
        (yyvsp[-1].explist)->push_back((yyvsp[0].expression));
        (yyval.explist) = (yyvsp[-1].explist);
    }
#line 3090 "hulk/parser.cpp"
    break;

  case 150: /* expression_block: expression_block expression SEMICOLON  */
#line 786 "parser.y"
                                            {
        (yyvsp[-2].explist)->push_back((yyvsp[-1].expression));
        (yyval.explist) = (yyvsp[-2].explist);
    }
#line 3099 "hulk/parser.cpp"
    break;

  case 151: /* expression_block: expression_block EOL  */
#line 790 "parser.y"
                           {
        (yyval.explist) = (yyvsp[-1].explist);
    }
#line 3107 "hulk/parser.cpp"
    break;

  case 152: /* expression_block: EOL  */
#line 793 "parser.y"
          {
        (yyval.explist) = new std::vector<ExpressionNode*>;
    }
#line 3115 "hulk/parser.cpp"
    break;

  case 153: /* expression_block: %empty  */
#line 796 "parser.y"
      {
        (yyval.explist) = new std::vector<ExpressionNode*>;
    }
#line 3123 "hulk/parser.cpp"
    break;

  case 154: /* type_member_list: %empty  */
#line 803 "parser.y"
    { (yyval.typemembers) = new std::vector<ASTNode*>; }
#line 3129 "hulk/parser.cpp"
    break;

  case 155: /* type_member_list: type_member_list type_member  */
#line 804 "parser.y"
                                   {
        (yyvsp[-1].typemembers)->push_back((yyvsp[0].ast_node));
        (yyval.typemembers) = (yyvsp[-1].typemembers);
    }
#line 3138 "hulk/parser.cpp"
    break;

  case 156: /* type_member: IDENTIFIER EQUALS expression SEMICOLON  */
#line 811 "parser.y"
                                           {
        (yyval.ast_node) = new Attribute((yyvsp[-3].string), nullptr, (yyvsp[-1].expression));
        free((yyvsp[-3].string));
    }
#line 3147 "hulk/parser.cpp"
    break;

  case 157: /* type_member: IDENTIFIER COLON type EQUALS expression SEMICOLON  */
#line 815 "parser.y"
                                                        {
        (yyval.ast_node) = new Attribute((yyvsp[-5].string), (yyvsp[-3].type), (yyvsp[-1].expression));
        free((yyvsp[-5].string));
    }
#line 3156 "hulk/parser.cpp"
    break;

  case 158: /* type_member: IDENTIFIER LPAREN parameter_list RPAREN ARROW expression SEMICOLON  */
#line 819 "parser.y"
                                                                         {
        std::vector<Parameter> params;
        for (const auto& pair : *(yyvsp[-4].decllist)) {
            params.emplace_back(pair.id, pair.type);
        }
        (yyval.ast_node) = new DefFuncNode((yyvsp[-6].string), params, (yyvsp[-1].expression), nullptr);
        delete (yyvsp[-4].decllist);
        free((yyvsp[-6].string));
    }
#line 3170 "hulk/parser.cpp"
    break;

  case 159: /* type_member: IDENTIFIER LPAREN RPAREN ARROW expression SEMICOLON  */
#line 828 "parser.y"
                                                          {
        std::vector<Parameter> params;
        (yyval.ast_node) = new DefFuncNode((yyvsp[-5].string), params, (yyvsp[-1].expression), nullptr);
        free((yyvsp[-5].string));
    }
#line 3180 "hulk/parser.cpp"
    break;

  case 160: /* type_member: IDENTIFIER LPAREN parameter_list RPAREN COLON type ARROW expression SEMICOLON  */
#line 833 "parser.y"
                                                                                    {
        std::vector<Parameter> params;
        for (const auto& pair : *(yyvsp[-6].decllist)) {
            params.emplace_back(pair.id, pair.type);
        }
        (yyval.ast_node) = new DefFuncNode((yyvsp[-8].string), params, (yyvsp[-1].expression), (yyvsp[-3].type));
        delete (yyvsp[-6].decllist);
        free((yyvsp[-8].string));
    }
#line 3194 "hulk/parser.cpp"
    break;

  case 161: /* type_member: IDENTIFIER LPAREN RPAREN COLON type ARROW expression SEMICOLON  */
#line 842 "parser.y"
                                                                     {
        std::vector<Parameter> params;
        (yyval.ast_node) = new DefFuncNode((yyvsp[-7].string), params, (yyvsp[-1].expression), (yyvsp[-3].type));
        free((yyvsp[-7].string));
    }
#line 3204 "hulk/parser.cpp"
    break;


#line 3208 "hulk/parser.cpp"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 849 "parser.y"


void yyerror(const char* s) {
    std::cerr << "Error: " << s << " en línea " << line_count << ", cerca de '" << yytext << "'" << std::endl;
}
