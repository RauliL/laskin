" Vim syntax file
" Language: Laskin
" Filenames: *.laskin

if version < 600
    syntax clear
elseif exists("b:current_syntax")
    finish
endif

" Keywords
syn keyword laskinConditional if else case
syn keyword laskinRepeat while for

" Constants
syn keyword laskinConstant true false e pi inf

" Function declarations
"syn match laskinFunctionDeclaration ":\s*[^\s({]\+"

" Number literals
syn match laskinInteger "\<0[xX]\x+\%(_\x\+\)*\>" display
syn match laskinInteger "\<0\%(0[dD]\)\=\%(0\|[1-9]\d*\%(_\d\+\)*\)\>" display
syn match laskinInteger "\<0[oO]\=\o\+\%(_\o\+\)*\>" display
syn match laskinInteger "\<0[bB][01]\+\%(_[01]\+\)*\>" display
syn match laskinReal "\<\%(0\|[1-9]\d*\%(_\d\+\)*\)\.\d\+\%(_\d\+\)*\>" display
syn match laskinReal "\<\%(0\|[1-9]\d*\%(_\d\+\)*\)\%(\.\d\+\%(_\d\+\)*\)\=\%([eE][-+]\=\d\+\%(_\d\+\)*\)\>" display

" String literals
syn region laskinString start="\"" end="\"" skip="\\\\\|\\\"" fold
syn region laskinString start="'" end="'" skip="\\\\\|\\\'" fold

" Comments
syn match laskinSharpBang "\%^#!.*" display
syn keyword laskinTodo TODO FIXME XXX contained
syn match laskinComment "#.*" contains=laskinSharpBang,laskinTodo

hi def link laskinConditional Conditional
hi def link laskinRepeat Repeat
"hi def link laskinFunctionDeclaration Define
hi def link laskinConstant Constant
hi def link laskinInteger Number
hi def link laskinReal Float
hi def link laskinComment Comment
hi def link laskinSharpBang PreProc
hi def link laskinString String
hi def link laskinTodo Todo
