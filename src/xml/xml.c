#include <lt/xml.h>
#include <lt/ctype.h>
#include <lt/utf8.h>
#include <lt/str.h>
#include <lt/mem.h>
#include <lt/darr.h>
#include <lt/io.h>
#include <lt/strstream.h>

#define ENC_UTF8 0
#define ENC_UTF16LE 1

typedef
struct parse_ctx {
	char* it, *end;
	u32 line;
	lt_xml_entity_t* elem;
	lt_xml_err_info_t* err_info;
	lt_alloc_t* alloc;
	u8 enc;
} parse_ctx_t;

static LT_INLINE
b8 is_whitespace(u32 c) {
	return c == 0x20 || c == 0x09 || c == 0x0D || c == 0x0A;
}

static LT_INLINE
b8 is_name_start(u32 c) {
	return c == ':' || c == '_' ||
			(c >= 'A' && c <= 'Z') ||
			(c >= 'a' && c <= 'z') ||
			(c >= 0xC0 && c <= 0xD6) ||
			(c >= 0xD8 && c <= 0x2FF) ||
			(c >= 0x370 && c <= 0x37D) ||
			(c >= 0x37F && c <= 0x1FFF) ||
			(c >= 0x200C && c <= 0x200D) ||
			(c >= 0x2070 && c <= 0x218F) ||
			(c >= 0x2C00 && c <= 0x2FEF) ||
			(c >= 0x3001 && c <= 0xD7FF) ||
			(c >= 0xF900 && c <= 0xFDCF) ||
			(c >= 0xFDF0 && c <= 0xFFFD) ||
			(c >= 0x10000 && c <= 0xEFFFF);
}

static
b8 is_name(u32 c) {
	return is_name_start(c) || c == '-' || c == '.' || (c >= '0' && c <= '9') || c == 0xB7 || (c >= 0x300 && c <= 0x36F) || (c >= 0x203F && c <= 0x2040);
}

