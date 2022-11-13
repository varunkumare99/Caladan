# Caladan
- Language built using LLVM.
## Features
- Functions
- optimizations (peephole, reassociate expressions, eliminate common
   subexpressions, delete unreachable blocks in CFG etc)
- if-else
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
- [ ] for
- [ ] break
- [ ] continue
- [ ] while
- [ ] switch
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
