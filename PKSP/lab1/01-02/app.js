const http = require('http');

http.createServer(function (request, response) {
    response.writeHead(200, { 'Content-Type': 'text/html; charset=utf-8' });
    response.end('Hello World!');
}).listen(3000);

console.log('Server active: http://localhost:3000/')