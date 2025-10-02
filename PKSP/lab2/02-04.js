var http = require('http');
var fs = require('fs');
const SURENAME = 'Кучерук';
const NAME = 'Николай';
const LASTNAME = 'Петрович';


const server = http.createServer(function (request, response) {
    if (request.url === "/xmlhttprequest") {
        let xmlhttprequest = fs.readFileSync('./xmlhttprequest.html');
        response.writeHead(200, { 'content-type': 'text-html;charset=utf-8' });
        response.end(xmlhttprequest);
    }
    else if (request.url === "/api/name") {
        response.writeHead(200, { 'content-type': 'text/plain; charset=utf-8' });
         response.end(SURENAME+' '+ NAME+' '+LASTNAME);
    }
    else {
        let error = fs.readFileSync('./error.html');
        response.writeHead(404, { 'content-type': 'text/html; charset=utf-8' })
        response.end(error);
    }
}
);

server.listen(5000);

console.log("Server running at http://localhost:5000/xmlhttprequest");