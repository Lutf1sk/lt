#pragma once

#include <lt2/common.h>

typedef struct html_template {
	u8* it;
	u8* end;
} html_template;

usz echo(html_template* t, ls str);
usz echo_escaped(html_template* t, ls str);
usz echo_attributes(html_template* t, char* fmt, ...);

#define __with(begin, end) for (b8 __once = ((begin), 1); __once; (__once = 0), (end))

#define $(str)     echo_escaped(__template, lls(str, strlen(str)))
#define $ls(str)   echo_escaped(__template, str)
#define $raw(str)  echo(__template, lls(str, strlen(str)))
#define $echo(str) echo(__template, str)

#define $begin_elem(name, ...) $raw("<"), $raw(name) __VA_OPT__(, echo_attributes(__template, " " __VA_ARGS__)), $raw(">")
#define $end_elem(name) $raw("</"), $raw(name), $raw(">")
#define $elem(name, ...) __with($begin_elem(name __VA_OPT__(, __VA_ARGS__)), $end_elem(name))

#define $html(a...) $elem("html", a)
#define $head(a...) $elem("head", a)
#define $body(a...) $elem("body", a)

#define $title(a...)  $elem("title",  a)
#define $script(a...) $elem("script", a)
#define $style(a...)  $elem("script", a)
#define $link(a...)   $begin_elem("link", a)
#define $meta(a...)   $begin_elem("meta", a)

#define $area(a...)     $begin_elem("area",     a)
#define $base(a...)     $begin_elem("base",     a)
#define $col(a...)      $begin_elem("col",      a)
#define $embed(a...)    $begin_elem("embed",    a)
#define $img(a...)      $begin_elem("img",      a)
#define $input(a...)    $begin_elem("input",    a)
#define $keygen(a...)   $begin_elem("keygen",   a)
#define $menuitem(a...) $begin_elem("menuitem", a)
#define $param(a...)    $begin_elem("param",    a)
#define $source(a...)   $begin_elem("source",   a)
#define $track(a...)    $begin_elem("track",    a)

#define $br  $raw("<br>")
#define $wbr $raw("<wbr>")
#define $hr  $raw("<hr>")

#define $h1(a)...     $elem("h1", a)
#define $h2(a...)     $elem("h2", a)
#define $h3(a...)     $elem("h3", a)
#define $h4(a...)     $elem("h4", a)
#define $h5(a...)     $elem("h5", a)
#define $h6(a...)     $elem("h6", a)
#define $p(a...)      $elem("p",  a)
#define $a(a...)      $elem("a",  a)
#define $i(a...)      $elem("i",  a)
#define $b(a...)      $elem("b",  a)
#define $strong(a...) $elem("strong", a)
#define $code(a...)   $elem("code",  a)
#define $span(a...)   $elem("span",  a)

#define $nav(a...) $elem("nav",  a)
#define $div(a...) $elem("div",  a)

#define $table(a...) $elem("table", a)
#define $thead(a...) $elem("thead", a)
#define $tbody(a...) $elem("tbody", a)
#define $tfoot(a...) $elem("tfoot", a)
#define $tr(a...)    $elem("tr",    a)
#define $th(a...)    $elem("th",    a)
#define $td(a...)    $elem("td",    a)

#define $ul(a...)    $elem("ul",    a)
#define $ol(a...)    $elem("ol",    a)
#define $dl(a...)    $elem("dl",    a)
#define $li(a...)    $elem("li",    a)
#define $dt(a...)    $elem("dt",    a)
#define $dd(a...)    $elem("dd",    a)

#define $header(a...) $elem("header", a)
#define $main(a...)   $elem("main",   a)
#define $footer(a...) $elem("footer", a)

#define $template html_template* __template, void* __userdata
#define $forward __template, __userdata
#define $userdata __userdata

#define $forward_page(name) \
	void __lt2_begin_page_##name($template); \
	void __lt2_end_page_##name($template);

#define $page_prefix(name) void __lt2_begin_page_##name($template)
#define $page_suffix(name) void __lt2_end_page_##name($template)
#define $page(name) __with(__lt2_begin_page_##name(__template, __userdata), __lt2_end_page_##name(__template, __userdata))

