const WebSocket = require("ws");
const fs = require("fs");

let ws = new WebSocket("ws://localhost:4000");

ws.on("open", () => {
  let duplex = WebSocket.createWebSocketStream(ws, { encoding: "utf8" });
  let rfile = fs.createReadStream(`./static/11-01.txt`);
  rfile.pipe(duplex);
});

ws.on("error", (e) => {
  console.log(`error: ${e}`);
});
