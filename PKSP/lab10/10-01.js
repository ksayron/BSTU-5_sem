const http = require('http');
const fs = require('fs');
const ws = require('ws');


const httpPort = 3000;
const httpServer = http.createServer((req, res) => {
  if (req.method === 'GET' && req.url === '/start') {
          res.writeHead(200, { "Content-Type": "text/html; charset=utf-8" });
          let html = fs.readFileSync("startWS.html");
          res.end(html);
    return;
  }


  res.writeHead(400, { 'Content-Type': 'text/plain; charset=utf-8' });
  res.end('Bad request');
});

httpServer.listen(httpPort, () => {
  console.log(`server listening on http://localhost:${httpPort}/start`);
});


const wsPort = 4000;
const wss = new ws.Server({ port: wsPort }, () => {
  console.log(`ws server listening on ws://localhost:${wsPort}`);
});


const clients = new Map();

wss.on('connection', (ws, req) => {
  console.log('New WS connection from', req.socket.remoteAddress + ':' + req.socket.remotePort);
  clients.set(ws, { lastClientNum: null, serverSeq: 0 });
  ws.send('New WS connection from', req.socket.remoteAddress + ':' + req.socket.remotePort)
  ws.on('message', msg => {
    const text = msg.toString();
    console.log('Received from client:', text);
    console.log(text[text.length-1])
    const m = text[text.length-1];
    clients.get(ws).lastClientNum = m;
  });

  ws.on('close', () => {
    console.log('WS closed for client');
    clients.delete(ws);
  });

  ws.on('error', err => {
    console.log('WS error', err);
    clients.delete(ws);
  });
});

setInterval(() => {
  for (const [ws, state] of clients.entries()) {
    if (ws.readyState === ws.OPEN) {
      state.serverSeq++;
      const n = state.lastClientNum === null ? 0 : state.lastClientNum;
      const k = state.serverSeq;
      const out = `10-01-server: ${n}->${k}`;
      ws.send(out);
      console.log('Sent to client:', out);
    }
  }
}, 5000);