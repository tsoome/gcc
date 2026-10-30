/* Solaris support needed only by C/C++ frontends.
   Copyright (C) 2004-2024 Free Software Foundation, Inc.
   Contributed by CodeSourcery, LLC.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "c-family/c-common.h"
#include "stringpool.h"
#include "attribs.h"

#include "c-family/c-format.h"
#include "intl.h"

#include "c-family/c-pragma.h"

#define NO_FMT NULL, FMT_LEN_none, STD_C89

static const format_length_info printf_length_specs[] =
{
  { "h", FMT_LEN_h, STD_C89, "hh", FMT_LEN_hh, STD_C99, 0 },
  { "l", FMT_LEN_l, STD_C89, "ll", FMT_LEN_ll, STD_C9L, 0 },
  { "L", FMT_LEN_L, STD_C89, NO_FMT, 0 },
  { "z", FMT_LEN_z, STD_C99, NO_FMT, 0 },
  { "t", FMT_LEN_t, STD_C99, NO_FMT, 0 },
  { "j", FMT_LEN_j, STD_C99, NO_FMT, 0 },
  { "w", FMT_LEN_w, STD_C89, NO_FMT, 0 },
  { NO_FMT, NO_FMT, 0 }
};

static const format_length_info cmn_err_length_specs[] =
{
  { "h", FMT_LEN_h, STD_C89, "hh", FMT_LEN_hh, STD_C99, 0 },
  { "l", FMT_LEN_l, STD_C89, "ll", FMT_LEN_ll, STD_C89, 0 },
  { "j", FMT_LEN_j, STD_C99, NO_FMT, 0 },
  { "z", FMT_LEN_z, STD_C99, NO_FMT, 0 },
  { NO_FMT, NO_FMT, 0 }
};

static const format_length_info mdb_printf_length_specs[] =
{
  { "h", FMT_LEN_h, STD_C89, NO_FMT, 0 },
  { "l", FMT_LEN_l, STD_C89, "ll", FMT_LEN_ll, STD_C9L, 0 },
  { "j", FMT_LEN_j, STD_C99, NO_FMT, 0 },
  { "z", FMT_LEN_z, STD_C99, NO_FMT, 0 },
  { NO_FMT, NO_FMT, 0 }
};

static const format_flag_spec printf_flag_specs[] =
{
  { ' ',  0, 0, 0, N_("' ' flag"),        N_("the ' ' printf flag"),              STD_C89 },
  { '+',  0, 0, 0, N_("'+' flag"),        N_("the '+' printf flag"),              STD_C89 },
  { '#',  0, 0, 0, N_("'#' flag"),        N_("the '#' printf flag"),              STD_C89 },
  { '0',  0, 0, 0, N_("'0' flag"),        N_("the '0' printf flag"),              STD_C89 },
  { '-',  0, 0, 0, N_("'-' flag"),        N_("the '-' printf flag"),              STD_C89 },
  { '\'', 0, 0, 0, N_("''' flag"),        N_("the ''' printf flag"),              STD_EXT },
  { 'w',  0, 0, 0, N_("field width"),     N_("field width in printf format"),     STD_C89 },
  { '?',  0, 0, 0, N_("field width"),     N_("field width in printf format"),     STD_EXT },
  { 'p',  0, 0, 0, N_("precision"),       N_("precision in printf format"),       STD_C89 },
  { 'L',  0, 0, 0, N_("length modifier"), N_("length modifier in printf format"), STD_C89 },
  { 't',  0, 0, 0, N_("ptrdiff_t type"),  N_("ptrdiff_t type in printf format"),  STD_C89 },
  { 0, 0, 0, 0, NULL, NULL, STD_C89 }
};

static const format_flag_spec cmn_err_flag_specs[] =
{
  { '0',  0, 0, 0, N_("'0' flag"),        N_("the '0' flag"),                     STD_C89 },
  { '-',  0, 0, 0, N_("'-' flag"),        N_("the '-' flag"),                     STD_C89 },
  { 'w',  0, 0, 0, N_("field width"),     N_("field width in printf format"),     STD_C89 },
  { 'p',  0, 0, 0, N_("precision"),       N_("precision in printf format"),       STD_C89 },
  { 'L',  0, 0, 0, N_("length modifier"), N_("length modifier in printf format"), STD_C89 },
  { 0, 0, 0, 0, NULL, NULL, STD_C89 }
};

static const format_flag_spec mdb_printf_flag_specs[] =
{
  { '+',  0, 0, 0, N_("'+' flag"),        N_("the '+' flag"),                     STD_C89 },
  { '#',  0, 0, 0, N_("'#' flag"),        N_("the '#' flag"),                     STD_C89 },
  { '0',  0, 0, 0, N_("'0' flag"),        N_("the '0' flag"),                     STD_C89 },
  { '-',  0, 0, 0, N_("'-' flag"),        N_("the '-' flag"),                     STD_C89 },
  { '?',  0, 0, 0, N_("'?' field width"), N_("pointer-sized field width"),        STD_C89 },
  { 'w',  0, 0, 0, N_("field width"),     N_("field width in printf format"),     STD_C89 },
  { 'L',  0, 0, 0, N_("length modifier"), N_("length modifier in printf format"), STD_C89 },
  { 0, 0, 0, 0, NULL, NULL, STD_C89 }
};

static const format_flag_pair printf_flag_pairs[] =
{
  { ' ', '+', 1, 0   },
  { '0', '-', 1, 0   },
  { '0', 'p', 1, 'i' },
  { 0, 0, 0, 0 }
};

static const format_flag_pair cmn_err_flag_pairs[] =
{
  { '0', '-', 1, 0 },
  { 0, 0, 0, 0 }
};

/* '?' sets the field width to that of a hexadecimal pointer value; an
   explicit field width (digits or '*') overrides it.  */
