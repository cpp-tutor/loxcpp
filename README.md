## Scratch branch for work on Abaci scripting/programming language

Development code using existing interpreter engine. Language specification subject to change and is not fully implemented.

### Assignments

Constant assignment

```
let a = 3.3
```

Variable assignment

```
let b <- 1
```

Variable reassginment

```
b <- 2
```

Tuple assignment

```
let c <- { x:1, y:2.2, z:"hi" }
```

Array assignment

```
let d = [ 2, 3, 5, 7, 11 ]
```

Math operators

`+`, `-`, `*`, `/` (creates floating point when `% `non-zero), `%` (integer only), `//` (floor division), `%%` (create rational)

### Comparison operators

`=` (same as assign constant, but in different context), `/=`, `<`, `<=`, `>=`, `>`, begin `<`/`<=` v `<`/`<=` end

Decisions

```
if cond
  stmts...
endif
```

```
case expression of
  when match1 stmts...
  when match2... stmts...
  else  # this is optional
  stmts...
endcase
```

### Loops

```
while cond
  stmts...
endwhile
```

```
repeat
  stmts...
until cond
```

```
for cond step n
  stmts...
endfor
```

### Function definition

let f(x) -> x + 1

fn g() 
  for 0 <= x < 10
    print x, ' ',
  endfor
endfn

### Function call

b <- f(b)
g()

### Class definition

class c
  fn init(x, y) ->
    this.my_x <- x
    this.my_y <- y
    this.immutable = 42
  endfn

  fn i(d) ->
    print my_x * d
    print my_y * d
  endfn
endclass

### Function reassingment

let k <- g
k <- f
k(42)

### Comments

\# This is a comment
