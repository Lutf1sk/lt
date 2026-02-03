#pragma once

#include <lt2/common.h>

typedef struct html_template {
	u8* it;
	u8* end;
} html_template;

usz echo(html_template* t, ls str);
usz echo_escaped(html_template* t, ls str);
usz echo_attributes(html_template* t, char* fmt, ...);

#define $(str)     echo_escaped(__template, lls(str, strlen(str)))
#define $ls(str)   echo_escaped(__template, str)
#define $raw(str)  echo(__template, lls(str, strlen(str)))
#define $echo(str) echo(__template, str)

#define $begin_elem(name, ...) $raw("<"), $raw(name) __VA_OPT__(, echo_attributes(__template, " " __VA_ARGS__)), $raw(">")
#define $end_elem(name) $raw("</"), $raw(name), $raw(">")
#define $elem(name, ...) __with($begin_elem(name __VA_OPT__(, __VA_ARGS__)), $end_elem(name))

#define $html(...) $elem("html", __VA_ARGS__)
#define $head(...) $elem("head", __VA_ARGS__)
#define $body(...) $elem("body", __VA_ARGS__)

#define $title(...)  $elem("title",  __VA_ARGS__)
#define $script(...) $elem("script", __VA_ARGS__)
#define $style(...)  $elem("script", __VA_ARGS__)
#define $link(...)   $begin_elem("link", __VA_ARGS__)
#define $meta(...)   $begin_elem("meta", __VA_ARGS__)

#define $area(...)     $begin_elem("area",     __VA_ARGS__)
#define $base(...)     $begin_elem("base",     __VA_ARGS__)
#define $col(...)      $begin_elem("col",      __VA_ARGS__)
#define $embed(...)    $begin_elem("embed",    __VA_ARGS__)
#define $img(...)      $begin_elem("img",      __VA_ARGS__)
#define $input(...)    $begin_elem("input",	   __VA_ARGS__)
#define $keygen(...)   $begin_elem("keygen",   __VA_ARGS__)
#define $menuitem(...) $begin_elem("menuitem", __VA_ARGS__)
#define $param(...)    $begin_elem("param",    __VA_ARGS__)
#define $source(...)   $begin_elem("source",   __VA_ARGS__)
#define $track(...)    $begin_elem("track",    __VA_ARGS__)

#define $br  $raw("<br>")
#define $wbr $raw("<wbr>")
#define $hr  $raw("<hr>")

#define $h1(...)     $elem("h1",     __VA_ARGS__)
#define $h2(...)     $elem("h2",     __VA_ARGS__)
#define $h3(...)     $elem("h3",     __VA_ARGS__)
#define $h4(...)     $elem("h4",     __VA_ARGS__)
#define $h5(...)     $elem("h5",     __VA_ARGS__)
#define $h6(...)     $elem("h6",     __VA_ARGS__)
#define $p(...)      $elem("p",      __VA_ARGS__)
#define $a(...)      $elem("a",      __VA_ARGS__)
#define $i(...)      $elem("i",      __VA_ARGS__)
#define $b(...)      $elem("b",      __VA_ARGS__)
#define $strong(...) $elem("strong", __VA_ARGS__)
#define $code(...)   $elem("code",   __VA_ARGS__)
#define $span(...)   $elem("span",   __VA_ARGS__)

#define $header(...)  $elem("header",  __VA_ARGS__)
#define $main(...)    $elem("main",    __VA_ARGS__)
#define $footer(...)  $elem("footer",  __VA_ARGS__)
#define $nav(...)     $elem("nav",     __VA_ARGS__)
#define $div(...)     $elem("div",     __VA_ARGS__)
#define $article(...) $elem("article", __VA_ARGS__)

#define $form(...)     $elem("form",     __VA_ARGS__)
#define $textarea(...) $elem("textarea", __VA_ARGS__)
#define $select(...)   $elem("select",   __VA_ARGS__)
#define $option(...)   $elem("option",   __VA_ARGS__)

#define $table(...) $elem("table", __VA_ARGS__)
#define $thead(...) $elem("thead", __VA_ARGS__)
#define $tbody(...) $elem("tbody", __VA_ARGS__)
#define $tfoot(...) $elem("tfoot", __VA_ARGS__)
#define $tr(...)    $elem("tr",    __VA_ARGS__)
#define $th(...)    $elem("th",    __VA_ARGS__)
#define $td(...)    $elem("td",    __VA_ARGS__)

#define $ul(...)    $elem("ul",    __VA_ARGS__)
#define $ol(...)    $elem("ol",    __VA_ARGS__)
#define $dl(...)    $elem("dl",    __VA_ARGS__)
#define $li(...)    $elem("li",    __VA_ARGS__)
#define $dt(...)    $elem("dt",    __VA_ARGS__)
#define $dd(...)    $elem("dd",    __VA_ARGS__)

#define $svg(...)      $elem("svg",      __VA_ARGS__)
#define $rect(...)     $elem("rect",     __VA_ARGS__)
#define $circle(...)   $elem("circle",   __VA_ARGS__)
#define $ellipse(...)  $elem("elllipse", __VA_ARGS__)
#define $stop(...)     $elem("stop",     __VA_ARGS__)
#define $line(...)     $elem("line",     __VA_ARGS__)
#define $polygon(...)  $elem("polygon",  __VA_ARGS__)
#define $text(...)     $elem("text",     __VA_ARGS__)
#define $textPath(...) $elem("textPath", __VA_ARGS__)
#define $tspan(...)    $elem("tspan",    __VA_ARGS__)
#define $view(...)     $elem("view",     __VA_ARGS__)
#define $symbol(...)   $elem("symbol",   __VA_ARGS__)
#define $switch(...)   $elem("switch",   __VA_ARGS__)

#define $template html_template* __template, void* __userdata
#define $forward __template, __userdata
#define $userdata __userdata

#define $forward_page(name) \
	void __lt2_begin_page_##name($template); \
	void __lt2_end_page_##name($template);

#define $page_prefix(name) void __lt2_begin_page_##name($template)
#define $page_suffix(name) void __lt2_end_page_##name($template)
#define $page(name) __with(__lt2_begin_page_##name(__template, __userdata), __lt2_end_page_##name(__template, __userdata))

