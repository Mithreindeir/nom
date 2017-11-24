# Nom

[![Build Status](https://upload.wikimedia.org/wikipedia/commons/f/f8/License_icon-mit-88x31-2.svg)]()
[![Build Status](https://travis-ci.org/Mithreindeir/nom.svg?branch=master)](https://travis-ci.org/Mithreindeir/Nom)


Nom lang is a dynamically typed lightweight functional language focusing on simplicity and embeddability.
To install on linux, run the install.sh script. Any program you make will require you to type "nom program_name" or just add #!/usr/bin/nom to the beginning of the scripts to specify which interpreter to use. REPL isn't well tested. Examples are in tests folder.


Example factorial program written in Nom:


    factorial = function(n):
      if n <= 1:
        return n
      end
      else:
        return n * factorial(n-1)
      end
    end
    
    f = factorial(5)
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
    for i=start, i<end, i++:
        #Do something
    end

Functions

    foo = function(arg):
        #Do things
    end
    
    foo(5)
Objects
    
    foo.a = 5
    foo.b = "Dog"
    b = foo
    print(b.a, " ", b.b, "\n")
Arrays

    arr[0] = "Dog"
    arr[1] = 1
    #Arrays that hold objects cannot directly reference their members (use temp var)
    obj.a = "abcdef"
    arr[2] = obj
    s=size(arr)
