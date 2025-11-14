var http = require('http');
var fs = require('fs');
var url = require('url');
var DbModule = require('./DbModule');
const PORT = 5000;

var db = new DbModule.DB();

db.on('GET', async(request, response) => {
    console.log("GET called");
    response.writeHead(200,{'content-type':'application/json;charset=utf-8'});
    response.end(JSON.stringify(db.select()))
});


db.on('POST', async (request, response,body) => {
    console.log("POST called");
    let parsedBody = JSON.parse(body);
    db.insert(parsedBody);
    response.writeHead(200,{'content-type':'application/json;charset=utf-8'});
    response.end(JSON.stringify(parsedBody));
});


db.on('PUT',async(request,response,body)=>{
    console.log("PUT called");
    let parsedBody = JSON.parse(body);
    db.update(parsedBody);
    response.writeHead(200,{'content-type':'application/json;charset=utf-8'});
    response.end(JSON.stringify(parsedBody));
})


db.on('DELETE',async(request,response)=>{
    console.log("DELETE called");
    let parsedURL = url.parse(request.url,true);
    const id = parsedURL.query.id;
    if(!id){
        response.writeHead(400,{'content-type':'text/html;charset=utf-8'});
        response.end('<h1>400 Bad request</h1>')
    }
    const deletedRow = db.delete(id);
    response.writeHead(200,{'content-type':'application/json;charset=utf-8'});
    response.end(JSON.stringify(deletedRow));
});


const server = http.createServer(function (request,response){

    let body = '';
    

    if(url.parse(request.url).pathname==="/api/db"){
        let body='';
        request.on('data',data=>{
           body+=data;
        })
        request.on('end',()=>{
        db.emit(request.method,request,response,body);
        });
        
    }
    else if(request.url==="/"){
        let page = fs.readFileSync('./index.html');
        response.writeHead(200,{'content-type':'text/html;charset=utf-8'});
        response.end(page);
    }
    else{
        response.writeHead(404,{'content-type':'text/html'});
        response.end(
            "<h1>404 Not Found</h1>"
        );
    }
});

server.listen(PORT);


console.log("Server running at http://localhost:5000/");