static const format_flag_pair mdb_printf_flag_pairs[] =
{
  { '0', '-', 1, 0 },
  { '?', 'w', 1, 0 },
  { 0, 0, 0, 0 }
};

static const format_char_info printf_char_table[] =
{
  /* C89 conversion specifiers.  */
  /*                     none     hh       h        l        ll       L        z        t        j        H        D        DD        w8       w16      w32      w64      wf8      wf16      wf32      wf64     w */
  { "di",  0, STD_C89, { T89_I,   T99_SC,  T89_S,   T89_L,   T9L_LL,  BADLEN,  T99_SST, T99_PD,  T99_IM,  BADLEN,  BADLEN,  BADLEN,   T23_I8,  T23_I16, T23_I32, T23_I64, T23_IF8, T23_IF16, T23_IF32, T23_IF64 }, "-wp0 +'",  "i",  NULL },
  { "oxX", 0, STD_C89, { T89_UI,  T99_UC,  T89_US,  T89_UL,  T9L_ULL, BADLEN,  T99_ST,  T99_UPD, T99_UIM, BADLEN,  BADLEN,  BADLEN,   T23_U8,  T23_U16, T23_U32, T23_U64, T23_UF8, T23_UF16, T23_UF32, T23_UF64 }, "-wp0#",     "i",  NULL },
  { "u",   0, STD_C89, { T89_UI,  T99_UC,  T89_US,  T89_UL,  T9L_ULL, BADLEN,  T99_ST,  T99_UPD, T99_UIM, BADLEN,  BADLEN,  BADLEN,   T23_U8,  T23_U16, T23_U32, T23_U64, T23_UF8, T23_UF16, T23_UF32, T23_UF64 }, "-wp0'",    "i",  NULL },
  { "fgG", 0, STD_C89, { T89_D,   BADLEN,  BADLEN,  T99_D,   BADLEN,  T89_LD,  BADLEN,  BADLEN,  BADLEN,  T23_D32, T23_D64, T23_D128, BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,   BADLEN,   BADLEN }, "-wp0 +#'", "",   NULL },
  { "eE",  0, STD_C89, { T89_D,   BADLEN,  BADLEN,  T99_D,   BADLEN,  T89_LD,  BADLEN,  BADLEN,  BADLEN,  T23_D32, T23_D64, T23_D128, BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,   BADLEN,   BADLEN }, "-wp0 +#",  "",   NULL },
  { "c",   0, STD_C89, { T89_I,   BADLEN,  BADLEN,  T94_WI,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,   BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,   BADLEN,   BADLEN,  T94_WI }, "-w", "", NULL },
  { "s",   1, STD_C89, { T89_C,   BADLEN,  BADLEN,  T94_W,   BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,   BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,   BADLEN,   BADLEN,  T94_W }, "-wp", "cR", NULL },
  { "p",   1, STD_C89, { T89_V,   BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,   BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,   BADLEN,   BADLEN }, "-wp0#",     "ci", NULL },
  { "n",   1, STD_C89, { T89_I,   T99_SC,  T89_S,   T89_L,   T9L_LL,  BADLEN,  T99_SST, T99_PD,  T99_IM,  BADLEN,  BADLEN,  BADLEN,   T23_I8,  T23_I16, T23_I32, T23_I64, T23_IF8, T23_IF16, T23_IF32, T23_IF64 }, "",          "W",  NULL },
  /* C99 conversion specifiers.  */
  { "F",   0, STD_C99, { T99_D,   BADLEN,  BADLEN,  T99_D,   BADLEN,  T99_LD,  BADLEN,  BADLEN,  BADLEN,  T23_D32, T23_D64, T23_D128, BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,   BADLEN,   BADLEN }, "-wp0 +#'", "",   NULL },
  { "aA",  0, STD_C99, { T99_D,   BADLEN,  BADLEN,  T99_D,   BADLEN,  T99_LD,  BADLEN,  BADLEN,  BADLEN,  T23_D32, T23_D64, T23_D128, BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,   BADLEN,   BADLEN }, "-wp0 +#",   "",   NULL },
  /* X/Open conversion specifiers.  */
  { "C",   0, STD_EXT, { TEX_WI,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,   BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,   BADLEN,   BADLEN }, "-w",        "",   NULL },
  { "S",   1, STD_EXT, { TEX_W,   BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,   BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,   BADLEN,   BADLEN }, "-wp",       "R",  NULL },
  { NULL,  0, STD_C89, NOLENGTHS, NULL, NULL, NULL }
};

