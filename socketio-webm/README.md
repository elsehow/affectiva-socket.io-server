## executable dependencies
    affectiva sdk:
        http://www.affectiva.com
        Should be installed to "C:\\Program Files (x86)\\Affectiva\\
    affectiva sdk license
        Should be placed in "C:\\Program Files (x86)\\Affectiva\\Affdex SDK\\affdex.license"

## executable source files
    The source files for the executable can be found in the following repo:
        https://github.com/huynhing/emotions-exec

## installing

    npm install

## running

    npm start
    npm run server

NOTE
----
- Static files can be served in dist/

- Drop any html file in dist/ "npm start" to serve dist/ over HTTPS using the same key and cert as the server
