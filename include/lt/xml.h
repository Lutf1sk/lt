#ifndef LT_XML_H
#define LT_XML_H 1

#include <lt/fwd.h>
#include <lt/err.h>

typedef
struct lt_xml_attrib {
	lstr_t key;
	lstr_t val;
} lt_xml_attrib_t;

#define LT_XML_CDATA	0
#define LT_XML_ELEMENT	1
#define LT_XML_PI		2
#define LT_XML_CREF		3

typedef
struct lt_xml_entity {
	u8 type;
	union {
		struct {
			lstr_t name;
			lt_darr(lt_xml_attrib_t) attribs;
			lt_darr(struct lt_xml_entity) children;
		} elem;

		struct {
			lstr_t target;
			lstr_t content;
		} pi;

		lstr_t cdata;
		char cref;
	};
} lt_xml_entity_t;

typedef
struct lt_xml_err_info {
	lt_err_t err;
	u32 err_line;
	lstr_t err_str;
} lt_xml_err_info_t;

lt_err_t lt_xml_add_attrib(lt_xml_entity_t* elem, lt_xml_attrib_t attrib, lt_alloc_t* alloc);
lt_err_t lt_xml_add_child(lt_xml_entity_t* elem, lt_xml_entity_t* child, lt_alloc_t* alloc);

lt_xml_attrib_t* lt_xml_find_attrib(lt_xml_entity_t* elem, lstr_t key);

lt_err_t lt_xml_generate_str(lt_xml_entity_t* elem, lstr_t* out, lt_alloc_t* alloc);

usz lt_xml_child_count(lt_xml_entity_t* elem);
usz lt_xml_attrib_count(lt_xml_entity_t* elem);

lt_err_t lt_xml_parse(lt_xml_entity_t* xml, void* data, usz size, lt_xml_err_info_t* out_err_info, lt_alloc_t* alloc);

void lt_xml_free(lt_xml_entity_t* xml, lt_alloc_t* alloc);

isz lt_xml_write(lt_xml_entity_t* xml, lt_io_callback_t callb, void* usr);
isz lt_xml_write_pretty(lt_xml_entity_t* xml, lt_io_callback_t callb, void* usr);

#endif