static const format_char_info bitfield_string_type =
  { "b",   1, STD_C89, { T89_C,   BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN  }, "",   "cR", NULL };

/*
 * void_type_node causes gcc to think about our pointer as 'void *',
 * we would really need "const mdb_bitmask_t *", but I have no idea
 * currently how to achieve that.
 */
#define	MDB_BM	{ STD_C89, "const mdb_bitmask_t", &void_type_node }

/*
 * The bitmask argument is looked up with the length modifier of the
 * value argument, so it must be present in every column the "b" entry
 * of mdb_printf_char_table accepts.
 */
static const format_char_info mdb_bitfield_type =
  { "b",   1, STD_C89, { MDB_BM,   BADLEN,  MDB_BM,  MDB_BM,  MDB_BM,  BADLEN,  MDB_BM,  BADLEN,  MDB_BM  }, "",   "R", NULL };

static const format_char_info cmn_err_char_table[] =
{
  /*                     none     hh       h        l        ll       L        z        t        j        H       D       DD */
  { "dD",  0, STD_C89, { T89_I,   T99_SC,  T89_S,   T89_L,   T9L_LL,  BADLEN,  T99_SST, BADLEN,  T99_IM , BADLEN, BADLEN, BADLEN }, "-wp0", "",   NULL },
  { "oOxX",0, STD_C89, { T89_UI,  T99_UC,  T89_US,  T89_UL,  T9L_ULL, BADLEN,  T99_ST,  BADLEN,  T99_UIM, BADLEN, BADLEN, BADLEN }, "-wp0", "",   NULL },
  { "u",   0, STD_C89, { T89_UI,  T99_UC,  T89_US,  T89_UL,  T9L_ULL, BADLEN,  T99_ST,  BADLEN,  T99_UIM, BADLEN, BADLEN, BADLEN }, "-wp0", "",   NULL },
  { "c",   0, STD_C89, { T89_C,   BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN , BADLEN, BADLEN, BADLEN }, "-w",   "",   NULL },
  { "p",   1, STD_C89, { T89_V,   BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN , BADLEN, BADLEN, BADLEN }, "-w",   "c",  NULL },
  { "s",   1, STD_C89, { T89_C,   BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN , BADLEN, BADLEN, BADLEN }, "-wp",  "cR", NULL },
  { "b",   0, STD_C89, { T89_I,   BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN , BADLEN, BADLEN, BADLEN }, "-w0",  "",   &bitfield_string_type },
  { NULL,  0, STD_C89, NOLENGTHS, NULL, NULL, NULL }
};

