# Nom

[![Build Status](https://upload.wikimedia.org/wikipedia/commons/f/f8/License_icon-mit-88x31-2.svg)]()
[![Build Status](https://travis-ci.org/Mithreindeir/nom.svg?branch=master)](https://travis-ci.org/Mithreindeir/Nom)


Nom lang is a dynamically typed lightweight functional language focusing on simplicity and embeddability. It is lexically scoped, however only functions are considered code blocks. At the moment automatic memory managment is limited to reference counting, so avoid circular references. The Standard Library is currently being implemented, and not all functionality is there.

## Install
To install on linux, run the install.sh script. Any program you make will require you to type "nom program_name" or just add #!/usr/bin/nom to the beginning of the scripts to specify which interpreter to use. REPL isn't well tested. Examples are in tests folder.


Example factorial program written in Nom:


    var factorial = function(n):
      if n <= 1:
        return n
      end
      else:
        return n * factorial(n-1)
      end
    end
    
    var f = factorial(5)
    print(f)

## Nom vm
Nom runs on a stack based virual machine, and nom code is compiled to nom bytecode before it is executed in the vm, so technically it could be adapted to be a compiled language, but for now it is interpreted.

## Basic usage
A codeblock starts with a colon and ends with an end statement. This is used for everything that would require a codeblock, eg: conditionals, loops, functions.

One line comments can be used with "#"

If, else if and else statements

    if condition:
        #Do something
    end
    elseif condition:
        #Do something else
    end
    else:
        #DO something else
    end
Loops
  
    while condition:
        #do this
    end
    for var i=start, i<end, i++:
        #Do something
    end

Functions

    var foo = function(arg):
        #Do things
    end
    
    foo(5)
Objects
    
    var foo = array()
    foo.a = 5
    foo.b = "Dog"
    var b = foo
    print(b.a, " ", b.b, "\n")
Arrays

    var arr = array()
    arr[0] = "Dog"
    arr[1] = 1
    arr[2].a = "abcdef"
    var s=size(arr)