static LT_INLINE
b8 is_hex_digit(u32 c) {
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

static LT_INLINE
b8 enc_str_eq(parse_ctx_t* cx, lstr_t enc_str, lstr_t const_str) {
	if (cx->enc == ENC_UTF8) {
		return lt_lseq(enc_str, const_str);
	}
	else if (cx->enc == ENC_UTF16LE) {
		u16 str16[64];
		for (usz i = 0; i < const_str.len; ++i) // this is only possible because the function is never called with non-ascii characters
			str16[i] = const_str.str[i];
		return enc_str.len == const_str.len * 2 && memcmp(enc_str.str, str16, enc_str.len) == 0;
	}
	else {
		LT_ASSERT_NOT_REACHED();
		return 0;
	}
}

static LT_INLINE
b8 is_eof(parse_ctx_t* cx) {
	return cx->it >= cx->end;
}

static
b8 str_pending(parse_ctx_t* cx, lstr_t str) {
	if (cx->enc == ENC_UTF8) {
		if (cx->it + str.len > cx->end)
			return 0;
		return memcmp(cx->it, str.str, str.len) == 0;
	}
	else if (cx->enc == ENC_UTF16LE) {
		if (cx->it + str.len * 2 > cx->end)
			return 0;
		u16 str16[64];
		for (usz i = 0; i < str.len; ++i) // this is only possible because function is never called with non-ascii characters
			str16[i] = str.str[i];
		return memcmp(cx->it, str16, str.len * 2) == 0;
	}
	else {
		LT_ASSERT_NOT_REACHED();
		return 0;
	}
}

static
lt_err_t consume_str(parse_ctx_t* cx, lstr_t str) {
	if (cx->enc == ENC_UTF8) {
		if (cx->it + str.len > cx->end)
			return LT_ERR_INVALID_SYNTAX;
		if (memcmp(cx->it, str.str, str.len) != 0)
			return LT_ERR_INVALID_SYNTAX;
		cx->it += str.len;
	}
	else if (cx->enc == ENC_UTF16LE) {
		if (cx->it + str.len * 2 > cx->end)
			return LT_ERR_INVALID_SYNTAX;
		u16 str16[64];
		for (usz i = 0; i < str.len; ++i) // this is only possible because function is never called with non-ascii characters
			str16[i] = str.str[i];
		if (memcmp(cx->it, str16, str.len * 2) != 0)
			return 0;
		cx->it += str.len * 2;
	}
	return LT_SUCCESS;
}

usz utf16_decode_len(u16 c) {
	return 1;
}

static
lt_err_t consume(parse_ctx_t* cx, u32* out) {
	if (is_eof(cx))
		return LT_ERR_INVALID_SYNTAX;

	u32 dec_char;

	if (cx->enc == ENC_UTF8) {
		usz utf8_len = lt_utf8_decode_len(*cx->it);
		if (cx->it + utf8_len > cx->end)
			return LT_ERR_INVALID_FORMAT;

		lt_utf8_decode(&dec_char, cx->it);
		cx->it += utf8_len;
	}
	else if (cx->enc == ENC_UTF16LE) {
		dec_char = *(u16*)cx->it;
		cx->it += 2;
	}
	else {
		LT_ASSERT_NOT_REACHED();
		return LT_ERR_UNKNOWN;
	}

	if (out)
		*out = dec_char;
	if (dec_char == '\n')
		cx->line++;
	return LT_SUCCESS;
}

static
lt_err_t consume_char(parse_ctx_t* cx, u32 ch) {
	lt_err_t err;

	u32 c;
	if ((err = consume(cx, &c)))
		return err;
	if (c != ch)
		return c;

	return LT_SUCCESS;
}

static
lt_err_t read_char(parse_ctx_t* cx, u32* out) {
	if (is_eof(cx))
		return LT_ERR_INVALID_SYNTAX;

	u32 dec_char;

	if (cx->enc == ENC_UTF8) {
		usz utf8_len = lt_utf8_decode_len(*cx->it);
		if (cx->it + utf8_len > cx->end)
			return LT_ERR_INVALID_FORMAT;

		lt_utf8_decode(&dec_char, cx->it);
		if (out)
			*out = dec_char;
	}
	else if (cx->enc == ENC_UTF16LE) {
		dec_char = *(u16*)cx->it;
	}
	else {
		LT_ASSERT_NOT_REACHED();
		return LT_ERR_UNKNOWN;
	}

	if (out)
		*out = dec_char;

	return LT_SUCCESS;
}

static
lt_err_t consume_whitespace(parse_ctx_t* cx) {
	lt_err_t err;

	while (!is_eof(cx)) {
		u32 c;
		if ((err = read_char(cx, &c)))
			return err;
		if (!is_whitespace(c))
			break;
		consume(cx, NULL);
	}

	return LT_SUCCESS;
}

static
lt_err_t consume_digits(parse_ctx_t* cx, lstr_t* out) {
	lt_err_t err;

	char* start = cx->it;

	u32 c;
	if ((err = consume(cx, &c)))
			return err;
	if (!lt_is_digit(c))
		return LT_ERR_INVALID_SYNTAX;

	while (!is_eof(cx)) {
		if ((err = read_char(cx, &c)))
			return err;
		if (!lt_is_digit(c))
			break;
		consume(cx, NULL);
	}

	if (out)
		*out = lt_lsfrom_range(start, cx->it);
	return LT_SUCCESS;
}

static
lt_err_t consume_hex_digits(parse_ctx_t* cx, lstr_t* out) {
	lt_err_t err;

	char* start = cx->it;

	u32 c;
	if ((err = consume(cx, &c)))
			return err;
	if (!is_hex_digit(c))
		return LT_ERR_INVALID_SYNTAX;

	while (!is_eof(cx)) {
		if ((err = read_char(cx, &c)))
			return err;
		if (!is_hex_digit(c))
			break;
		consume(cx, NULL);
	}

	if (out)
		*out = lt_lsfrom_range(start, cx->it);
	return LT_SUCCESS;
}

static
lt_err_t consume_name(parse_ctx_t* cx, lstr_t* out) {
	lt_err_t err;

	char* name_start = cx->it;

	u32 c;
	if ((err = consume(cx, &c)))
		return err;
	if (!is_name_start(c))
		return LT_ERR_INVALID_SYNTAX;

	while (!is_eof(cx)) {
		if ((err = read_char(cx, &c)))
			return err;
		if (!is_name(c))
			break;
		consume(cx, NULL);
	}

	if (out) {
		*out = lt_lsfrom_range(name_start, cx->it);
		if (cx->enc == ENC_UTF16LE) {
			out->len /= 2;
			u16* it16 = (u16*)name_start;
			u8* it8 = (u8*)name_start;
			for (usz i = 0; i < out->len; ++i)
				it8[i] = it16[i]; // !!!!!!!!!! horrible, terrible stuff. fix this.
		}
	}
	return LT_SUCCESS;
}

static
lt_err_t consume_ref(parse_ctx_t* cx, u32* out) {
	lt_err_t err;
	u32 c;
	if ((err = consume(cx, &c)))
		return err;
	if (c != '&')
		return LT_ERR_INVALID_SYNTAX;

	if ((err = read_char(cx, &c)))
		return err;
	if (c == '#') {
		consume(cx, NULL);

		lstr_t number;
		if ((err = read_char(cx, &c)))
			return err;
		if (c == 'x') {
			consume(cx, NULL);

			if ((err = consume_hex_digits(cx, &number)))
				return err;

			u64 n;
			if ((err = lt_lshextou(number, &n)))
				return err;
			if (n > LT_U32_MAX)
				return LT_ERR_OVERFLOW;
			*out = n;
		}
		else {
			if ((err = consume_digits(cx, &number)))
				return err;
			u64 n;
			if ((err = lt_lstou(number, &n)))
				return err;
			if (n > LT_U32_MAX)
				return LT_ERR_OVERFLOW;
			*out = n;
		}
	}
	else {
		lstr_t name;
		if ((err = consume_name(cx, &name)))
			return err;

		if (lt_lseq_nocase(name, CLSTR("lt")))
			*out = '<';
		else if (lt_lseq_nocase(name, CLSTR("gt")))
			*out = '>';
		else if (lt_lseq_nocase(name, CLSTR("amp")))
			*out = '&';
		else if (lt_lseq_nocase(name, CLSTR("apos")))
			*out = '\'';
		else if (lt_lseq_nocase(name, CLSTR("quot")))
			*out = '"';
		else
			return LT_ERR_INVALID_SYNTAX;
	}

	if ((err = consume(cx, &c)))
			return err;
	if (c != ';')
		return LT_ERR_INVALID_SYNTAX;

	return LT_SUCCESS;
}

static
lt_err_t consume_literal(parse_ctx_t* cx, lstr_t* out) {
	lt_err_t err;

	if (is_eof(cx))
		return LT_ERR_INVALID_SYNTAX;

	u32 quote;
	if ((err = consume(cx, &quote)))
		return err;
	if (quote != '"' && quote != '\'')
		return LT_ERR_INVALID_SYNTAX;

	char* literal_start = cx->it;
	for (;;) {
		u32 c;
		if ((err = consume(cx, &c)))
			return err;
		if (c == quote)
			break;
	}

	if (out)
		*out = lt_lsfrom_range(literal_start, cx->it - 1);
	return LT_SUCCESS;
}

static
lt_err_t consume_comment(parse_ctx_t* cx) {
	lt_err_t err;
	if ((err = consume_str(cx, CLSTR("<!--"))))
		return err;

	while (!str_pending(cx, CLSTR("-->"))) {
		if (str_pending(cx, CLSTR("--")))
			return LT_ERR_INVALID_SYNTAX;
		if ((err = consume(cx, NULL)))
			return err;
	}
	consume_str(cx, CLSTR("-->"));
	return LT_SUCCESS;
}

static
lt_err_t consume_procinstr(parse_ctx_t* cx) {
	lt_err_t err;
	if ((err = consume_str(cx, CLSTR("<?"))))
		return err;

	lstr_t target;
	if ((err = consume_name(cx, &target)))
		return err;
	if ((err = consume_whitespace(cx)))
		return err;

	char* data_start = cx->it;
	while (!str_pending(cx, CLSTR("?>"))) {
		if ((err = consume(cx, NULL)))
			return err;
	}
	lstr_t content = lt_lsfrom_range(data_start, cx->it);

	lt_xml_entity_t ent = {
			.type = LT_XML_PI,
			.pi.target = target,
			.pi.content = content };
	lt_xml_add_child(cx->elem, &ent, cx->alloc);

	consume_str(cx, CLSTR("?>"));
	return LT_SUCCESS;
}

static
lt_err_t consume_cdata(parse_ctx_t* cx, lstr_t* out) {
	lt_err_t err;

	if ((err = consume_str(cx, CLSTR("<![CDATA["))))
		return err;

	char* cdata_start = cx->it;
	while (!str_pending(cx, CLSTR("]]>"))) {
		if ((err = consume(cx, NULL)))
			return err;
	}

	if (out)
		*out = lt_lsfrom_range(cdata_start, cx->it);

	consume_str(cx, CLSTR("]]>"));
	return LT_SUCCESS;
}

static
lt_err_t consume_doctypedef(parse_ctx_t* cx) {
	lt_err_t err;

	if ((err = consume_str(cx, CLSTR("<!DOCTYPE"))))
		return err;
	if ((err = consume_whitespace(cx)))
		return err;

	lstr_t name;
	if ((err = consume_name(cx, &name)))
		return err;
	if ((err = consume_whitespace(cx)))
		return err;

	// !! incomplete

	if ((err = consume_whitespace(cx)))
		return err;
	if ((err = consume_str(cx, CLSTR(">"))))
		return err;
	return LT_SUCCESS;
}

static
lt_err_t consume_stag(parse_ctx_t* cx, b8* out_empty, lt_xml_entity_t* elem) {
	lt_err_t err;

	*elem = (lt_xml_entity_t) { .type = LT_XML_ELEMENT };

	u32 c;
	if ((err = consume(cx, &c)))
		return err;
	if (c != '<')
		return LT_ERR_INVALID_SYNTAX;
	if ((err = consume_name(cx, &elem->elem.name)))
		return err;

	for (;;) {
		if ((err = consume_whitespace(cx)))
			return err;

		if ((err = read_char(cx, &c)))
			return err;

		if (!is_name_start(c))
			break;

		lstr_t attr_name;
		if ((err = consume_name(cx, &attr_name)))
			return err;
		if ((err = consume(cx, &c)))
			return err;
		if (c != '=')
			return LT_ERR_INVALID_SYNTAX;

		lstr_t attr_val;
		if ((err = consume_literal(cx, &attr_val)))
			return err;

		lt_xml_attrib_t attrib = {
				.key = attr_name,
				.val = attr_val };
		lt_xml_add_attrib(elem, attrib, cx->alloc);
	}

	if ((err = consume(cx, &c)))
		return err;
	if (c == '/') {
		*out_empty = 1;
		if ((err = consume(cx, &c)))
			return err;
	}
	else
		*out_empty = 0;
	if (c != '>')
		return LT_ERR_INVALID_SYNTAX;

	return LT_SUCCESS;
}

static
lt_err_t consume_etag(parse_ctx_t* cx) {
	lt_err_t err;

	u32 c;
	if ((err = consume_str(cx, CLSTR("</"))))
		return err;

	lstr_t elem_name;
	if ((err = consume_name(cx, &elem_name)))
		return err;
	if ((err = consume_whitespace(cx)))
		return err;

	if ((err = consume(cx, &c)))
		return err;
	if (c != '>')
		return LT_ERR_INVALID_SYNTAX;

	return LT_SUCCESS;
}

static lt_err_t consume_elem_content(parse_ctx_t* cx);

static
lt_err_t consume_elem(parse_ctx_t* cx) {
	lt_err_t err;

	lt_xml_entity_t ent;
	b8 empty;
	if ((err = consume_stag(cx, &empty, &ent)))
		return err;
	lt_xml_add_child(cx->elem, &ent, cx->alloc);
	if (empty)
		return LT_SUCCESS;

	lt_xml_entity_t* parent_elem = cx->elem;
	cx->elem = &cx->elem->elem.children[lt_darr_count(cx->elem->elem.children) - 1];
	if ((err = consume_elem_content(cx)))
		return err;
	if ((err = consume_etag(cx)))
		return err;
	cx->elem = parent_elem;

	return LT_SUCCESS;
}

static
lt_err_t consume_elem_content(parse_ctx_t* cx) {
	lt_err_t err;

	while (!is_eof(cx)) {
		u32 c;
		if ((err = read_char(cx, &c)))
			return err;

		if (c == '&') {
			u32 cref;
			if ((err = consume_ref(cx, &cref)))
				return err;
			lt_xml_entity_t ent = {
					.type = LT_XML_CREF,
					.cref = cref };
			lt_xml_add_child(cx->elem, &ent, cx->alloc);
			continue;
		}
		if (c != '<') {
			char* start = cx->it;
			if ((err = consume(cx, NULL)))
				return err;
			while (!is_eof(cx)) {
				if ((err = read_char(cx, &c)))
					return err;
				if (c == '&' || c == '<')
					break;
				consume(cx, NULL);
			}
			lt_xml_entity_t ent = {
					.type = LT_XML_CDATA,
					.cdata = lt_lsfrom_range(start, cx->it) };
			lt_xml_add_child(cx->elem, &ent, cx->alloc);
			continue;
		}

		if (str_pending(cx, CLSTR("<![CDATA["))) {
			lstr_t cdata;
			if ((err = consume_cdata(cx, &cdata)))
				return err;
			lt_xml_entity_t ent = {
					.type = LT_XML_CDATA,
					.cdata = cdata };
			lt_xml_add_child(cx->elem, &ent, cx->alloc);
		}
		else if (str_pending(cx, CLSTR("<!--"))) {
			if ((err = consume_comment(cx)))
				return err;
		}
		else if (str_pending(cx, CLSTR("<?"))) {
			if ((err = consume_procinstr(cx)))
				return err;
		}
		else if (str_pending(cx, CLSTR("</"))) {
			break;
		}
		else {
			if ((err = consume_elem(cx)))
				return err;
		}
	}

	return LT_SUCCESS;
}

lt_err_t lt_xml_add_attrib(lt_xml_entity_t* elem, lt_xml_attrib_t attrib, lt_alloc_t* alloc) {
	LT_ASSERT(elem != NULL);
	LT_ASSERT(elem->type == LT_XML_ELEMENT);

	if (elem->elem.attribs == NULL) {
		elem->elem.attribs = lt_darr_create(lt_xml_attrib_t, 8, alloc);
		if (elem->elem.attribs == NULL)
			return LT_ERR_OUT_OF_MEMORY;
	}
	lt_darr_push(elem->elem.attribs, attrib);
	return LT_SUCCESS;
}

lt_err_t lt_xml_add_child(lt_xml_entity_t* elem, lt_xml_entity_t* child, lt_alloc_t* alloc) {
	LT_ASSERT(elem != NULL);
	LT_ASSERT(elem->type == LT_XML_ELEMENT);

	if (elem->elem.children == NULL) {
		elem->elem.children = lt_darr_create(lt_xml_entity_t, 8, alloc);
		if (elem->elem.children == NULL)
			return LT_ERR_OUT_OF_MEMORY;
	}
	lt_darr_push(elem->elem.children, *child);
	return LT_SUCCESS;
}

lt_xml_attrib_t* lt_xml_find_attrib(lt_xml_entity_t* elem, lstr_t key) {
	usz attrib_count = lt_xml_attrib_count(elem);
	for (usz i = 0; i < attrib_count; ++i) {
		lt_xml_attrib_t* attrib = &elem->elem.attribs[i];
		if (lt_lseq(attrib->key, key))
			return attrib;
	}
	return NULL;
}

lt_err_t lt_xml_generate_str(lt_xml_entity_t* elem, lstr_t* out, lt_alloc_t* alloc) {
	lt_err_t err;

	lt_strstream_t ss;
	if ((err = lt_strstream_create(&ss, alloc)))
		return err;

	usz child_count = lt_xml_child_count(elem);
	for (usz i = 0; i < child_count; ++i) {
		lt_xml_entity_t* child = &elem->elem.children[i];
		switch (child->type) {
		case LT_XML_CDATA:
			lt_strstream_writels(&ss, child->cdata);
			break;

		case LT_XML_CREF:
			lt_strstream_writec(&ss, child->cref);
			break;

		case LT_XML_ELEMENT:
		default:
			break;
		}
	}

	*out = ss.str;
	return LT_SUCCESS;

err0:	lt_strstream_destroy(&ss);
		return err;
}

usz lt_xml_child_count(lt_xml_entity_t* elem) {
	if (elem->type != LT_XML_ELEMENT)
		return 0;
	if (elem->elem.children == NULL)
		return 0;
	return lt_darr_count(elem->elem.children);
}

usz lt_xml_attrib_count(lt_xml_entity_t* elem) {
	if (elem->type != LT_XML_ELEMENT)
		return 0;
	if (elem->elem.attribs == NULL)
		return 0;
	return lt_darr_count(elem->elem.attribs);
}

lt_err_t lt_xml_parse(lt_xml_entity_t* xml, void* data, usz size, lt_xml_err_info_t* out_err_info, lt_alloc_t* alloc) {
	lt_err_t err;

	*xml = (lt_xml_entity_t) { .type = LT_XML_ELEMENT };

	parse_ctx_t ctx = {
			.it = data,
			.end = (char*)data + size,
			.line = 1,
			.elem = xml,
			.err_info = out_err_info,
			.alloc = alloc,
			.enc = ENC_UTF8 };
	parse_ctx_t* cx = &ctx;

	if (size >= 2 && *(u16*)data == 0xFEFF) {
		cx->enc = ENC_UTF16LE;
		cx->it += 2;
	}

	if (str_pending(cx, CLSTR("<!DOCTYPE"))) {
		if ((err = consume_doctypedef(cx)))
			return err;
	}

	while (!is_eof(cx)) {
		if ((err = consume_elem_content(cx))) {
			lt_printf("'%S'\n", LSTR(cx->it, 5));
			return err;
		}
	}

	return LT_SUCCESS;
}

void lt_xml_free(lt_xml_entity_t* xml, lt_alloc_t* alloc) {
	switch (xml->type) {
	case LT_XML_CDATA:
// 		lt_mfree(alloc, xml->cdata.str);
		break;

	case LT_XML_ELEMENT:
		if (xml->elem.attribs) {
			for (usz i = 0; i < lt_darr_count(xml->elem.attribs); ++i) {
// 				lt_mfree(alloc, xml->elem.attribs[i].key.str);
// 				lt_mfree(alloc, xml->elem.attribs[i].val.str);
			}
			lt_darr_destroy(xml->elem.attribs);
		}
		if (xml->elem.children) {
			for (usz i = 0; i < lt_darr_count(xml->elem.children); ++i)
				lt_xml_free(&xml->elem.children[i], alloc);
			lt_darr_destroy(xml->elem.children);
		}
		break;

	case LT_XML_PI:
		break;
	}
}

#define PRINTF(...) do { if ((res = lt_io_printf(callb, usr, __VA_ARGS__)) < 0) return res; bytes += res; } while (0)

static
isz lt_xml_write_indent(lt_xml_entity_t* xml, lt_io_callback_t callb, void* usr, usz indent) {
	isz res;
	usz bytes = 0;

	switch (xml->type) {
	case LT_XML_CDATA:
		PRINTF("<![CDATA[%S]]>", xml->cdata);
		break;

	case LT_XML_ELEMENT:
		if (xml->elem.name.len == 0) {
			if (xml->elem.children) {
				for (usz i = 0; i < lt_darr_count(xml->elem.children); ++i) {
					res = lt_xml_write_indent(&xml->elem.children[i], callb, usr, indent);
					if (res < 0)
						return res;
					bytes += res;
				}
			}
			break;
		}
		PRINTF("<%S", xml->elem.name);

		if (xml->elem.attribs) {
			for (usz i = 0; i < lt_darr_count(xml->elem.attribs); ++i)
				PRINTF(" %S=\"%S\"", xml->elem.attribs[i].key, xml->elem.attribs[i].val);
		}
		PRINTF(">");
		if (xml->elem.children) {
			for (usz i = 0; i < lt_darr_count(xml->elem.children); ++i) {
				res = lt_xml_write_indent(&xml->elem.children[i], callb, usr, indent + 1);
				if (res < 0)
					return res;
				bytes += res;
			}
		}
		PRINTF("</%S>", xml->elem.name);
		break;

	case LT_XML_CREF:
		PRINTF("&#x%hd;", xml->cref);
		break;
	}

	return bytes;
}

isz lt_xml_write(lt_xml_entity_t* xml, lt_io_callback_t callb, void* usr) {
	return lt_xml_write_indent(xml, callb, usr, 0);
}

static
isz lt_xml_write_pretty_indent(lt_xml_entity_t* xml, lt_io_callback_t callb, void* usr, usz indent) {
	isz res;
	usz bytes = 0;

	switch (xml->type) {
	case LT_XML_CDATA:
		if (lt_lstrim(xml->cdata).len)
			PRINTF("%r\t<![CDATA[%S]]>\n", indent, lt_lstrim(xml->cdata));
		break;

	case LT_XML_ELEMENT:
		if (xml->elem.name.len == 0) {
			if (xml->elem.children) {
				for (usz i = 0; i < lt_darr_count(xml->elem.children); ++i) {
					res = lt_xml_write_pretty_indent(&xml->elem.children[i], callb, usr, indent);
					if (res < 0)
						return res;
					bytes += res;
				}
			}
			break;
		}
		PRINTF("%r\t<%S", indent, xml->elem.name);

		if (xml->elem.attribs) {
			for (usz i = 0; i < lt_darr_count(xml->elem.attribs); ++i)
				PRINTF(" %S=\"%S\"", xml->elem.attribs[i].key, xml->elem.attribs[i].val);
		}
		PRINTF(">\n");
		if (xml->elem.children) {
			for (usz i = 0; i < lt_darr_count(xml->elem.children); ++i) {
				res = lt_xml_write_pretty_indent(&xml->elem.children[i], callb, usr, indent + 1);
				if (res < 0)
					return res;
				bytes += res;
			}
		}
		PRINTF("%r\t</%S>\n", indent, xml->elem.name);
		break;

	case LT_XML_CREF:
		PRINTF("%r\t&#x%hd;\n", indent, xml->cref);
		break;
	}

	return bytes;
}

isz lt_xml_write_pretty(lt_xml_entity_t* xml, lt_io_callback_t callb, void* usr) {
	return lt_xml_write_pretty_indent(xml, callb, usr, 0);
}