#define	MDB_UP	{ STD_C89, "uintptr_t", &uintptr_type_node }
#define	MDB_TS	{ STD_C89, "time_t", T_L }

static const format_char_info mdb_printf_char_table[] =
{
  /*                     none     hh       h        l        ll       L        z        t        j        H       D       DD */
  { "aA",  0, STD_C89, { MDB_UP,  BADLEN,  BADLEN,  T89_UL,  T9L_ULL, BADLEN,  T99_ST,  BADLEN,  T99_UIM, BADLEN, BADLEN, BADLEN }, "-w0#?",  "i",   NULL },
  { "di",  0, STD_C89, { T89_I,   BADLEN,  T89_S,   T89_L,   T9L_LL,  BADLEN,  T99_SST, BADLEN,  T99_IM,  BADLEN, BADLEN, BADLEN }, "-w0+#?", "i",   NULL },
  { "eE",  0, STD_C89, { T89_D,   BADLEN,  BADLEN,  T99_D,   BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN, BADLEN, BADLEN }, "-w0+#?", "",    NULL },
  { "gG",  0, STD_C89, { T89_D,   BADLEN,  BADLEN,  T89_LD,  T89_LD,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN, BADLEN, BADLEN }, "-w0+#?", "",    NULL },
  { "oxX", 0, STD_C89, { T89_UI,  BADLEN,  T89_US,  T89_UL,  T9L_ULL, BADLEN,  T99_ST,  BADLEN,  T99_UIM, BADLEN, BADLEN, BADLEN }, "-w0#?",  "i",   NULL },
  { "q",   0, STD_C89, { T89_I,   BADLEN,  T89_S,   T89_L,   T9L_LL,  BADLEN,  T99_SST, BADLEN,  T99_IM,  BADLEN, BADLEN, BADLEN }, "-w0+#?", "i",   NULL },
  { "u",   0, STD_C89, { T89_UI,  BADLEN,  T89_US,  T89_UL,  T9L_ULL, BADLEN,  T99_ST,  BADLEN,  T99_UIM, BADLEN, BADLEN, BADLEN }, "-w0#?",  "i",   NULL },
  { "H",   0, STD_C89, { T89_UI,  BADLEN,  T89_US,  T89_UL,  T9L_ULL, BADLEN,  T99_ST,  BADLEN,  T99_UIM, BADLEN, BADLEN, BADLEN }, "-w0#?",  "i",   NULL },
  { "I",   0, STD_C89, { T89_UI,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN, BADLEN, BADLEN }, "-w?",    "",    NULL },
  { "c",   0, STD_C89, { T89_C,   BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN, BADLEN, BADLEN }, "-w?",    "",    NULL },
  { "m",   0, STD_C89, NOARGUMENTS, "w?",     "",    NULL },
  { "N",   1, STD_EXT, { T89_V,   BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN, BADLEN, BADLEN }, "-w?",    "c",   NULL },
  { "p",   1, STD_C89, { T89_V,   BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN, BADLEN, BADLEN }, "-w0#?",  "c",   NULL },
  { "r",   0, STD_C89, { T89_UI,  BADLEN,  T89_US,  T89_UL,  T9L_ULL, BADLEN,  T99_ST,  BADLEN,  T99_UIM, BADLEN, BADLEN, BADLEN }, "-w0#?",  "i",   NULL },
  { "R",   0, STD_C89, { T89_I,   BADLEN,  T89_S,   T89_L,   T9L_LL,  BADLEN,  T99_SST, BADLEN,  T99_IM,  BADLEN, BADLEN, BADLEN }, "-w0+#?", "i",   NULL },
  { "s",   1, STD_C89, { T89_C,   BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN, BADLEN, BADLEN }, "-w?",    "cR",  NULL },
  { "tT",  0, STD_C89, NOARGUMENTS, "w?",     "",    NULL },
  { "Y",   0, STD_C89, { MDB_TS,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN,  BADLEN, BADLEN, BADLEN }, "-w?",    "",    NULL },
  { "b",   0, STD_C89, { T89_UI,  BADLEN,  T89_US,  T89_UL,  T9L_ULL, BADLEN,  T99_ST,  BADLEN,  T99_UIM, BADLEN, BADLEN, BADLEN }, "-w0#?",  "c",   &mdb_bitfield_type },
  { "<",   0, STD_C89, NOARGUMENTS, "",       "",    NULL },
  { NULL,  0, STD_C89, NOLENGTHS, NULL, NULL, NULL }
};

