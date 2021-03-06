/* 
 * Copyright (C) 2006 Laird Breyer
 *  
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA.
 * 
 * Author:   Laird Breyer <laird@lbreyer.com>
 */

#ifndef PARSER_H
#define PARSER_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <expat.h>

typedef struct {
  enum XML_Status rstatus;
  enum XML_Parsing pstatus;
  int lineno;
  int colno;
  int length;
  long byteno;
} status_t;

typedef int result_t;
#define PARSER_OK       0x00
#define PARSER_STOP     0x01
#define PARSER_DEFAULT  0x02
#define PARSER_ABORT    0x04

typedef result_t (xml_start_tag_fun)(void *user, const char_t *name, const char_t **att);
typedef result_t (xml_attribute_fun)(void *user, const char_t *name, const char_t *value);
typedef result_t (xml_end_tag_fun)(void *user, const char_t *name);
typedef result_t (xml_chardata_fun)(void *user, const char_t *buf, size_t buflen);
typedef result_t (xml_pidata_fun)(void *user, const char_t *target, const char_t *data);
typedef result_t (xml_start_cdata_fun)(void *user);
typedef result_t (xml_end_cdata_fun)(void *user);
typedef result_t (xml_start_doctypedecl_fun)(void *user, const char_t *name, const char_t *sysid, const char_t *pubid, bool_t intsubset);
typedef result_t (xml_end_doctypedecl_fun)(void *user);
typedef result_t (xml_entitydecl_fun)(void *user, const char_t *name, bool_t isparam, const char_t *value, int len, const char_t *base, const char_t *sysid, const char_t *pubid, const char_t *notation);
typedef result_t (xml_comment_fun)(void *user, const char_t *data);
typedef result_t (xml_default_fun)(void *user, const char_t *data, size_t buflen);

/* note: the attribute callback function is not called by the parser_t,
 * it is intended for more high level parsers like stdparser_t
 *
 * note: the chardata() callback converts entities (&lt; -> <) which can
 * be a problem if the buffer is to be output as an XML fragment. There
 * is no way to prevent this expansion in libexpat, but the dfault()
 * contains the unexpanded data. Unfortunately, dfault can be called even
 * for non-chardata buffers, which makes this essentially useless. The
 * simplest method IMHO is to watch for this and recode the entities when needed.
 */
typedef struct {
  xml_start_tag_fun *start_tag; /* after start tag */
  xml_attribute_fun *attribute; /* after start tag, once for each name/value */
  xml_end_tag_fun *end_tag; /* after end tag */
  xml_chardata_fun *chardata; /* after any char data fragment */
  xml_pidata_fun *pidata; /* after processing instruction */
  xml_start_cdata_fun *start_cdata; /* after start of CDATA */
  xml_end_cdata_fun *end_cdata; /* after end of CDATA */
  xml_comment_fun *comment; /* after comment */
  xml_start_doctypedecl_fun *start_doctypedecl; /* after DOCTYPE before [] */
  xml_end_doctypedecl_fun *end_doctypedecl; /* end of DOCTYPE after [] */
  xml_entitydecl_fun *entitydecl; /* after ENTITY */
  xml_default_fun *dfault; /* any fragment not covered by above */
} callback_t;

typedef struct {
  XML_Parser p;
  status_t cur;
  callback_t callbacks;
  void *user;
} parser_t;

bool_t create_parser(parser_t *parser, void *ud);
bool_t free_parser(parser_t *parser);
bool_t reset_parser(parser_t *parser);
bool_t reset_handlers_parser(parser_t *parser);

byte_t *getbuf_parser(parser_t *parser, size_t n);
void freebuf_parser(parser_t *parser, byte_t *b);

bool_t stop_parser(parser_t *parser, bool_t abort);
bool_t restart_parser(parser_t *parser);
bool_t ok_parser(parser_t *parser);
bool_t suspended_parser(parser_t *parser);
bool_t aborted_parser(parser_t *parser);

bool_t setup_parser(parser_t *parser, callback_t *callbacks);
bool_t audit_parser(parser_t *parser);
bool_t do_parser(parser_t *parser, size_t nbytes);
bool_t do_parser2(parser_t *parser, const byte_t *buf, size_t nbytes);

const char_t *error_message_parser(parser_t *parser);

#endif
