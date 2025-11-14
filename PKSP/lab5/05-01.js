var http = require('http');
var fs = require('fs');
var url = require('url');
var DbModule = require('./DbModule');
const PORT = 5000;

var db = new DbModule.DB();

let serverStats = {
    time_start:null,
    time_finish:null,
    request_count:0,
    commit_count:0
}

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
    db.insert(parsedBody);
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

db.on('COMMIT', async () => {
    console.log("DB.COMMIT triggered");
    serverStats.commit_count = db.commit();
});


const server = http.createServer(function (request,response){
    if (request) {
        serverStats.request_count++;
    }
    if(url.parse(request.url).pathname==="/api/db"){
        let body='';
        request.on('data',data=>{
           body+=data;
        })
        request.on('end',()=>{
        db.emit(request.method,request,response,body);
        });
        
    }
        else if (url.parse(request.url).pathname === "/api/ss") {
        response.writeHead(200, { 'content-type': 'application/json;charset=utf-8' });
        collectStatistics(0);
        response.end(JSON.stringify(serverStats));
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

let stopTimeout;
const stopServer = (delay) => {
    stopTimeout = setTimeout(() => {
        console.log("Timeout passed. Exiting the application...");
        process.exit(0);

    }, delay);
    stopTimeout.unref();
}

let commitInterval;
const startCommiting = (interval) => {
    commitInterval = setInterval(() => {
        db.emit('COMMIT');
        console.log("Commit operation executed");
    }, interval);
    commitInterval.unref();
}

let statisticsTimeout;
const collectStatistics = (collDelay) => {
    serverStats.time_start = new Date(Date.now()).toISOString();
    statisticsTimeout = setTimeout(() => {
        serverStats.time_finish = new Date(Date.now()).toISOString();
        console.log("Statistic collected");
        console.log(serverStats);
    }, collDelay);

    statisticsTimeout.unref();
}

process.stdin.setEncoding('utf8');
process.stdin.resume();

process.stdout.write('--> ');

process.stdin.on('readable', () => {
    let chunk;
    while ((chunk = process.stdin.read()) !== null) {
        const lines = chunk.split(/\r?\n/);

        for (let i = 0; i < lines.length; i++) {
            const rawLine = lines[i];

            if (rawLine.trim() === '') {
                if (i === lines.length - 1) process.stdout.write('--> ');
                continue;
            }

            changeState(rawLine);
        }
    }
});

function changeState(line) {
    const input = line.trim().toLowerCase();

    if (input === '') {
        process.stdout.write('--> ');
        return;
    }

    if (input === 'exit') {
        console.log('Exiting the application...');
        process.exit(0);
    }

    let command = input.slice(0, 2);
    let flag = input.slice(3);

    let invalid_flag = false;

    if (isNaN(flag)) {
        console.log("Invalid flag");
        invalid_flag = true;
        changeState();
    }

    let command_NoFlag = false;
    if (flag == "") {
        command_NoFlag = true;
        console.log(`No flag for ${command} command`);
    }


    if (!invalid_flag) {
        switch (command) {
            case 'sd':
                clearTimeout(stopTimeout);
                if (!command_NoFlag) {
                    const duration = parseInt(flag, 10);
                    if (!isNaN(duration)) {
                        stopServer(duration);
                    } else {
                        console.log('Invalid flag for sd');
                        console.log('Server stop aborted');
                    }
                } else {
                    clearTimeout(stopTimeout);
                    console.log('Server stop aborted');
                }
                break;

            case 'sc':
                clearInterval(commitInterval);
                if (!command_NoFlag) {
                    const interval = parseInt(flag, 10);
                    if (!isNaN(interval)) {
                        startCommiting(interval);
                    } else {
                        console.log('Invalid flag for sc');
                        console.log('Commit execution aborted');
                    }
                } else {
                    clearInterval(commitInterval);
                    console.log('Commit execution aborted');
                }
                break;

            case 'ss':
                clearTimeout(statisticsTimeout);
                if (!command_NoFlag) {
                    const statTimeout = parseInt(flag, 10);
                    if (!isNaN(statTimeout)) {
                        collectStatistics(statTimeout);
                    } else {
                        console.log('Invalid flag for ss');
                        console.log('Statistics collection aborted');
                    }
                } else {
                    clearTimeout(statisticsTimeout);
                    console.log('Statistics collection aborted');
                }
                break;

            default:
                console.log(`Unknown command: ${command}`);
        }
    }
}