EXPORTED_CONST format_kind_info solaris_format_types[] = {
  { "illumos_printf",  printf_length_specs,  printf_char_table, " +#0-'", NULL,
    printf_flag_specs, printf_flag_pairs,
    FMT_FLAG_ARG_CONVERT|FMT_FLAG_DOLLAR_MULTIPLE|FMT_FLAG_USE_DOLLAR|FMT_FLAG_EMPTY_PREC_OK|FMT_FLAG_ZERO_WIDTH_BAD,
    'w', 0, 'p', 0, 'L', 0,
    &integer_type_node, &integer_type_node
  },
  { "cmn_err",  cmn_err_length_specs,  cmn_err_char_table, "0-", NULL,
    cmn_err_flag_specs, cmn_err_flag_pairs,
    FMT_FLAG_ARG_CONVERT|FMT_FLAG_EMPTY_PREC_OK,
    'w', 0, 'p', 0, 'L', 0,
    &integer_type_node, &integer_type_node
  },
  { "mdb_printf",  mdb_printf_length_specs,  mdb_printf_char_table, "+#0-?", NULL,
    mdb_printf_flag_specs, mdb_printf_flag_pairs,
    FMT_FLAG_ARG_CONVERT,
    'w', 0, 0, 0, 'L', 0,
    &integer_type_node, NULL
  }
};

/* Handle #pragma align ALIGNMENT (VAR [, VAR]...)  */

static void
solaris_pragma_align (cpp_reader *pfile ATTRIBUTE_UNUSED)
{
  tree t, x;
  enum cpp_ttype ttype;
  unsigned HOST_WIDE_INT low;

  if (pragma_lex (&x) != CPP_NUMBER
      || pragma_lex (&t) != CPP_OPEN_PAREN)
    {
      warning (0, "malformed %<#pragma align%>, ignoring");
      return;
    }

  low = TREE_INT_CST_LOW (x);
  if (!tree_fits_uhwi_p (x)
      || (low != 1 && low != 2 && low != 4 && low != 8 && low != 16
	  && low != 32 && low != 64 && low != 128))
    {
      warning (0, "invalid alignment for %<#pragma align%>, ignoring");
      return;
    }

  ttype = pragma_lex (&t);
  if (ttype != CPP_NAME)
    {
      warning (0, "malformed %<#pragma align%>, ignoring");
      return;
    }

  while (1)
    {
      tree decl = identifier_global_value (t);
      if (decl && DECL_P (decl))
	warning (0, "%<#pragma align%> must appear before the declaration of "
		 "%qD, ignoring", decl);
      else
	solaris_pending_aligns = tree_cons (t, build_tree_list (NULL, x),
					    solaris_pending_aligns);

      ttype = pragma_lex (&t);
      if (ttype == CPP_COMMA)
	{
	  ttype = pragma_lex (&t);
	  if (ttype != CPP_NAME)
	    {
	      warning (0, "malformed %<#pragma align%>");
	      return;
	    }
	}
      else if (ttype == CPP_CLOSE_PAREN)
	{
	  if (pragma_lex (&t) != CPP_EOF)
	    warning (0, "junk at end of %<#pragma align%>");
	  return;
	}
      else
	{
	  warning (0, "malformed %<#pragma align%>");
	  return;
	}
    }
}

