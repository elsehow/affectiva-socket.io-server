# concaminate system tests

these are end-to-end tests for the concaminate system

## installation

    npm install

## running the tests

    npm test

# tests

the remainder of this readme explains each test we do

## 1. test affectiva executable

we expect the executable to print some json over stdout
we expect it to print something like this (though potentially minified):

    {
      "expressions": { "smile": 1, ... }
      "emotions": { "joy": 1, ... }
    }

this test:

- checks for errors when running the executable
- assures the result from executable can be parsed as json
- assures the json contains all our expression and emotion fields

## license

BSD
