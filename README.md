# affectiva websocket API

this is a websocket server that receives webcam data, in webm files, over a websocket connection. it replies with data from the [Affectiva SDK](http://www.affectiva.com).

## installing

    npm install

### dependencies
Get the [Affectiva SDK]( http://www.affectiva.com).

Install it to `C:\\Program Files (x86)\\Affectiva\\`

Place your SDK license in `C:\\Program Files (x86)\\Affectiva\\Affdex SDK\\affdex.license`

### generating SSL keys on windows

somewhat wisely, chrome requires https for all sites that ask for webcam access.

you can generate your own SSL cert on Windows with [OpenSSL](https://www.openssl.org/)

first, set to the conf file in C:/

    set OPENSSL_CONF=c:/openssl.cnf


now, generate a key and cert

    openssl.exe req -x509 -newkey rsa:2048 -keyout key-pass.pem -out cert.pem -days 365

remove pass from key-pass.key, producing key.pem

    openssl.exe rsa -in key-pass.pem -out key.pem

now, put `key.pem` and `key.cert` in the root folder, and the https servers should work


## running

    npm start
    npm run server


## executable source files

The source files for the executable can be found at [huynhing/emotions-exec](https://github.com/huynhing/emotions-exec)

NOTE
----
Static files can be served in `dist/`.  Drop any html file in dist/ "npm start" to serve dist/ over HTTPS using the same key and cert as the server
