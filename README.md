# IU Compiler
自制编程语言IU的编译器. 语言设计上借鉴了C语言和Standford的教学语言cool. 这个项目只是我在学习编译原理时搞的玩具，所以出于方便实施的角度
砍掉了一些features. 详细内容以后再补上

#IU Syntax

    program       ::= [class]
    class         ::= class TYPE_ID [inherits TYPE_ID] {[feature]}
    feature       ::= [field]
                   |  [method]
    field         ::= TYPE_ID OBJECT_ID = [expression]
    method        ::= [TYPE_ID] | [void] OBJECT_ID([parameter]){[statement]}
    statement     ::= if_stmt
                  ::= while_stmt
                  ::= expression
    if_stmt       ::= if(expression) {[statement]}
    while_stmt    ::= while(expression) {[statement]}
    relation_exp  ::= arith_exp +|-|>|<|>=|<=|==|= arith_exp
    arith_exp     ::= expression *|/|% expression
    expression    ::= OBJECT_ID
                   |  [OBJECT_ID.]OBJECT_ID([expression [,expression]])
                   |  new TYPE_ID
                   |  INT_CONST
                   |  STRING_CONST
                   |  CHAR_CONST
                   |  true
                   |  false
                   |  break
                   |  continue