/* Handle #pragma init (function [, function]...)  */

static void
solaris_pragma_init (cpp_reader *pfile ATTRIBUTE_UNUSED)
{
  tree t;
  enum cpp_ttype ttype;

  if (pragma_lex (&t) != CPP_OPEN_PAREN)
    {
      warning (0, "malformed %<#pragma init%>, ignoring");
      return;
    }

  ttype = pragma_lex (&t);
  if (ttype != CPP_NAME)
    {
      warning (0, "malformed %<#pragma init%>, ignoring");
      return;
    }

  while (1)
    {
      tree decl = identifier_global_value (t);
      if (decl && DECL_P (decl))
	{
	  tree attrs = build_tree_list (get_identifier ("init"),
					NULL);
	  TREE_USED (decl) = 1;
	  DECL_PRESERVE_P (decl) = 1;
	  decl_attributes (&decl, attrs, 0);
	}
      else
	solaris_pending_inits = tree_cons (t, NULL, solaris_pending_inits);

      ttype = pragma_lex (&t);
      if (ttype == CPP_COMMA)
	{
	  ttype = pragma_lex (&t);
	  if (ttype != CPP_NAME)
	    {
	      warning (0, "malformed %<#pragma init%>");
	      return;
	    }
	}
      else if (ttype == CPP_CLOSE_PAREN)
	{
	  if (pragma_lex (&t) != CPP_EOF)
	    warning (0, "junk at end of %<#pragma init%>");
	  return;
	}
      else
	{
	  warning (0, "malformed %<#pragma init%>");
	  return;
	}
    }
}

/* Handle #pragma fini (function [, function]...)  */

static void
solaris_pragma_fini (cpp_reader *pfile ATTRIBUTE_UNUSED)
{
  tree t;
  enum cpp_ttype ttype;

  if (pragma_lex (&t) != CPP_OPEN_PAREN)
    {
      warning (0, "malformed %<#pragma fini%>, ignoring");
      return;
    }

  ttype = pragma_lex (&t);
  if (ttype != CPP_NAME)
    {
      warning (0, "malformed %<#pragma fini%>, ignoring");
      return;
    }

  while (1)
    {
      tree decl = identifier_global_value (t);
      if (decl && DECL_P (decl))
	{
	  tree attrs = build_tree_list (get_identifier ("fini"),
					NULL);
	  TREE_USED (decl) = 1;
	  DECL_PRESERVE_P (decl) = 1;
	  decl_attributes (&decl, attrs, 0);
	}
      else
	solaris_pending_finis = tree_cons (t, NULL, solaris_pending_finis);

      ttype = pragma_lex (&t);
      if (ttype == CPP_COMMA)
	{
	  ttype = pragma_lex (&t);
	  if (ttype != CPP_NAME)
	    {
	      warning (0, "malformed %<#pragma fini%>");
	      return;
	    }
	}
      else if (ttype == CPP_CLOSE_PAREN)
	{
	  if (pragma_lex (&t) != CPP_EOF)
	    warning (0, "junk at end of %<#pragma fini%>");
	  return;
	}
      else
	{
	  warning (0, "malformed %<#pragma fini%>");
	  return;
	}
    }
}

/* Register Solaris-specific #pragma directives.  */

void
solaris_register_pragmas (void)
{
  c_register_pragma_with_expansion (0, "align", solaris_pragma_align);
  c_register_pragma (0, "init", solaris_pragma_init);
  c_register_pragma (0, "fini", solaris_pragma_fini);
}
