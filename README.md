# Nom

[![Build Status](https://upload.wikimedia.org/wikipedia/commons/f/f8/License_icon-mit-88x31-2.svg)]()
[![Build Status](https://travis-ci.org/Mithreindeir/VelocityRaptor.svg?branch=master)](https://travis-ci.org/Mithreindeir/VelocityRaptor)


Nom lang is a dynamically typed lightweight functional language focusing on simplicity and embeddability.
To install on linux, run the install.sh script. Any program you make will require you to type "nom program_name" or just add #!/usr/bin/nom to the beginning of the scripts to specify which interpreter to use. No REPL loop yet available.


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
