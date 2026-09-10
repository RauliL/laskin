" Vim syntax file
" Language: Laskin
" Maintainer: Rauli Laine
" Last Change: 2026 Sep 10

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

" Syntax: Constants
syn keyword laskinBoolean true false
syn match laskinConstant /-inf\>/
syn keyword laskinConstant e pi inf nan
syn keyword laskinConstant january february march april may june july august september october november december
syn keyword laskinConstant sunday monday tuesday wednesday thursday friday saturday

" Syntax: Control flow
" Note: \> after if would match before '-' because hyphen is not a word character.
syn match laskinConditional /\<if-else/
syn match laskinConditional /\<try-else/
syn match laskinConditional /\<if\%(-else\)\@!/
syn match laskinConditional /\<try\%(-else\)\@!/
syn keyword laskinRepeat while

" Syntax: Dictionary and program
syn keyword laskinStatement quit lookup define delete symbols include

" Syntax: Strings
syn match laskinStringEscape "\\["'\\/btnfr]" contained
syn match laskinStringEscape "\\u\x\{4}" contained
syn region laskinString start=/"/ end=/"/ contains=laskinStringEscape
syn region laskinString start=/'/ end=/'/ contains=laskinStringEscape

" Syntax: Numbers
let s:units  = 'mm\|cm\|km\|mg\|kg\|ms\|min\|m\|g\|s\|h\|d'
let s:n_body = '\d\+\%(_\d\+\)*\%(\.\d\+\%(_\d\+\)*\)\?\%(' . s:units . '\)\?\>'
let s:guard  = '\(\.\)\@<!'
exe 'syn match laskinNumber /' . s:guard . '-' . s:n_body . '/'
exe 'syn match laskinNumber /' . s:guard . '+' . s:n_body . '/'
exe 'syn match laskinNumber /' . s:guard . '\(\<[+-]\)\@<!\<' . s:n_body . '/'

" Syntax: Definitions
syn match laskinDefinition /->/

" Syntax: Delimiters
syn match laskinDelimiter /[()\[\]{}]/

" Syntax: Operators (from laskin/src/api/utils.cpp)
" Conversions (before bare > operator; avoid \<> which is an empty match)
syn match laskinConversion /\([>]\)string\>/
syn match laskinConversion /\([>]\)source\>/

" Type tests (? is not supported in syn keyword)
syn match laskinTypeTest /\<\%(boolean\|date\|month\|number\|vector\|record\|string\|time\|quote\|weekday\)[?]/

" Symbolic operators (longest match first)
syn match laskinOperator /<>/
syn match laskinOperator />=/
syn match laskinOperator /<=/
syn match laskinOperator /=\ze\($\|[^=]\)/
syn match laskinOperator />\ze\($\|[^=[:alpha:]]\)/
syn match laskinOperator /<\ze\($\|[^>=]\)/
syn match laskinOperator /[*\/%]/
syn match laskinOperator /\(\w\|\.\)\@<!-\%(\d\|inf\|>\)\@!/
syn match laskinOperator /\(\w\|\.\)\@<!+\%(\d\)\@!/

" Stack manipulation (laskin/src/api/utils.cpp)
syn keyword laskinStack clear dup drop nip over rot swap tuck depth

" I/O
syn match laskinIO /\.\./
syn match laskinIO /\.s\>/
syn match laskinIO /\(\d\)\@<!\./

hi def link laskinTodo               Todo
hi def link laskinSharpBang          PreProc
hi def link laskinComment            Comment
hi def link laskinStringEscape       Special
hi def link laskinString             String
hi def link laskinBoolean            Boolean
hi def link laskinConstant           Constant
hi def link laskinConditional        Conditional
hi def link laskinRepeat             Repeat
hi def link laskinStatement          Statement
hi def link laskinNumber             Number
hi def link laskinOperator           Operator
hi def link laskinStack              Operator
hi def link laskinTypeTest           Operator
hi def link laskinConversion         Operator
hi def link laskinIO                 Debug
hi def link laskinDelimiter          Delimiter
hi def link laskinDefinition         Define

let b:current_syntax = "laskin"
if main_syntax == "laskin"
  unlet main_syntax
endif
