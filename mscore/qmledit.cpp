//=============================================================================
//  MuseScore
//  Music Composition & Notation
//
//  Copyright (C) 2002-2014 Werner Schweer
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2
//  as published by the Free Software Foundation and appearing in
//  the file LICENCE.GPL
//
//  Syntax highlighter based on example code from Ariya Hidayat
//  (git://gitorious.org/ofi-labs/x2.git BSD licensed).
//=============================================================================

#include "qmledit.h"
#include "musescore.h"

namespace Ms {

QmlEdit::QmlEdit(QWidget *parent) {}
QmlEdit::~QmlEdit() {}

//---------------------------------------------------------
//   JSHighlighter
//---------------------------------------------------------

JSHighlighter::JSHighlighter(QTextDocument *parent) {
      // Reserved Words
      // https://developer.mozilla.org/en/JavaScript/Reference/Reserved_Words
      static const char* data1[] = { "break", "case", "catch", "continue",
         "default", "delete", "do", "else", "finally", "for", "function",
         "if", "in", "instanceof", "new", "return", "switch", "this", "throw",
         "try", "typeof", "var", "void", "while", "with", "true", "false",
         "null" };
      addKeywords(data1);

      // built-in and other popular objects + properties
      static const char* data2[] = { "Object", "prototype", "create",
         "defineProperty", "defineProperties", "getOwnPropertyDescriptor",
         "keys", "getOwnPropertyNames", "constructor", "__parent__", "__proto__",
         "__defineGetter__", "__defineSetter__", "eval", "hasOwnProperty",
         "isPrototypeOf", "__lookupGetter__", "__lookupSetter__", "__noSuchMethod__",
         "propertyIsEnumerable", "toSource", "toLocaleString", "toString",
         "unwatch", "valueOf", "watch", "Function", "arguments", "arity", "caller",
         "constructor", "length", "name", "apply", "bind", "call", "String",
         "fromCharCode", "length", "charAt", "charCodeAt", "concat", "indexOf",
         "lastIndexOf", "localCompare", "match", "quote", "replace", "search",
         "slice", "split", "substr", "substring", "toLocaleLowerCase",
         "toLocaleUpperCase", "toLowerCase", "toUpperCase", "trim", "trimLeft",
         "trimRight", "Array", "isArray", "index", "input", "pop", "push",
         "reverse", "shift", "sort", "splice", "unshift", "concat", "join",
         "filter", "forEach", "every", "map", "some", "reduce", "reduceRight",
         "RegExp", "global", "ignoreCase", "lastIndex", "multiline", "source",
         "exec", "test", "JSON", "parse", "stringify", "decodeURI",
         "decodeURIComponent", "encodeURI", "encodeURIComponent", "eval",
         "isFinite", "isNaN", "parseFloat", "parseInt", "Infinity", "NaN",
         "undefined", "Math", "E", "LN2", "LN10", "LOG2E", "LOG10E", "PI",
         "SQRT1_2", "SQRT2", "abs", "acos", "asin", "atan", "atan2", "ceil",
         "cos", "exp", "floor", "log", "max", "min", "pow", "random", "round",
         "sin", "sqrt", "tan", "document", "window", "navigator", "userAgent"
         };
      addKeywords(data2);
      }
}
