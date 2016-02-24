# make the mock executable
chmod +x affectiva-cli-mock.js
# test the cli specs on the mock
node affectiva-cli.specs.js -b 'affectiva-cli-mock.js' | ./node_modules/.bin/tap-spec

chmod +x server.js
node server.specs.js | ./node_modules/.bin/tap-spec
