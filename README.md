# Caladan
- Language built using LLVM.
## Features
- Functions
- optimizations (peephole, reassociate expressions, eliminate common
   subexpressions, delete unreachable blocks in CFG etc)
- if-else expression
- for expression
- mutable variables
- switch expression

### Types
- int 64bit

### Operators
- add(+)
- minus(-)
- multiplication(*)
- division(/)
- remaineder (%)
- less than (<)
- greater than (>)

### TODO
- [ ] break
- [ ] continue
- [ ] while
- [ ] list of expressions
### Build
```
mkdir build
cd build
cmake -GNinja ..
ninja
./caladan
```
### Examples

```
>>> func foo(x y) {
	x*x + 2*x*y + y*y
}

>>> foo(3, 4);
Result : 49

```
```
>>> func foo(x) {
	if x > 10 {
		x + 20
	}
	else {
		x + 10
	}
}

>>> foo(30);
Result : 50
```
``` 
>>> func foo(x) {
		for i=1, i < x, 2 {
			printi(x)
		}
	}
	
>>> foo(10);
1
3
5
7
9
```

```
>>> func foo(x) {
		var a=2, b=2 {
			for i = 1, i < x, 2 {
				printi(i + a + b)
			}
		}
	}
	
>>> foo(10);
6
8
10
12
14
```
```
>>> func foo(x y) {
		switch(x) {
			case 5:
				printi(x + y)
			case 6:
				printi(x - y)
			default:
				printi(x % y)
		}
	}

>>> foo(5, 7);
12

>>> foo(6, 23);
-17

>>> foo(100, 7);
2
```
```
>>> func factorial(x) {
		if x < 1 {
			1
		}
		else {
			x * factorial(x-1)
		}
	}
	
>>> factorial(10);
36288000
```
