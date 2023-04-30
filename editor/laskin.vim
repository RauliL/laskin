" Vim syntax file
" Language: Laskin
" Maintainer: Rauli Laine
" Last Change: 2023 Apr 30

if !exists("main_syntax")
  " Quit if a syntax file has already been loaded.
  if exists("b:current_syntax")
    finish
  endif
  let main_syntax = "laskin"
elseif exists("b:current_syntax") && b:current_syntax == "laskin"
  finish
endif

" Syntax: Comments
syn keyword laskinTodo contained TODO FIXME XXX NOTE
syn match laskinSharpBang "\%^#!.*" display
syn match laskinComment "#.*$" contains=laskinTodo

" Syntax: Keywords
syn keyword laskinBoolean true false
syn keyword laskinConstant null e pi
syn keyword laskinConstant january february march april may june july august september october november december
syn keyword laskinConstant sunday monday tuesday wednesday thursday friday saturday
syn keyword laskinConditional if
syn match laskinConditional /if-else/
syn keyword laskinRepeat while

" Syntax: Strings
syn match laskinStringEscape "\\["'\\/btnfr]" contained
syn match laskinStringEscape "\\u\x\{4}" contained
syn region laskinString start=/"/ end=/"/ contains=laskinStringEscape
syn region laskinString start=/'/ end=/'/ contains=laskinStringEscape

" Syntax: Numbers
syn match laskinNumber /\<[+-]\?\d\+\(.\d\+\)\?\([eE]\d\+\([+-]\d\+\)\?\)\?\>/

" Syntax: Definitions
syn match laskinDefinition /->/

" Syntax: Delimiters
syn match laskinDelimiter /[()\[\]{}]/

" Syntax: Operators
syn keyword laskinOperator clear dup drop nip over rot swap tuck depth

hi def link laskinTodo               Todo
hi def link laskinSharpBang          PreProc
hi def link laskinComment            Comment
hi def link laskinStringEscape       Special
hi def link laskinString             String
hi def link laskinStringDouble       Double
hi def link laskinNumber             Number
hi def link laskinBoolean            Boolean
hi def link laskinConstant           Constant
hi def link laskinConditional        Conditional
hi def link laskinRepeat             Repeat
hi def link laskinOperator           Operator
hi def link laskinDelimiter          Delimiter
hi def link laskinDefinition         Define

let b:current_syntax = "laskin"
if main_syntax == "laskin"
  unlet main_syntax
endif
