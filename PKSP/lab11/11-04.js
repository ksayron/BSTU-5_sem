let WebSocket = require("ws");

let wss = new WebSocket.Server({ port: 4000, host: "localhost" });
let k = 0;

wss.on("connection", (ws) => {
  ws.on("message", (msg) => {
    console.log(`message: ${msg.toString()}`);
    let params = JSON.parse(msg);
    ws.send(JSON.stringify({ server: ++k, client: params.client, timestamp: new Date().toISOString() }, null, 2));
  });
});

wss.on("error", (err) => {
  console.log("error: ", err);
});
