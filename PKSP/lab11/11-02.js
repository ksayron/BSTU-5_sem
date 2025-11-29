let WebSocket = require("ws");
let fs = require("fs");

let wss = new WebSocket.Server({ port: 4000, host: "localhost" });

wss.on("connection", (ws) => {
  let duplex = WebSocket.createWebSocketStream(ws, { encoding: "utf8" });
  let rfile = fs.createReadStream(`./download/11-02.txt`);
  rfile.pipe(duplex);
});

wss.on("error", (e) => {
  console.log(`error: ${e}`);
});
