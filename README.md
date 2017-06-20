# piperun - Run ELF binary code read from standard input.

**piperun** is a simple C program than allows you to
execute a compiled ELF binary read from standard input.

## Usage
```sh
gcc -c t/hello.c -o /dev/stdout | ./piperun
```

or

```sh
make check
```


Simply run `make` then pipe a compiled ELF binary to `./pipeprun` and marvel
as it is executed! Alternately, the above test can also be run with a simple
`make check` command.

## How?

**piperun** uses a syscall-created memfd followed by an fexecve(3)
call to execute your code.

## But why?:

* There is no other good way (that I know of) to execute an ELF binary without writing it to a temporary file first.
* Using `/lib/ld-linux-x86-64.so.2` to execute code also only works with temporary files due to it's usage of mmap(3).
* **BECAUSE IT'S COOL AF**
